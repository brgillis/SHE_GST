"""
    @file generate_images.py

    Created 23 Jul 2015

    This module contains the functions which do the heavy lifting of actually
    generating images.
"""

__updated__ = "2019-09-04"

# Copyright (C) 2012-2020 Euclid Science Ground Segment
#
# This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General
# Public License as published by the Free Software Foundation; either version 3.0 of the License, or (at your option)
# any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


from copy import deepcopy
from multiprocessing import cpu_count, Pool
import os

from SHE_PPT import detector
from SHE_PPT import products
from SHE_PPT.file_io import (get_allowed_filename, write_listfile, append_hdu, write_pickled_product,
                             write_xml_product, find_file_in_path, find_file)
from SHE_PPT.logging import getLogger
from SHE_PPT.magic_values import (gain_label, stamp_size_label, model_hash_label,
                                  model_seed_label, noise_seed_label, extname_label, ccdid_label, dither_dx_label,
                                  dither_dy_label, scale_label,
                                  sci_tag, noisemap_tag, mask_tag, segmentation_tag, details_tag,
                                  detections_tag, bulge_psf_tag, disk_psf_tag, background_tag, psf_im_tag)
from SHE_PPT.table_formats.details import initialise_details_table, details_table_format as datf
from SHE_PPT.table_formats.detections import initialise_detections_table, detections_table_format as detf
from SHE_PPT.table_formats.psf import initialise_psf_table, psf_table_format as pstf
from SHE_PPT.table_utility import add_row, table_to_hdu
from SHE_PPT.utility import hash_any
import galsim
import h5py

import SHE_GST
from SHE_GST_GalaxyImageGeneration import magic_values as mv
from SHE_GST_GalaxyImageGeneration.combine_dithers import (combine_image_dithers,
                                                           combine_segmentation_dithers)
from SHE_GST_GalaxyImageGeneration.config.check_config import get_full_options
from SHE_GST_GalaxyImageGeneration.cutouts import make_cutout_image
from SHE_GST_GalaxyImageGeneration.dither_schemes import get_dither_scheme
from SHE_GST_GalaxyImageGeneration.galaxy import (get_bulge_galaxy_profile,
                                                  get_disk_galaxy_profile,
                                                  is_target_galaxy)
from SHE_GST_GalaxyImageGeneration.magnitude_conversions import get_I
from SHE_GST_GalaxyImageGeneration.noise import get_var_ADU_per_pixel, add_stable_noise
from SHE_GST_GalaxyImageGeneration.psf import (get_psf_profile, sort_psfs_from_archive, add_psf_to_archive,
                                               load_psf_model_from_file, single_psf_filename)
from SHE_GST_GalaxyImageGeneration.segmentation_map import make_segmentation_map
from SHE_GST_GalaxyImageGeneration.signal_to_noise import get_signal_to_noise_estimate
from SHE_GST_GalaxyImageGeneration.wcs import get_wcs_from_image_phl
import SHE_GST_PhysicalModel
from astropy import table
from astropy.io import fits
import numpy as np


model_hash_maxlen = 17  # Maximum possible length within filenames


default_gsparams = galsim.GSParams(folding_threshold=5e-3,
                                   maxk_threshold=1e-3,
                                   kvalue_accuracy=1e-5,
                                   stepk_minimum_hlr=5,
                                   )


class generate_image_group_with_options_caller(object):

    def __init__(self, *args, **kwargs):
        self.args = args
        self.kwargs = kwargs

    def __call__(self, x):
        return generate_image_group(x, *self.args, **self.kwargs)


def generate_images(survey, options):
    """
        @brief This function handles assigning specific images to be created by different parallel
            threads.

        @details If successful, generates images and corresponding details according to
            the configuration stored in the survey and options objects.

        @param survey
            <SHE_GST_PhysicalModel.Survey> The survey object which specifies parameters for generation
        @param options
            <dict> The options dictionary for this run
    """
    logger = getLogger(__name__)
    logger.debug("Entering generate_images method.")

    # Seed the survey
    if options['seed'] == 0:
        survey.set_seed()  # Seed from the time
    else:
        survey.set_seed(options['seed'])

    # Create empty image objects for the survey
    survey.fill_image_groups()
    image_groups = survey.get_image_groups()

    # Multiprocessing doesn't currently work, so print a warning if it's requested

    if options['num_parallel_threads'] != 1:
        logger.warning("Multi-processing is not currently functional; it requires features that " +
                       "will be available in Python 3. Until then, if you wish to use multiple " +
                       "processes, please call this program multiple times with different seed " +
                       "values. Continuing with a single process...")
        options['num_parallel_threads'] = 1

    # If we just have one thread, we'll just use a simply function call to ease debugging
    if options['num_parallel_threads'] == 1:
        for image_group in image_groups:
            generate_image_group(image_group, options)
    else:
        if options['num_parallel_threads'] <= 0:
            options['num_parallel_threads'] += cpu_count()

        pool = Pool(processes=cpu_count(), maxtasksperchild=1)
        pool.map(generate_image_group_with_options_caller(options), image_group, chunksize=1)

    logger.debug("Exiting generate_images method.")

    return


class ProductFilenames(object):

    def __init__(self, is_image=False):
        self.prod_filenames = []
        self.data_filenames = []

        if is_image:
            self.bkg_filenames = []
            self.wgt_filenames = []

        return


