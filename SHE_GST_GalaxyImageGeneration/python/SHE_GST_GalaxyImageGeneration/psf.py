""" @file psf.py

    Created 11 Dec 2015

    @TODO: File docstring
"""

__updated__ = "2021-08-17"

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
from functools import lru_cache
from os.path import join

from SHE_PPT.constants.fits import BULGE_PSF_TAG, DISK_PSF_TAG
from SHE_PPT.file_io import find_file
from SHE_PPT.logging import getLogger
from SHE_PPT.table_formats.she_psf_model_image import tf as pstf
from astropy.io import fits
from astropy.io.fits import table_to_hdu
import galsim

import SHE_GST_GalaxyImageGeneration.magic_values as mv
import numpy as np


logger = getLogger(__name__)


# Magic values for this module
sed_names = {'ell': 'el_cb2004a_001',
             'sbc': 'sbc_cb2004a_001',
             'scd': 'scd_cb2004a_001',
             'sb2': 'sb2_b2004a_001',
             'sb3': 'sb3_b2004a_001',
             }

seds = {4.0: 'sbc',
        3.5: 'sbc',
        3.0: 'sbc',
        2.71: 'scd',
        2.56: 'sb2',
        2.0: 'sb3',
        1.8: 'sb3',
        }

single_psf_filename = join(mv.default_data_dir, "psf_models", sed_names['sb3'] + ".fits_0.000_0.804_0.00.fits")

allowed_ns = np.array((1.8, 2.0, 2.56, 2.71, 3.0, 3.5, 4.0))
allowed_zs = np.array((0., 0.5, 1.0, 1.5, 2.0))

gal_id_label = "GAL_ID"
exposure_index_label = "EX_INDEX"
SCALE_LABEL = "GS_SCALE"
type_label = "PSF_TYPE"


@lru_cache()
def load_psf_model_from_sed_z(sed,
                              z=0.0,
                              data_dir=mv.default_data_dir,
                              gsparams=galsim.GSParams(),
                              workdir="."):

    z_str = "%0.2f" % z

    model_filename = join(data_dir, "psf_models", sed_names[sed] + ".fits_0.000_0.804_" + z_str + ".fits")

    qualified_filename = find_file(model_filename, workdir)

    return load_psf_model_from_file(qualified_filename,
                                    scale=mv.psf_model_scale,
                                    offset=mv.default_psf_center_offset,
                                    gsparams=gsparams)


@lru_cache()
def load_psf_model_from_file(file_name,
                             scale,
                             offset,
                             gsparams=galsim.GSParams()):

    try:
        model = galsim.fits.read(file_name)
    except OSError as e:
        if not "HDU is empty" in str(e):
            raise
        # It might be in HDU 1 instead of 0
        model = galsim.fits.read(file_name, hdu=1)

    return galsim.InterpolatedImage(model,
                                    scale=scale,
                                    offset=offset,
                                    gsparams=gsparams)


@lru_cache()
def get_background_psf_profile(gsparams=galsim.GSParams(),
                               pixel_scale=mv.default_pixel_scale):

    prof = galsim.OpticalPSF(lam=725,  # nm
                             diam=1.2,  # m
                             scale_unit=galsim.degrees,
                             defocus=0,
                             obscuration=0.33,
                             nstruts=3,
                             gsparams=gsparams,
                             ).dilate(1. / pixel_scale)  # Scale to be in units of pixels

    return prof


def get_psf_profile(n,
                    z,
                    bulge,
                    use_background_psf=False,
                    data_dir=mv.default_data_dir,
                    model_psf_file_name=None,
                    model_psf_scale=mv.psf_model_scale,
                    model_psf_offset=mv.default_psf_center_offset,
                    pixel_scale=mv.default_pixel_scale,
                    gsparams=galsim.GSParams(),
                    workdir="."):

    if use_background_psf:
        return get_background_psf_profile(pixel_scale=pixel_scale, gsparams=gsparams)

    if model_psf_file_name is not None:
        qualified_model_filename = find_file(model_psf_file_name, workdir)
        return load_psf_model_from_file(qualified_model_filename, model_psf_scale, model_psf_offset, gsparams=gsparams)

    diffs = np.abs(allowed_zs - z)
    zi_best = np.argmin(diffs)

    if(bulge):
        sed = 'ell'
    else:
        diffs = np.abs(allowed_ns - n)
        ni_best = np.argmin(diffs)
        sed = seds[allowed_ns[ni_best]]

    return load_psf_model_from_sed_z(sed, allowed_zs[zi_best], gsparams=gsparams, data_dir=data_dir)


@lru_cache()
def create_psf_hdu(psf_profile,
                   stamp_size=mv.default_psf_stamp_size,
                   scale=mv.default_pixel_scale / mv.default_psf_scale_factor,):
    """Creates an HDU of an image of a PSF profile.
    """

    # Draw the profile onto an image of the proper size
    psf_image = galsim.ImageF(stamp_size, stamp_size, scale=scale)
    psf_profile.drawImage(psf_image, method='no_pixel')

    # Set up an image HDU with this image
    psf_hdu = fits.ImageHDU(data=psf_image.array)

    # Return
    return psf_hdu