def generate_image_group(image_group_phl, options):
    """
    Generate a FOV and save it in a multi-extension FITS file.

    Args:
        image_group_phl: <SHE_GST_PhysicalModel.ImageGroup> Physical model for the image_phl group
        options: <dict> Options dictionary

    Returns:
        None
    """

    workdir = options['workdir']

    image_group_phl.fill_images()

    # Get the model hash so we can set up filenames
    full_options = get_full_options(options, image_group_phl.get_image_descendants()[0])

    num_dithers = len(get_dither_scheme(options['dithering_scheme']))

    image_filenames = ProductFilenames(is_image=True)
    detections_filenames = ProductFilenames()
    details_filenames = ProductFilenames()
    mosaic_filenames = ProductFilenames()
    psf_filenames = ProductFilenames()

    full_options = get_full_options(options, image_group_phl)
    model_hash = hash_any(full_options, format="base64")
    model_hash_fn = model_hash[0:model_hash_maxlen].replace('.', '-').replace('+', '-')
    psf_archive_filename = get_allowed_filename("PSF-ARCHIVE", model_hash_fn, extension=".hdf5",
                                                version=SHE_GST.__version__)

    if ((options['output_psf_file_name'] is None or options['output_psf_file_name'] == 'None') and
            (options['model_psf_file_name'] is None or options['model_psf_file_name'] == 'None') and
            not options['single_psf']):
        psf_archive_filehandle = h5py.File(os.path.join(workdir, psf_archive_filename), 'a')
    else:
        psf_archive_filehandle = None

    # Get the filenames we'll need
    for i in range(num_dithers):
        for filename_list, tag in ((image_filenames, sci_tag),
                                   (detections_filenames, detections_tag),
                                   (details_filenames, details_tag),
                                   (mosaic_filenames, segmentation_tag),
                                   (psf_filenames, psf_im_tag),):

            # For products that aren't lists, don't include the dither label
            if tag in (detections_tag, details_tag):
                dither_tag = ""
            else:
                dither_tag = "-D" + str(i + 1)

            subfilenames_lists_labels_exts = [(filename_list.prod_filenames, "GST-P", ".xml"),
                                              (filename_list.data_filenames, "GST", ".fits"), ]

            # For the VIS image, also add other subfilenames
            if tag == sci_tag:
                subfilenames_lists_labels_exts.append((filename_list.bkg_filenames, "GST-BKG", ".fits"))
                subfilenames_lists_labels_exts.append((filename_list.wgt_filenames, "GST-WGT", ".fits"))

            for (subfilename_list, label, extension) in subfilenames_lists_labels_exts:

                filename = get_allowed_filename(label + "-" + tag + dither_tag, model_hash_fn, extension=extension,
                                                version=SHE_GST.__version__)
                subfilename_list.append(filename)

                # If it exists already, delete it
                qualified_filename = os.path.join(options['workdir'], filename)
                if os.path.exists(qualified_filename):
                    os.remove(qualified_filename)

    # Set up XML products we're outputting
    for i in range(num_dithers):

        # Image product

        image_product = products.calibrated_frame.create_dpd_vis_calibrated_frame()
        image_product.set_data_filename(image_filenames.data_filenames[i])
        image_product.set_bkg_filename(image_filenames.bkg_filenames[i])
        image_product.set_wgt_filename(image_filenames.wgt_filenames[i])

        write_xml_product(image_product, image_filenames.prod_filenames[i], workdir=workdir)

        # Segmentation map

        mock_mosaic_product = products.exposure_mosaic.create_exposure_mosaic_product(
            data_filename=mosaic_filenames.data_filenames[i])

        write_xml_product(mock_mosaic_product, mosaic_filenames.prod_filenames[i], workdir=workdir)

        # PSF catalogue and images

        psf_product = products.psf_image.create_dpd_she_psf_image(filename=psf_filenames.data_filenames[i])

        write_xml_product(psf_product, psf_filenames.prod_filenames[i], workdir=workdir)

        # Detections table
        if i == 0:
            my_detections_product = products.detections.create_detections_product(
                data_filename=detections_filenames.data_filenames[i])
            write_xml_product(my_detections_product, detections_filenames.prod_filenames[i], workdir=workdir)

    # end for i in range(num_dithers):

    # Set up combined tables
    details_tables = []
    detections_tables = []
    psf_tables = []
    for i in range(num_dithers):
        psf_tables.append([])

    image_i = -1

    # Generate each image_phl, then append it and its data to the fits files
    for image_phl in image_group_phl.get_image_descendants():

        image_i += 1
        wcs_list = []

        for i in range(num_dithers):
            wcs_list.append(get_wcs_from_image_phl(image_phl,
                                                   dither_offset=get_dither_scheme(options['dithering_scheme'])[i]))

        # Generate the data
        (image_dithers, noise_maps, mask_maps, wgt_maps, bkg_maps, segmentation_maps,
         detections_table, details_table) = generate_image(image_phl, options, wcs_list, psf_archive_filehandle)

        # Append to the fits file for each dither
        if not options['details_only']:
            for i in range(num_dithers):

                workdir = options['workdir']

                qualified_image_filename = os.path.join(workdir, image_filenames.data_filenames[i])

                # Science image
                im_hdu = fits.ImageHDU(data=image_dithers[i].array,
                                       header=fits.header.Header(list(image_dithers[i].header.items())))
                append_hdu(qualified_image_filename, im_hdu)

                # Noise map
                rms_hdu = fits.ImageHDU(data=noise_maps[i].array,
                                        header=fits.header.Header(list(noise_maps[i].header.items())))
                append_hdu(qualified_image_filename, rms_hdu)

                # Mask map
                flg_hdu = fits.ImageHDU(data=mask_maps[i].array,
                                        header=fits.header.Header(list(mask_maps[i].header.items())))
                append_hdu(qualified_image_filename, flg_hdu)

                # Background map
                bkg_hdu = fits.ImageHDU(data=bkg_maps[i].array,
                                        header=fits.header.Header(list(bkg_maps[i].header.items())))
                append_hdu(os.path.join(workdir, image_filenames.bkg_filenames[i]), bkg_hdu)

                # Weight map
                wgt_hdu = fits.ImageHDU(data=wgt_maps[i].array,
                                        header=fits.header.Header(list(wgt_maps[i].header.items())))
                append_hdu(os.path.join(workdir, image_filenames.wgt_filenames[i]), wgt_hdu)

                # Segmentation map

                seg_hdu = fits.ImageHDU(data=segmentation_maps[i].array,
                                        header=fits.header.Header(list(segmentation_maps[i].header.items())))
                append_hdu(os.path.join(workdir, mosaic_filenames.data_filenames[i]), seg_hdu)

                # PSF catalogue and images

                num_rows = len(details_table[datf.ID])
                psf_table = initialise_psf_table(image_phl.get_parent(), options,
                                                 init_columns={pstf.ID: details_table[datf.ID],
                                                               pstf.template: -1 * np.ones(num_rows, dtype=np.int64),
                                                               pstf.bulge_index: -1 * np.ones(num_rows, dtype=np.int32),
                                                               pstf.disk_index: -1 * np.ones(num_rows, dtype=np.int32)})

                psf_tables[i].append(psf_table)

        # Tables to combine

        details_tables.append(details_table)
        detections_tables.append(detections_table)

    # end for image_phl in image_group_phl.get_image_descendants():

    # Output combined tables

    combined_details_table = table.vstack(details_tables)
    dal_hdu = table_to_hdu(combined_details_table)
    append_hdu(os.path.join(workdir, details_filenames.data_filenames[0]), dal_hdu)

    combined_detections_table = table.vstack(detections_tables)
    dtc_hdu = table_to_hdu(combined_detections_table)
    append_hdu(os.path.join(workdir, detections_filenames.data_filenames[0]), dtc_hdu)

    # Output data products for tables

    details_prod = products.details.create_details_product(details_filenames.data_filenames[0])
    write_xml_product(details_prod, options['details_table'], workdir=workdir)

    detections_prod = products.detections.create_detections_product(detections_filenames.data_filenames[0])
    write_xml_product(detections_prod, detections_filenames.prod_filenames[0], workdir=workdir)
    write_listfile(os.path.join(workdir, options['detections_tables']), [detections_filenames.prod_filenames[0]])

    if not options['details_only']:

        combined_psf_tables = []

        for i in range(num_dithers):

            combined_psf_tables.append(table.vstack(psf_tables[i]))

            if options['output_psf_file_name'] is None or options['output_psf_file_name'] == 'None':
                if options['model_psf_file_name'] is not None and options['model_psf_file_name'] != 'None':
                    output_psf_file_name = options['model_psf_file_name']
                elif options['single_psf']:
                    output_psf_file_name = single_psf_filename
                else:
                    output_psf_file_name = None
            else:
                output_psf_file_name = options['output_psf_file_name']

            sort_psfs_from_archive(psf_table=combined_psf_tables[i],
                                   psf_data_filename=psf_filenames.data_filenames[i],
                                   archive_filehandle=psf_archive_filehandle,
                                   output_psf_filename=output_psf_file_name,
                                   exposure_index=i,
                                   stamp_size=options['psf_stamp_size'],
                                   scale=image_group_phl.get_param_value("pixel_scale") / options['psf_scale_factor'],
                                   workdir=workdir)

        # Output listfiles of filenames
        write_listfile(os.path.join(options['workdir'], options['data_images']), image_filenames.prod_filenames)
        write_listfile(os.path.join(options['workdir'],
                                    options['segmentation_images']), mosaic_filenames.prod_filenames)
        write_listfile(os.path.join(options['workdir'],
                                    options['psf_images_and_tables']), psf_filenames.prod_filenames)

        # If we're dithering, create stacks
        if num_dithers > 1:

            combine_image_dithers(options['data_images'],
                                  options['stacked_data_image'],
                                  options['dithering_scheme'],
                                  workdir=options['workdir'])

            combine_segmentation_dithers(options['segmentation_images'],
                                         options['stacked_segmentation_image'],
                                         options['dithering_scheme'],
                                         workdir=options['workdir'])

    # Remove the now-unneeded PSF archive file
    del psf_archive_filehandle
    if os.path.exists(os.path.join(workdir, psf_archive_filename)):
        os.remove(os.path.join(workdir, psf_archive_filename))

    return


def print_galaxies(image_phl,
                   options,
                   wcs_list,
                   centre_offset,
                   num_dithers,
                   dithers,
                   full_x_size,
                   full_y_size,
                   pixel_scale,
                   detections_table,
                   details_table,
                   psf_archive_filehandle):
    """
        @brief Prints galaxies onto a new image and stores details on them in the output table.

        @param image_phl
            <SHE_GST_PhysicalModel.Image> Image-level object which will generate galaxies to print
        @param options
            <dict> Options dictionary for this run
        @param centre_offset
            <float> The difference between Galsim's stated centres and the actual centres
        @param num_dithers
            <int> How many dithers there are
        @param dithers
            <list<>> A list which will be populated with a galsim.Image for each dither
        @param full_x_size
            <int> The size in pixels of the x-axis of the generated images
        @param full_y_size
            <int> The size in pixels of the y-axis of the generated images
        @param pixel_scale
            <float> The scale of pixels in the generated images in arcsec/pixel
        @param detections_table
            <astropy.Table> The table containing mock galaxy detections (ID and position),
                            to be filled
        @param details_table
            <astropy.Table> The table containing details on each galaxy, to be filled.

        @returns galaxies
            <SHE_GST_PhysicalModel.galaxy_list> Iterable list of the galaxies which were printed.
    """

    logger = getLogger(__name__)
    logger.debug("Entering 'print_galaxies' function.")

    # Get some data out of the options
    model_psf_offset = (options["model_psf_x_offset"], options["model_psf_y_offset"])

    # Get the galaxies we'll be drawing
    galaxies = image_phl.get_galaxy_descendants()

    background_galaxies = []
    target_galaxies = []

    # Since all WCSs are uniform so far, we just use a single jacobian WCS for profile transformations
    jacobian_wcs = wcs_list[0].jacobian(image_pos=galsim.PositionD(0., 0.))

    # Seed the python RNG
    np.random.seed(image_phl.get_full_seed() % 2**32)

    # Generate parameters first (for consistent rng)

    logger.debug("Generating galaxy parameters.")
    for galaxy in galaxies:
        galaxy.generate_parameters()

        # Sort out target galaxies
        if is_target_galaxy(galaxy, options):
            target_galaxies.append(galaxy)
        else:
            background_galaxies.append(galaxy)
    logger.debug("Finished generating galaxy parameters.")

    num_background_galaxies = len(background_galaxies)
    num_target_galaxies = len(target_galaxies)

    # If we're aiming for a certain number of target galaxies, adjust as necessary
    if options['num_target_galaxies'] > 0:
        if num_target_galaxies <= 0:
            if options['render_background_galaxies']:
                raise ValueError("Image too small to accurately represent background galaxies")
            num_ratio = 1e99
        else:
            num_ratio = options['num_target_galaxies'] * (1. / num_target_galaxies)

        if num_ratio > 1:

            # Add new galaxies
            logger.debug("Adjusting number of target galaxies upward.")
            num_extra_target_galaxies = options['num_target_galaxies'] - num_target_galaxies
            if options['mode'] == 'stamps' and options['render_background_galaxies']:
                num_extra_background_galaxies = int((num_ratio - 1) * num_background_galaxies)
            else:
                num_extra_background_galaxies = 0

            num_new_target_galaxies = 0
            num_new_background_galaxies = 0

            field = image_phl.get_field_descendants()[0]

            while ((num_new_target_galaxies < num_extra_target_galaxies) or
                   (num_new_background_galaxies < num_extra_background_galaxies)):

                new_galaxy = field.add_galaxy()

                bad_type = True

                while bad_type:
                    new_galaxy.clear()
                    new_galaxy.generate_parameters()

                    # Check what type it is, and if we can add another galaxy of that type

                    if is_target_galaxy(new_galaxy, options):
                        if num_new_target_galaxies < num_extra_target_galaxies:
                            target_galaxies.append(new_galaxy)
                            num_new_target_galaxies += 1
                            bad_type = False
                    else:
                        if num_new_background_galaxies < num_extra_background_galaxies:
                            background_galaxies.append(new_galaxy)
                            num_new_background_galaxies += 1
                            bad_type = False

            logger.debug("Finished adjusting number of target galaxies upward.")

        elif num_ratio < 1:

            # Remove galaxies from the lists
            logger.debug("Adjusting number of target galaxies downward.")
            num_extra_target_galaxies = num_target_galaxies - options['num_target_galaxies']
            num_extra_background_galaxies = int((1 - num_ratio) * num_background_galaxies)

            for _ in range(num_extra_target_galaxies):
                del target_galaxies[-1]
            for _ in range(num_extra_background_galaxies):
                del background_galaxies[-1]
            logger.debug("Finished adjusting number of target galaxies downward.")

        num_target_galaxies = len(target_galaxies)

        # Set up the new galaxies list
        galaxies = target_galaxies + background_galaxies

    else:
        num_ratio = 1

    # If shape noise cancellation is being applied, we'll need to arrange galaxy groups and pairs
    # manually
    galaxy_group_IDs = {}
    if options['shape_noise_cancellation']:

        logger.debug("Implementing shape noise cancellation adjustments.")

        # First, shear must be set at the level of galaxy group or higher
        if image_phl.get_generation_level('shear_magnitude') > SHE_GST_PhysicalModel.galaxy_group_level:
            image_phl.set_generation_level('shear_magnitude', SHE_GST_PhysicalModel.galaxy_group_level)
        if image_phl.get_generation_level('shear_angle') > SHE_GST_PhysicalModel.galaxy_group_level:
            image_phl.set_generation_level('shear_angle', SHE_GST_PhysicalModel.galaxy_group_level)

        # Determine how many groups we need, creating just enough
        galaxies_per_group = options['galaxies_per_group']
        num_groups = (num_target_galaxies + galaxies_per_group - 1) // galaxies_per_group
        num_pairs_per_group = (galaxies_per_group + 1) // 2

        # Set up galaxy groups and pairs
        for i in range(num_groups):
            image_phl.add_galaxy_group()
        galaxy_groups = image_phl.get_galaxy_group_descendants()

        for galaxy_group in galaxy_groups:
            for i in range(num_pairs_per_group):
                galaxy_group.add_galaxy_pair()

        # Abduct galaxies into groups
        for i in range(num_target_galaxies):
            group_i = i // galaxies_per_group
            pair_i = (i % galaxies_per_group) // 2

            galaxy_groups[group_i].get_galaxy_pair_descendants()[pair_i].abduct_child(target_galaxies[i])

        # For each group, set the rotations as uniformly distributed
        logger.debug("Rotating galaxies in each group uniformly")
        for galaxy_group in galaxy_groups:

            # Note the group id
            group_ID = galaxy_group.get_full_ID()

            base_rotation = galaxy_group.get_param_value("rotation")

            # Go for loose galaxies first, in case we do that in the future
            galaxies_in_group = galaxy_group.get_galaxies()
            num_galaxies_in_group = len(galaxies_in_group)

            for i, galaxy in enumerate(galaxies_in_group):
                new_rotation = base_rotation + i * 180. / num_galaxies_in_group
                if new_rotation > 180:
                    new_rotation -= 180
                galaxy.set_param_params("rotation", "fixed", new_rotation)

            # Now handle pairs
            galaxy_pairs_in_group = galaxy_group.get_galaxy_pairs()
            num_galaxy_pairs_in_group = len(galaxy_pairs_in_group)

            for i, galaxy_pair in enumerate(galaxy_pairs_in_group):
                new_rotation = base_rotation + i * 90. / num_galaxy_pairs_in_group
                if new_rotation > 180:
                    new_rotation -= 180
                galaxy_pair.set_param_params("rotation", "fixed", new_rotation)
                for galaxy in galaxy_pair.get_galaxies():
                    galaxy.set_param_params("rotation", "fixed", new_rotation)
                    new_rotation += 90.
                    if new_rotation > 180:
                        new_rotation -= 180
                    # Note the group ID for this galaxy
                    galaxy_group_IDs[galaxy.get_full_ID()] = group_ID

        logger.debug("Finished implementing shape noise cancellation")
    else:
        for galaxy in galaxies:
            # Use the galaxy's own ID as the group ID
            galaxy_group_IDs[galaxy.get_full_ID()] = galaxy.get_full_ID()

    # Figure out how to set up the grid for galaxy stamps, making it as square as possible
    ncols = int(np.ceil(np.sqrt(num_target_galaxies)))
    if ncols == 0:
        ncols = 1
    nrows = int(np.ceil(num_target_galaxies / ncols))
    if nrows == 0:
        nrows = 1

    # Indices to keep track of row and column we're drawing galaxy/psf to
    icol = -1
    irow = 0

    # Only do galaxy stamp image in stamps mode and not details_only
    if (options['mode'] == 'stamps') and (not options['details_only']):

        stamp_size_pix = options['stamp_size']

        stamp_image_npix_x = ncols * stamp_size_pix
        stamp_image_npix_y = nrows * stamp_size_pix

        if options['render_background_galaxies']:
            size_ratio = (num_target_galaxies / num_ratio) * np.pi * (stamp_size_pix / 2) ** 2 / \
                (full_x_size * full_y_size)
            bg_stamp_area_pix = stamp_size_pix ** 2 / size_ratio
            bg_aperture_rad_pix = np.sqrt(bg_stamp_area_pix / np.pi)

            old_x_size, full_x_size = full_x_size, stamp_image_npix_x
            old_y_size, full_y_size = full_y_size, stamp_image_npix_y

            # Check this is valid
            if 2 * bg_aperture_rad_pix > stamp_size_pix:
                raise Exception("Stamp size is too small to properly render background galaxies. " +
                                "Increase the stamp size to at least " +
                                str(int(2 * bg_aperture_rad_pix + 1)) + " pixels.")

        # Replace the dithers we've generated with properly-sized ones
        for di in range(num_dithers):
            dithers[di] = galsim.Image(stamp_image_npix_x,
                                       stamp_image_npix_y,
                                       dtype=dithers[di].dtype,
                                       wcs=wcs_list[di])

    if options['render_background_galaxies']:
        logger.info("Printing " + str(num_target_galaxies) + " target galaxies and " +
                    str(num_background_galaxies) + " background galaxies.")
    else:
        logger.info("Printing " + str(num_target_galaxies) + " target galaxies.")

    num_target_galaxies_printed = 0
    num_background_galaxies_printed = 0

    # Loop over galaxies now

    for galaxy in galaxies:

        is_target_gal = is_target_galaxy(galaxy, options)

        # If it isn't a target and we aren't rendering background galaxies, skip it
        if (not is_target_gal) and (not options['render_background_galaxies']):
            continue

        if is_target_gal:
            if num_target_galaxies_printed % 100 == 0:
                logger.info("Printed " + str(num_target_galaxies_printed) + "/" +
                            str(num_target_galaxies) + " target galaxies.")
            num_target_galaxies_printed += 1
        else:
            if num_background_galaxies_printed % 500 == 0:
                logger.info("Printed " + str(num_background_galaxies_printed) + "/" +
                            str(num_background_galaxies) + " background galaxies.")
            num_background_galaxies_printed += 1

        # Get some galaxy info to avoid repeating method calls
        gal_intensity = get_I(galaxy.get_param_value('apparent_mag_vis'),
                              'mag_vis',
                              gain=options['gain'],
                              exp_time=galaxy.get_param_value('exp_time'))
        if options['single_psf']:
            gal_n = 1
            gal_z = 0
        else:
            gal_n = galaxy.get_param_value('sersic_index')
            gal_z = galaxy.get_param_value('redshift')

        if not options['details_only']:

            use_background_psf = (not is_target_gal) or ((not options['euclid_psf']) and
                                                         (options['model_psf_file_name'] is None))

            # Set up the profiles for the psf
            bulge_psf_profile = get_psf_profile(n=gal_n,
                                                z=gal_z,
                                                bulge=True,
                                                use_background_psf=use_background_psf,
                                                data_dir=options['data_dir'],
                                                model_psf_file_name=options['model_psf_file_name'],
                                                model_psf_scale=options['model_psf_scale'] *
                                                36000,  # Needs to be in units of pixels
                                                model_psf_offset=model_psf_offset,
                                                pixel_scale=pixel_scale,
                                                gsparams=default_gsparams,
                                                workdir=options['workdir'])
            if options['chromatic_psf']:
                disk_psf_profile = get_psf_profile(n=gal_n,
                                                   z=gal_z,
                                                   bulge=False,
                                                   use_background_psf=use_background_psf,
                                                   data_dir=options['data_dir'],
                                                   model_psf_file_name=options['model_psf_file_name'],
                                                   model_psf_scale=options['model_psf_scale'] *
                                                   36000,  # Needs to be in units of pixels
                                                   model_psf_offset=model_psf_offset,
                                                   pixel_scale=pixel_scale,
                                                   gsparams=default_gsparams,
                                                   workdir=options['workdir'])
            else:
                disk_psf_profile = bulge_psf_profile

            # Save the profiles to the archive file
            for di in range(num_dithers):
                if ((options['output_psf_file_name'] is None or options['output_psf_file_name'] == 'None') and
                    (options['model_psf_file_name'] is None or options['model_psf_file_name'] == 'None') and
                        not options['single_psf']):
                    output_bulge_psf_profile = bulge_psf_profile
                    output_disk_psf_profile = disk_psf_profile

                    add_psf_to_archive(psf_profile=output_bulge_psf_profile,
                                       archive_filehandle=psf_archive_filehandle,
                                       galaxy_id=galaxy.get_full_ID(),
                                       exposure_index=di,
                                       psf_type="bulge",
                                       stamp_size=options['psf_stamp_size'],
                                       scale=pixel_scale / options['psf_scale_factor'],)
                    add_psf_to_archive(psf_profile=output_disk_psf_profile,
                                       archive_filehandle=psf_archive_filehandle,
                                       galaxy_id=galaxy.get_full_ID(),
                                       exposure_index=di,
                                       psf_type="disk",
                                       stamp_size=options['psf_stamp_size'],
                                       scale=pixel_scale / options['psf_scale_factor'])

            # Get the position of the galaxy, depending on whether we're in field or stamp mode

            if is_target_gal:

                # Increment position
                icol += 1
                if icol >= ncols:
                    icol = 0
                    irow += 1
                    if irow >= nrows:
                        raise Exception("More galaxies than expected when printing stamps.")

                # Adjust galaxy position in stamp mode
                if (options['mode'] == 'stamps'):

                    xp_init = galaxy.get_param_value("xp")
                    yp_init = galaxy.get_param_value("yp")

                    xp_sp_shift = xp_init - int(xp_init)
                    yp_sp_shift = yp_init - int(yp_init)

                    xp = xp_sp_shift + stamp_size_pix // 2 + icol * stamp_size_pix
                    yp = yp_sp_shift + stamp_size_pix // 2 + irow * stamp_size_pix

                else:

                    xp = galaxy.get_param_value("xp")
                    yp = galaxy.get_param_value("yp")

            elif options['mode'] == 'stamps':

                # Background galaxy in stamp mode. We'll have to determine a valid position
                # on a non-empty stamp

                # Pick a random stamp to appear on
                si = np.random.randint(0, num_target_galaxies)
                yi = si // ncols
                xi = si - yi * ncols

                # Use the generated xp and yp values as random input here, so we don't affect
                # the seeding
                xp_init = galaxy.get_param_value("xp")
                yp_init = galaxy.get_param_value("yp")

                # Place bgs in a circular aperture around target galaxies

                # Uniform random angle
                theta_rad = 2 * np.pi * xp_init / old_x_size

                # Random distance, using sqrt to weight by area at a given distance
                rp = bg_aperture_rad_pix * np.sqrt(yp_init / old_y_size)

                xp = stamp_size_pix // 2 + xi * stamp_size_pix + rp * np.cos(theta_rad)
                yp = stamp_size_pix // 2 + yi * stamp_size_pix + rp * np.sin(theta_rad)

            else:

                xp = galaxy.get_param_value("xp")
                yp = galaxy.get_param_value("yp")

            xp_i = int(xp)
            yp_i = int(yp)

            xp_sp_shift = xp - xp_i
            yp_sp_shift = yp - yp_i

            subsampling_factor = int(1 / mv.psf_model_scale)

        else:  # if not options['details_only']:
            # Store dummy values for pixel positions
            xp = -1
            yp = -1
            xc = -1
            yc = -1
            xp_sp_shift = 0
            yp_sp_shift = 0

        # Store galaxy data to save calls to the class

        rotation = galaxy.get_param_value('rotation')
        spin = galaxy.get_param_value('spin')
        tilt = galaxy.get_param_value('tilt')

        g_shear = galaxy.get_param_value('shear_magnitude')
        beta_shear = galaxy.get_param_value('shear_angle')

        g_ell = galaxy.get_param_value('bulge_ellipticity')

        bulge_fraction = galaxy.get_param_value('bulge_fraction')
        bulge_size = galaxy.get_param_value('apparent_size_bulge')
        bulge_trunc_factor = galaxy.get_param_value('bulge_truncation_factor')

        disk_size = galaxy.get_param_value('apparent_size_disk')
        disk_height_ratio = galaxy.get_param_value('disk_height_ratio')
        disk_trunc_factor = galaxy.get_param_value('disk_truncation_factor')

        if not options['details_only']:
            if is_target_gal:

                bulge_gal_profile_world = get_bulge_galaxy_profile(sersic_index=gal_n,
                                                                   half_light_radius=bulge_size,
                                                                   flux=gal_intensity * bulge_fraction,
                                                                   g_ell=g_ell,
                                                                   beta_deg_ell=rotation,
                                                                   g_shear=g_shear,
                                                                   beta_deg_shear=beta_shear,
                                                                   trunc_factor=bulge_trunc_factor,
                                                                   gsparams=default_gsparams)

                # Convert the profile to image co-ordinates
                bulge_gal_profile = jacobian_wcs.toImage(bulge_gal_profile_world)

                # Convolve the galaxy, psf, and pixel profile to determine the final (well,
                # before noise) pixelized image_phl
                final_bulge = galsim.Convolve([bulge_gal_profile, bulge_psf_profile],
                                              gsparams=default_gsparams)

                # Try to get a disk galaxy profile if the galsim version supports it
                disk_gal_profile_world = get_disk_galaxy_profile(half_light_radius=disk_size,
                                                                 rotation=rotation,
                                                                 tilt=tilt,
                                                                 flux=gal_intensity * (1 - bulge_fraction),
                                                                 g_shear=g_shear,
                                                                 beta_deg_shear=beta_shear,
                                                                 height_ratio=disk_height_ratio,
                                                                 trunc_factor=disk_trunc_factor,
                                                                 gsparams=default_gsparams)

                # Convert the profile to image co-ordinates
                disk_gal_profile = jacobian_wcs.toImage(disk_gal_profile_world)

                final_disk = galsim.Convolve([disk_gal_profile, disk_psf_profile,
                                              galsim.Pixel(scale=pixel_scale)],
                                             gsparams=default_gsparams)

                # Now draw the PSFs for this galaxy onto those images

            else:
                # Just use a single sersic profile for background galaxies
                # to make them more of a compromise between bulges and disks
                gal_profile_world = get_bulge_galaxy_profile(sersic_index=gal_n,
                                                             half_light_radius=bulge_size,
                                                             flux=gal_intensity,
                                                             g_ell=2. * g_ell,
                                                             beta_deg_ell=rotation,
                                                             g_shear=g_shear,
                                                             beta_deg_shear=beta_shear,
                                                             gsparams=default_gsparams)

                gal_profile = jacobian_wcs.toImage(gal_profile_world)

                # Convolve the galaxy, psf, and pixel profile to determine the final
                # (well, before noise) pixelized image_phl
                final_gal = galsim.Convolve([gal_profile, disk_psf_profile],
                                            gsparams=default_gsparams)

            if not options['mode'] == 'stamps':
                if is_target_gal:
                    stamp_size_pix = 2 * (
                        np.max((int(options['stamp_size_factor'] * bulge_size / (3600 * pixel_scale)),
                                int(options['stamp_size_factor'] * disk_size / (3600 * pixel_scale))))) + \
                        int(np.max(np.shape(disk_psf_profile.image.array)) / subsampling_factor)
                else:
                    stamp_size_pix = 4 * (
                        np.max((int(options['stamp_size_factor'] * bulge_size / (3600 * pixel_scale)),
                                int(options['stamp_size_factor'] * disk_size / (3600 * pixel_scale)))))

                if stamp_size_pix > full_x_size:
                    stamp_size_pix = full_x_size
                if stamp_size_pix > full_y_size:
                    stamp_size_pix = full_y_size

            # Determine boundaries
            xl = xp_i - stamp_size_pix // 2 + 1
            xh = xl + stamp_size_pix - 1
            yl = yp_i - stamp_size_pix // 2 + 1
            yh = yl + stamp_size_pix - 1

            x_shift = 0
            y_shift = 0

            if not (is_target_gal and (options['mode'] == 'stamps')):
                # Check if the stamp crosses an edge and adjust as necessary
                if xl < 1:
                    x_shift = 1 - xl
                elif xh > full_x_size:
                    x_shift = full_x_size - xh
                xh += x_shift
                xl += x_shift

                if yl < 1:
                    y_shift = 1 - yl
                elif yh > full_y_size:
                    y_shift = full_y_size - yh
                yh += y_shift
                yl += y_shift

            bounds = galsim.BoundsI(xl, xh, yl, yh)

            gal_images = []
            for di in range(num_dithers):
                gal_images.append(dithers[di][bounds])

            # Get centers, correcting by 1.5 - 1 since Galsim is offset by 1, .5 to move from
            # corner of pixel to center
            x_centre_offset = x_shift
            y_centre_offset = y_shift
            xc = bounds.center.x + centre_offset + x_centre_offset
            yc = bounds.center.y + centre_offset + y_centre_offset

            # Draw the image
            for gal_image, (x_offset, y_offset) in zip(gal_images, get_dither_scheme(options['dithering_scheme'])):

                if is_target_gal:
                    final_bulge.drawImage(gal_image, scale=1.0,
                                          offset=(-x_centre_offset + x_offset + xp_sp_shift,
                                                  - y_centre_offset + y_offset + yp_sp_shift),
                                          add_to_image=True)

                    disk_xp_sp_shift = xp_sp_shift
                    disk_yp_sp_shift = yp_sp_shift

                    final_disk.drawImage(gal_image, scale=1.0,
                                         offset=(-x_centre_offset + x_offset + disk_xp_sp_shift,
                                                 - y_centre_offset + y_offset + disk_yp_sp_shift),
                                         add_to_image=True,
                                         method='no_pixel')

                else:
                    final_gal.drawImage(gal_image, scale=1.0,
                                        offset=(-x_centre_offset + x_offset + xp_sp_shift,
                                                - y_centre_offset + y_offset + xp_sp_shift),
                                        add_to_image=True)

        xy_world = wcs_list[0].toWorld(galsim.PositionD(xc + xp_sp_shift, yc + yp_sp_shift))

        # Record all data used for this galaxy in the output table (except snr, which we calculate later)
        if (not options['details_output_format'] == 'none') and (is_target_gal or (options['mode'] == 'field')):
            g1 = g_shear * np.cos(2 * beta_shear * np.pi / 180)
            g2 = g_shear * np.sin(2 * beta_shear * np.pi / 180)

            details_table.add_row(vals={datf.ID: galaxy.get_full_ID(),
                                        datf.group_ID: galaxy_group_IDs[galaxy.get_full_ID()],
                                        datf.ra: xy_world.x,
                                        datf.dec: xy_world.y,
                                        datf.hlr_bulge: bulge_size,
                                        datf.hlr_disk: disk_size,
                                        datf.bulge_ellipticity: g_ell,
                                        datf.bulge_axis_ratio: galaxy.get_param_value('bulge_axis_ratio'),
                                        datf.bulge_fraction: bulge_fraction,
                                        datf.disk_height_ratio: disk_height_ratio,
                                        datf.z: gal_z,
                                        datf.magnitude: galaxy.get_param_value('apparent_mag_vis'),
                                        datf.snr: 0,
                                        datf.sersic_index: gal_n,
                                        datf.rotation: rotation,
                                        datf.tilt: tilt,
                                        datf.spin: spin,
                                        datf.g1: g1,
                                        datf.g2: g2,
                                        datf.target_galaxy: is_target_gal})

        if is_target_gal and not options['details_only']:

            hlr = bulge_size * bulge_fraction + disk_size * (1 - bulge_fraction)

            # Add to detections table only if it's a target galaxy
            detections_table.add_row(vals={
                detf.ID: galaxy.get_full_ID(),
                detf.seg_ID: -99,
                detf.vis_det: True,
                detf.gal_x_world: xy_world.x,
                detf.gal_y_world: xy_world.y,
                detf.STAR_FLAG: False,
                detf.STAR_PROB: 0.,
                detf.hlr: hlr,
                detf.FLUX_VIS_APER: 10**(-0.4 * galaxy.get_param_value('apparent_mag_vis')),
            })

            del final_disk, disk_psf_profile

    logger.info("Finished printing galaxies.")

    return galaxies