def add_psf_to_archive(psf_profile,
                       archive_filehandle,
                       galaxy_id,
                       exposure_index,
                       psf_type,
                       stamp_size=mv.default_psf_stamp_size,
                       scale=mv.default_pixel_scale / mv.default_psf_scale_factor):
    """Creates a PSF HDU and saves it to an archive file.
    """

    # Create the HDU
    psf_hdu = create_psf_hdu(psf_profile=psf_profile,
                             stamp_size=stamp_size,
                             scale=scale)

    psf_dataset = archive_filehandle.create_dataset(str(galaxy_id) + "_" + str(exposure_index) + "_" + psf_type,
                                                    data=psf_hdu.data)

    # Add needed keywords to the attributes of this dataset
    psf_dataset.attrs[gal_id_label] = galaxy_id
    psf_dataset.attrs[exposure_index_label] = exposure_index
    psf_dataset.attrs[SCALE_LABEL] = scale
    psf_dataset.attrs[type_label] = psf_type

    return


def get_psf_from_archive(archive_filehandle,
                         galaxy_id,
                         exposure_index,
                         psf_type):

    dataset = archive_filehandle[str(galaxy_id) + "_" + str(exposure_index) + "_" + psf_type]

    psf_image = galsim.ImageF(dataset[:, :], scale=dataset.attrs[SCALE_LABEL])

    return psf_image


def sort_psfs_from_archive(psf_table,
                           psf_data_filename,
                           exposure_index,
                           archive_filehandle=None,
                           output_psf_filename=None,
                           stamp_size=mv.default_psf_stamp_size,
                           scale=mv.psf_model_scale,
                           workdir="."):

    logger.debug("Entering sort_psfs_from_archive")

    qualified_psf_data_filename = join(workdir, psf_data_filename)

    psf_table_hdu = table_to_hdu(psf_table)
    data_hdulist = fits.open(qualified_psf_data_filename, mode='append')
    data_hdulist.append(psf_table_hdu)

    psf_table.remove_indices(pstf.ID)  # Necessary for bug workaround in astropy
    psf_table.add_index(pstf.ID)  # Allow it to be indexed by galaxy ID

    hdu_index = 2  # Start indexing at 2, since 0 is empty and 1 is table

    # If we're using a single output PSF for all galaxies, use optimisations
    if output_psf_filename != None and output_psf_filename != 'None':
        qualified_output_psf_filename = find_file(output_psf_filename, workdir)
        psf_profile = load_psf_model_from_file(qualified_output_psf_filename,
                                               scale=scale,
                                               offset=mv.default_psf_center_offset)
        bulge_psf_hdu = create_psf_hdu(psf_profile=psf_profile,
                                       stamp_size=stamp_size,
                                       scale=scale)

        # Make the headers for each HDU correct
        bulge_psf_hdu.header[exposure_index_label] = exposure_index
        bulge_psf_hdu.header[SCALE_LABEL] = scale

        disk_psf_hdu = deepcopy(bulge_psf_hdu)

        bulge_psf_hdu.header[type_label] = "bulge"
        disk_psf_hdu.header[type_label] = "disk"

        bulge_psf_hdu.header['EXTNAME'] = "ALL." + BULGE_PSF_TAG
        disk_psf_hdu.header['EXTNAME'] = "ALL." + DISK_PSF_TAG

        data_hdulist.append(bulge_psf_hdu)
        data_hdulist.append(disk_psf_hdu)

        for row in psf_table:

            gal_id = row[pstf.ID]

            row[pstf.bulge_index] = hdu_index
            row[pstf.disk_index] = hdu_index + 1

    # Otherwise, sort from the archive
    else:

        for dataset_key in archive_filehandle:

            dataset = archive_filehandle[dataset_key]

            if dataset.attrs[exposure_index_label] != exposure_index:
                continue

            logger.debug("Sorting PSF " + dataset_key + " into file.")

            gal_id = dataset.attrs[gal_id_label]

            # Set up the hdu
            psf_hdu = fits.ImageHDU(data=dataset[:, :])
            psf_hdu.header[gal_id_label] = dataset.attrs[gal_id_label]
            psf_hdu.header[exposure_index_label] = dataset.attrs[exposure_index_label]
            psf_hdu.header[SCALE_LABEL] = dataset.attrs[SCALE_LABEL]
            psf_hdu.header[type_label] = dataset.attrs[type_label]
            if dataset.attrs[type_label] == "bulge":
                psf_hdu.header['EXTNAME'] = str(gal_id) + "." + BULGE_PSF_TAG
            else:
                psf_hdu.header['EXTNAME'] = str(gal_id) + "." + DISK_PSF_TAG

            # Add to the data file
            data_hdulist.append(psf_hdu)

            # Update the PSF table with the HDU index of this
            if dataset.attrs[type_label] == "bulge":
                psf_table.loc[gal_id][pstf.bulge_index] = hdu_index
            else:
                psf_table.loc[gal_id][pstf.disk_index] = hdu_index
            hdu_index += 1

    # Now that the table is complete, update the values for it in the fits file
    out_table = data_hdulist[1].data

    out_table[pstf.bulge_index] = psf_table[pstf.bulge_index]
    out_table[pstf.disk_index] = psf_table[pstf.disk_index]

    data_hdulist.flush()
    data_hdulist.close()

    logger.debug("Exiting sort_psfs_from_archive")

    return