def add_image_header_info(gs_image,
                          gain,
                          full_options,
                          model_seed,
                          extname,
                          ccdid,
                          wcs,
                          stamp_size=None,):
    """
        @brief Adds various information to the image's header.

        @param gs_image
            <galsim.Image> Galsim image object.
        @param gain
            <float> Gain of the image
        @param stamp_size
            <int> Size of postage stamps in the image, if applicable
        @param full_options
            <dict> Full options dictionary
        @param model_seed
            <int> Seed used for the physical model
        @param extname
            <str> Name of the extension
        @param ccdid
            <str> ID of the CCD
        @param dither_shift
            <(float,float)> Shift for this dither.

    """

    logger = getLogger(__name__)
    logger.debug("Entering add_image_header_info method.")

    # Add a header attribute if needed
    if not hasattr(gs_image, "header"):
        gs_image.header = galsim.FitsHeader()

    # SHE_GST_PhysicalModel package version label
    gs_image.header[mv.version_label] = mv.version_str

    # Galsim version label
    if hasattr(galsim, "__version__"):
        gs_image.header[mv.galsim_version_label] = galsim.__version__
    else:
        gs_image.header[mv.galsim_version_label] = '<1.2'

    # Gain
    gs_image.header[gain_label] = (gain, 'e-/ADU')

    # Stamp size
    if stamp_size is not None:
        gs_image.header[stamp_size_label] = stamp_size
    elif stamp_size_label in gs_image.header:
        del gs_image.header[stamp_size_label]

    # Model hash
    gs_image.header[model_hash_label] = hash_any(full_options, format="base64")

    # Seeds
    gs_image.header[model_seed_label] = model_seed
    gs_image.header[noise_seed_label] = full_options["noise_seed"]

    # Extension name
    gs_image.header[extname_label] = extname
    gs_image.header[ccdid_label] = ccdid

    # Pixel scale
    scale, _, _, _ = gs_image.wcs.jacobian().getDecomposition()
    gs_image.header[scale_label] = scale

    # WCS info
    wcs.writeToFitsHeader(gs_image.header, gs_image.bounds)

    logger.debug("Exiting add_image_header_info method.")

    return


def generate_image(image_phl,
                   options,
                   wcs_list,
                   psf_archive_filehandle):
    """
        @brief Creates a single image_phl of galaxies

        @details If successful, generates an image_phl and corresponding details according to
            the configuration stored in the image_phl and options objects.

        @param image_phl
            <SHE_GST_PhysicalModel.Image> The Image-level object which specifies how galaxies are to be generated.
        @param options
            <dict> The options dictionary for this run.
    """

    logger = getLogger(__name__)
    logger.debug("Entering generate_image method.")

    logger.debug("# Printing image " + str(image_phl.get_local_ID()) + " #")

    # Magic numbers

    centre_offset = -0.5

    # Setup

    image_phl.autofill_children()

    # General setup from config
    num_dithers = len(get_dither_scheme(options['dithering_scheme']))

    # Setup for the file
    dithers = []
    noise_maps = []
    mask_maps = []
    wgt_maps = []
    bkg_maps = []
    segmentation_maps = []

    # Create the image_phl object, using the appropriate method for the image_phl type
    full_x_size = int(image_phl.get_param_value("image_size_xp"))
    full_y_size = int(image_phl.get_param_value("image_size_yp"))
    pixel_scale = image_phl.get_param_value("pixel_scale")
    if not options['details_only']:
        for di in range(num_dithers):
            if options['image_datatype'] == '32f':
                dithers.append(galsim.ImageF(full_x_size, full_y_size, wcs=wcs_list[di]))
            elif options['image_datatype'] == '64f':
                dithers.append(galsim.ImageD(full_x_size, full_y_size, wcs=wcs_list[di]))
            else:
                raise Exception("Bad image_phl type slipped through somehow.")
    if options['mode'] == 'field':
        stamp_size_pix = None
    else:
        stamp_size_pix = options['stamp_size']

    # Set up a table for output if necessary
    if options['details_output_format'] == 'none':
        detections_table = None
        details_table = None
    else:
        full_options = get_full_options(options, image_phl)
        detections_table = initialise_detections_table(image_phl.get_parent(), full_options,
                                                       optional_columns=[detf.seg_ID,
                                                                         detf.STAR_FLAG,
                                                                         detf.STAR_PROB,
                                                                         detf.hlr,
                                                                         detf.FLUX_VIS_APER])
        details_table = initialise_details_table(image_phl.get_parent(), full_options)

    # Print the galaxies
    galaxies = print_galaxies(image_phl, options, wcs_list, centre_offset, num_dithers, dithers,
                              full_x_size, full_y_size, pixel_scale,
                              detections_table, details_table, psf_archive_filehandle)

    sky_level_subtracted = image_phl.get_param_value('subtracted_background')
    sky_level_subtracted_pixel = sky_level_subtracted * pixel_scale ** 2 * 3600**2
    sky_level_unsubtracted_pixel = image_phl.get_param_value('unsubtracted_background') * pixel_scale ** 2 * 3600**2
    if options['output_unsubtracted_background'] is None:
        output_sky_level_unsubtracted_pixel = sky_level_unsubtracted_pixel
    else:
        output_sky_level_unsubtracted_pixel = options['output_unsubtracted_background'] * pixel_scale ** 2 * 3600**2

    # Get the initial noise deviates
    base_deviates = []
    if not options['suppress_noise']:
        for di in range(num_dithers):
            if options['noise_seed'] != 0:
                base_deviate = galsim.BaseDeviate(num_dithers * options['noise_seed'] + di)
            else:
                base_deviate = galsim.BaseDeviate(image_phl.get_full_seed() + 1 + di)
            base_deviates.append(base_deviate)

    # For each dither
    if not options['details_only']:
        for di in range(num_dithers):

            logger.debug("Printing dither " + str(di + 1) + ".")

            # Make mock noise, mask, and background maps for this dither
            if options['image_datatype'] == '32f':
                noise_maps.append(galsim.ImageF(np.ones_like(dithers[di].array), wcs=wcs_list[di]))
                wgt_maps.append(galsim.ImageF(np.ones_like(dithers[di].array), wcs=wcs_list[di]))
                bkg_maps.append(galsim.ImageF(np.ones_like(
                    dithers[di].array), wcs=wcs_list[di]) * output_sky_level_unsubtracted_pixel)
            elif options['image_datatype'] == '64f':
                noise_maps.append(galsim.ImageD(np.ones_like(dithers[di].array), wcs=wcs_list[di]))
                wgt_maps.append(galsim.ImageD(np.ones_like(dithers[di].array), wcs=wcs_list[di]))
                bkg_maps.append(galsim.ImageD(np.ones_like(
                    dithers[di].array), wcs=wcs_list[di]) * output_sky_level_unsubtracted_pixel)

            wgt_maps[di].array[noise_maps[di].array > 0] /= noise_maps[di].array[noise_maps[di].array > 0] ** 2
            wgt_maps[di].array[noise_maps[di].array <= 0] *= 0

            mask_maps.append(galsim.ImageI(np.zeros_like(dithers[di].array, dtype=np.int16), wcs=wcs_list[di]))

            logger.info("Generating segmentation map " + str(di) + ".")
            noise_level = np.sqrt(get_var_ADU_per_pixel(pixel_value_ADU=sky_level_unsubtracted_pixel,
                                                        sky_level_ADU_per_sq_arcsec=sky_level_subtracted,
                                                        read_noise_count=options['read_noise'],
                                                        pixel_scale=pixel_scale * 3600,
                                                        gain=options['gain']))
            segmentation_maps.append(make_segmentation_map(dithers[di],
                                                           detections_table,
                                                           wcs_list[di],
                                                           threshold=0.01 * noise_level,
                                                           options=options))

            # If we're using cutouts, make the cutout image_phl now
            if options['mode'] == 'cutouts':
                dithers[di] = make_cutout_image(dithers[di],
                                                options,
                                                galaxies,
                                                detections_table,
                                                details_table,
                                                centre_offset)
                noise_maps[di] = make_cutout_image(noise_maps[di],
                                                   options,
                                                   galaxies,
                                                   detections_table,
                                                   details_table,
                                                   centre_offset)
                mask_maps[di] = make_cutout_image(mask_maps[di],
                                                  options,
                                                  galaxies,
                                                  detections_table,
                                                  details_table,
                                                  centre_offset)
                bkg_maps[di] = make_cutout_image(bkg_maps[di],
                                                 options,
                                                 galaxies,
                                                 detections_table,
                                                 details_table,
                                                 centre_offset)
                wgt_maps[di] = make_cutout_image(wgt_maps[di],
                                                 options,
                                                 galaxies,
                                                 detections_table,
                                                 details_table,
                                                 centre_offset)
                segmentation_maps[di] = make_cutout_image(segmentation_maps[di],
                                                          options,
                                                          galaxies,
                                                          detections_table,
                                                          details_table,
                                                          centre_offset)

            dither = dithers[di]
            dither += sky_level_unsubtracted_pixel

            detector_id_str = detector.get_id_string(image_phl.get_local_ID() % 6 + 1,
                                                     image_phl.get_local_ID() // 6 + 1)

            # Add a header containing version info
            add_image_header_info(dither, options['gain'], full_options, image_phl.get_full_seed(),
                                  extname=detector_id_str + "." + sci_tag, ccdid=detector_id_str,
                                  wcs=wcs_list[di], stamp_size=stamp_size_pix)
            add_image_header_info(noise_maps[di], options['gain'], full_options, image_phl.get_full_seed(),
                                  extname=detector_id_str + "." + noisemap_tag, ccdid=detector_id_str,
                                  wcs=wcs_list[di], stamp_size=stamp_size_pix)
            add_image_header_info(mask_maps[di], options['gain'], full_options, image_phl.get_full_seed(),
                                  extname=detector_id_str + "." + mask_tag, ccdid=detector_id_str,
                                  wcs=wcs_list[di], stamp_size=stamp_size_pix)
            add_image_header_info(bkg_maps[di], options['gain'], full_options, image_phl.get_full_seed(),
                                  extname=detector_id_str, ccdid=detector_id_str,
                                  wcs=wcs_list[di], stamp_size=stamp_size_pix)
            add_image_header_info(wgt_maps[di], options['gain'], full_options, image_phl.get_full_seed(),
                                  extname=detector_id_str, ccdid=detector_id_str,
                                  wcs=wcs_list[di], stamp_size=stamp_size_pix)
            add_image_header_info(segmentation_maps[di], options['gain'], full_options, image_phl.get_full_seed(),
                                  extname=detector_id_str + "." + segmentation_tag, ccdid=detector_id_str,
                                  wcs=wcs_list[di], stamp_size=stamp_size_pix)

            # Note - noise map here deliberately doesn't include galaxy contributions

            noise_maps[di] *= noise_level

            if not options['suppress_noise']:

                if options['stable_rng']:
                    # Use noise array here that does contain galaxy info
                    var_array = get_var_ADU_per_pixel(pixel_value_ADU=dither.array,
                                                      sky_level_ADU_per_sq_arcsec=sky_level_subtracted,
                                                      read_noise_count=options['read_noise'],
                                                      pixel_scale=pixel_scale * 3600,
                                                      gain=options['gain'])

                    add_stable_noise(image=dither,
                                     base_deviate=base_deviates[di],
                                     var_array=var_array,
                                     image_phl=image_phl,
                                     options=options)
                else:
                    dither.addNoise(galsim.CCDNoise(base_deviates[di],
                                                    gain=options['gain'],
                                                    read_noise=options['read_noise'],
                                                    sky_level=sky_level_subtracted_pixel))
                dithers[di] = dither
            else:
                dithers[di] = dithers[di]

        logger.info("Finished printing dither " + str(di + 1) + ".")

        # Now that the galaxies have been printed, we can calculate their S/Ns
        for row in details_table:
            signal_to_noise_estimates = []
            for di in range(num_dithers):
                signal_to_noise_estimates.append(get_signal_to_noise_estimate(ra=row[datf.ra],
                                                                              dec=row[datf.dec],
                                                                              image=dithers[di],
                                                                              background=bkg_maps[di],
                                                                              rms=np.mean(noise_maps[di].array),
                                                                              gain=options['gain'],
                                                                              stamp_size=options['stamp_size']))
            # Add the S/N estimates in quadrature
            snr_squared = 0
            for signal_to_noise_estimate in signal_to_noise_estimates:
                snr_squared += signal_to_noise_estimate**2
            snr = np.sqrt(snr_squared)
            row[datf.snr] = snr

    logger.info("Finished printing image " + str(image_phl.get_local_ID()) + ".")

    # We no longer need this image_phl's children, so clear it to save memory
    image_phl.clear()

    logger.debug("Exiting generate_image method.")
    return (dithers, noise_maps, mask_maps, wgt_maps, bkg_maps, segmentation_maps,
            detections_table, details_table)
