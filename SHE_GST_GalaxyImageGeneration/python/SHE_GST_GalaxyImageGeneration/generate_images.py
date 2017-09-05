"""
    @file generate_images.py

    Created 23 Jul 2015

    This module contains the functions which do the heavy lifting of actually
    generating images.

    ---------------------------------------------------------------------

    Copyright (C) 2012-2020 Euclid Science Ground Segment      
       
    This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General    
    Public License as published by the Free Software Foundation; either version 3.0 of the License, or (at your option)    
    any later version.    
       
    This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied    
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more    
    details.    
       
    You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to    
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
"""

from __future__ import division

from multiprocessing import cpu_count, Pool
from os.path import join
from copy import deepcopy

from astropy.table import Table
import galsim

from SHE_GST_GalaxyImageGeneration import magic_values as mv
from SHE_GST_GalaxyImageGeneration.combine_dithers import combine_dithers
from SHE_GST_GalaxyImageGeneration.compress_image import compress_image
from SHE_GST_GalaxyImageGeneration.config.check_config import get_full_options
from SHE_GST_GalaxyImageGeneration.cutouts import make_cutout_image
from SHE_GST_GalaxyImageGeneration.dither_schemes import get_dither_scheme
from SHE_GST_GalaxyImageGeneration.galaxy import (get_bulge_galaxy_profile,
                                             get_disk_galaxy_image,
                                             is_target_galaxy, get_disk_galaxy_profile)
from SHE_GST_GalaxyImageGeneration.magnitude_conversions import get_I
from SHE_GST_GalaxyImageGeneration.noise import get_var_ADU_per_pixel
from SHE_GST_GalaxyImageGeneration.psf import get_psf_profile

from SHE_GST_IceBRGpy.logging import getLogger
from SHE_GST_IceBRGpy.rebin import rebin

from SHE_PPT.details_table_format import initialise_details_table, details_table_format as datf
from SHE_PPT.detections_table_format import initialise_detections_table
from SHE_PPT.file_io import get_allowed_filename
from SHE_PPT.table_utility import add_row, output_tables
from SHE_PPT.utility import hash_any
from SHE_PPT.magic_values import (gain_label,scale_label,stamp_size_label,model_hash_label,
                                  model_seed_label,noise_seed_label)

import numpy as np
    
default_gsparams = galsim.GSParams(folding_threshold=5e-3,
                                   maxk_threshold=1e-3,
                                   kvalue_accuracy=1e-5,
                                   stepk_minimum_hlr=5,
                                   )

try:
    import pyfftw
    from SHE_GST_IceBRGpy.convolve import fftw_convolve as convolve
except ImportError as _e:
    from scipy.signal import fftconvolve as convolve

class generate_image_with_options_caller(object):
    def __init__(self, *args, **kwargs):
        self.args = args
        self.kwargs = kwargs
    def __call__(self, x):
        return generate_image(x, *self.args, **self.kwargs)

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
    logger = getLogger(mv.logger_name)
    logger.debug("Entering generate_images method.")

    # Seed the survey
    if options['seed'] == 0:
        survey.set_seed() # Seed from the time
    else:
        survey.set_seed(options['seed'])

    # Create empty image objects for the survey
    survey.fill_images()
    images = survey.get_images()

    # Multiprocessing doesn't currently work, so print a warning if it's requested

    if options['num_parallel_threads'] != 1:
        logger.warning("Multi-processing is not currently functional; it requires features that " +
                       "will be available in Python 3. Until then, if you wish to use multiple " +
                       "processes, please call this program multiple times with different seed " +
                       "values. Continuing with a single process...")
        options['num_parallel_threads'] = 1

    # If we just have one thread, we'll just use a simply function call to ease debugging
    if options['num_parallel_threads'] == 1:
        for image in images:
            generate_image(image, options)
    else:
        if options['num_parallel_threads'] <= 0:
            options['num_parallel_threads'] += cpu_count()

        pool = Pool(processes=cpu_count(), maxtasksperchild=1)
        pool.map(generate_image_with_options_caller(options), images, chunksize=1)
        
    logger.debug("Exiting generate_images method.")

    return

def print_galaxies_and_psfs(image,
                            options,
                            centre_offset,
                            num_dithers,
                            dithers,
                            p_bulge_psf_image,
                            p_disk_psf_image,
                            full_x_size,
                            full_y_size,
                            pixel_scale,
                            detections_table,
                            details_table):
    """
        @brief Prints galaxies onto a new image and stores details on them in the output table.

        @param image
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

    logger = getLogger(mv.logger_name)
    logger.debug("Entering 'print_galaxies_and_psfs' function.")
    
    # Get some data out of the options
    model_psf_offset = (options["model_psf_x_offset"],options["model_psf_y_offset"])
    single_output_psf = options['single_psf'] or (options['model_psf_file_name']
                                                  is not None)

    # Get the galaxies we'll be drawing
    galaxies = image.get_galaxy_descendants()

    background_galaxies = []
    target_galaxies = []

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

            field = image.get_field_descendants()[0]

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
    if options['shape_noise_cancellation']:
        
        logger.debug("Implementing shape noise cancellation adjustments.")
    
        # Determine how many groups we need, creating just enough
        galaxies_per_group = options['galaxies_per_group']
        num_groups = (num_target_galaxies + galaxies_per_group - 1) // galaxies_per_group
        num_pairs_per_group = (galaxies_per_group+1)//2
        
        # Set up galaxy groups and pairs    
        for i in range(num_groups):
            image.add_galaxy_group()
        galaxy_groups = image.get_galaxy_group_descendants()
        
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

            base_rotation = galaxy_group.get_param_value("rotation")

            # Go for loose galaxies first, in case we do that in the future
            galaxies_in_group = galaxy_group.get_galaxies()
            num_galaxies_in_group = len(galaxies_in_group)

            for i, galaxy in enumerate(galaxies_in_group):
                new_rotation = base_rotation + i * 180. / num_galaxies_in_group
                if new_rotation > 180: new_rotation -= 180
                galaxy.set_param_params("rotation", "fixed", new_rotation)

            # Now handle pairs
            galaxy_pairs_in_group = galaxy_group.get_galaxy_pairs()
            num_galaxy_pairs_in_group = len(galaxy_pairs_in_group)

            for i, galaxy_pair in enumerate(galaxy_pairs_in_group):
                new_rotation = base_rotation + i * 90. / num_galaxy_pairs_in_group
                if new_rotation > 180: new_rotation -= 180
                galaxy_pair.set_param_params("rotation", "fixed", new_rotation)
                for galaxy in galaxy_pair.get_galaxies():
                    galaxy.set_param_params("rotation", "fixed", new_rotation)
                    new_rotation += 90.
                    if new_rotation > 180: new_rotation -= 180
        
        logger.debug("Finished implementing shape noise cancellation")

    # Figure out how to set up the grid for galaxy/psf stamps, making it as square as possible
    ncols = int(np.ceil(np.sqrt(num_target_galaxies)))
    nrows = int(np.ceil(num_target_galaxies / ncols))
    
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
                                       scale=dithers[di].scale)
            
    # Set up bulge and disk psf images
    if not options['details_only']:
        psf_stamp_size_pix = options['psf_stamp_size']
    
        if single_output_psf:
            psf_stamp_image_npix_x = psf_stamp_size_pix
            psf_stamp_image_npix_y = psf_stamp_size_pix
        else:
            psf_stamp_image_npix_x = ncols * psf_stamp_size_pix
            psf_stamp_image_npix_y = nrows * psf_stamp_size_pix
        
        p_bulge_psf_image.append(galsim.Image(psf_stamp_image_npix_x,
                                              psf_stamp_image_npix_y,
                                              dtype=dithers[0].dtype,
                                              scale=dithers[0].scale/options['psf_scale_factor']))
        p_disk_psf_image.append( galsim.Image(psf_stamp_image_npix_x,
                                              psf_stamp_image_npix_y,
                                              dtype=dithers[0].dtype,
                                              scale=dithers[0].scale/options['psf_scale_factor']))

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
            if num_target_galaxies_printed % 10 == 0:
                logger.info("Printed " + str(num_target_galaxies_printed) + "/" +
                            str(num_target_galaxies) + " target galaxies.")
            num_target_galaxies_printed += 1
        else:
            if num_background_galaxies_printed % 50 == 0:
                logger.info("Printed " + str(num_background_galaxies_printed) + "/" +
                            str(num_background_galaxies) + " background galaxies.")
            num_background_galaxies_printed += 1

        # Get some galaxy info to avoid repeating method calls
        gal_I = get_I(galaxy.get_param_value('apparent_mag_vis'),
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
                                                model_psf_scale=options['model_psf_scale'],
                                                model_psf_offset=model_psf_offset)
            if options['chromatic_psf']:
                disk_psf_profile = get_psf_profile(n=gal_n,
                                                   z=gal_z,
                                                   bulge=False,
                                                   use_background_psf=use_background_psf,
                                                   data_dir=options['data_dir'],
                                                   model_psf_file_name=options['model_psf_file_name'],
                                                   model_psf_scale=options['model_psf_scale'],
                                                   model_psf_offset=model_psf_offset)
            else:
                    disk_psf_profile = bulge_psf_profile

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
                
                # Get psf position regardless    
                if single_output_psf:
                    psf_icol = psf_irow = 0
                else:
                    psf_icol, psf_irow = icol, irow
                psf_xp = psf_stamp_size_pix // 2 + psf_icol * psf_stamp_size_pix
                psf_yp = psf_stamp_size_pix // 2 + psf_irow * psf_stamp_size_pix
                

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

            subsampling_factor = int(pixel_scale / mv.psf_model_scale)

        else: # if not options['details_only']:
            # Store dummy values for pixel positions
            xp = -1
            yp = -1
            xc = -1
            yc = -1
            xp_sp_shift = 0
            yp_sp_shift = 0
            psf_xc = -1
            psf_yc = -1

        # Store galaxy data to save calls to the class

        rotation = galaxy.get_param_value('rotation')
        spin = galaxy.get_param_value('spin')
        tilt = galaxy.get_param_value('tilt')

        g_shear = galaxy.get_param_value('shear_magnitude')
        beta_shear = galaxy.get_param_value('shear_angle')

        g_ell = galaxy.get_param_value('bulge_ellipticity')

        bulge_fraction = galaxy.get_param_value('bulge_fraction')
        n = galaxy.get_param_value('sersic_index')

        bulge_size = galaxy.get_param_value('apparent_size_bulge')
        disk_size = galaxy.get_param_value('apparent_size_disk')
        disk_height_ratio=galaxy.get_param_value('disk_height_ratio')

        if not options['details_only']:
            if is_target_gal:

                bulge_gal_profile = get_bulge_galaxy_profile(sersic_index=n,
                                                half_light_radius=bulge_size,
                                                flux=gal_I * bulge_fraction,
                                                g_ell=g_ell,
                                                beta_deg_ell=rotation,
                                                g_shear=g_shear,
                                                beta_deg_shear=beta_shear,
                                                data_dir=options['data_dir'])

                # Convolve the galaxy, psf, and pixel profile to determine the final (well,
                # before noise) pixelized image
                final_bulge = galsim.Convolve([bulge_gal_profile, bulge_psf_profile],
                                              gsparams=default_gsparams)

                # Try to get a disk galaxy profile if the galsim version supports it
                disk_gal_profile = get_disk_galaxy_profile(half_light_radius=disk_size,
                                                           rotation=rotation,
                                                           tilt=tilt,
                                                           flux=gal_I * (1 - bulge_fraction),
                                                           g_shear=g_shear,
                                                           beta_deg_shear=beta_shear,
                                                           height_ratio=disk_height_ratio)

                final_disk = galsim.Convolve([disk_gal_profile, disk_psf_profile,
                                              galsim.Pixel(scale=pixel_scale)],
                                          gsparams=default_gsparams)
                    
                # Now draw the PSFs for this galaxy onto those images
                
                # Determine boundaries on the PSF image
                xl = psf_xp - psf_stamp_size_pix // 2 + 1
                xh = xl + psf_stamp_size_pix - 1
                yl = psf_yp - psf_stamp_size_pix // 2 + 1
                yh = yl + psf_stamp_size_pix - 1
        
                psf_bounds = galsim.BoundsI(xl, xh, yl, yh)
        
                # Get centers, correcting by 1.5 - 1 since Galsim is offset by 1, .5 to move from
                # corner of pixel to center
                psf_xc = psf_bounds.center().x
                psf_yc = psf_bounds.center().y
        
                # Draw the PSF image
                if (not single_output_psf) or (icol+irow==0):
                    bulge_psf_profile.drawImage(p_bulge_psf_image[0][psf_bounds],
                                                add_to_image=False,
                                                method='no_pixel',
                                                offset=(centre_offset,centre_offset))
                    disk_psf_profile.drawImage( p_disk_psf_image[0][psf_bounds],
                                                add_to_image=False,
                                                method='no_pixel',
                                                offset=(centre_offset,centre_offset))
                
            else:
                # Just use a single sersic profile for background galaxies
                # to make them more of a compromise between bulges and disks
                gal_profile = get_bulge_galaxy_profile(sersic_index=n,
                                                half_light_radius=bulge_size,
                                                flux=gal_I,
                                                g_ell=2.*g_ell,
                                                beta_deg_ell=rotation,
                                                g_shear=g_shear,
                                                beta_deg_shear=beta_shear,
                                                data_dir=options['data_dir'])

                # Convolve the galaxy, psf, and pixel profile to determine the final
                # (well, before noise) pixelized image
                final_gal = galsim.Convolve([gal_profile, disk_psf_profile],
                                              gsparams=default_gsparams)
                
                # Use dummy values for psf centre
                psf_xc = -1
                psf_yc = -1

            if not options['mode'] == 'stamps':
                if is_target_gal:
                    stamp_size_pix = 2 * (
                        np.max((int(options['stamp_size_factor'] * bulge_size / pixel_scale),
                                int(options['stamp_size_factor'] * disk_size / pixel_scale)))) + \
                                          int(np.max(np.shape(disk_psf_profile.image.array)) / subsampling_factor)
                else:
                    stamp_size_pix = 4 * (
                        np.max((int(options['stamp_size_factor'] * bulge_size / pixel_scale),
                                int(options['stamp_size_factor'] * disk_size / pixel_scale))))

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
            for di in xrange(num_dithers):
                gal_images.append(dithers[di][bounds])

            # Get centers, correcting by 1.5 - 1 since Galsim is offset by 1, .5 to move from
            # corner of pixel to center
            x_centre_offset = x_shift
            y_centre_offset = y_shift
            xc = bounds.center().x + centre_offset + x_centre_offset
            yc = bounds.center().y + centre_offset + y_centre_offset

            # Draw the image
            for gal_image, (x_offset, y_offset) in zip(gal_images, get_dither_scheme(options['dithering_scheme'])):

                if is_target_gal:
                    final_bulge.drawImage(gal_image, scale=pixel_scale,
                                          offset=(-x_centre_offset + x_offset + xp_sp_shift,
                                                  -y_centre_offset + y_offset + yp_sp_shift),
                                          add_to_image=True)
                    
                    disk_xp_sp_shift = xp_sp_shift
                    disk_yp_sp_shift = yp_sp_shift
                    
                    final_disk.drawImage(gal_image, scale=pixel_scale,
                                         offset=(-x_centre_offset + x_offset + disk_xp_sp_shift,
                                                 -y_centre_offset + y_offset + disk_yp_sp_shift),
                                         add_to_image=True,
                                         method='no_pixel')

                else:
                    final_gal.drawImage(gal_image, scale=pixel_scale,
                                         offset=(-x_centre_offset + x_offset + xp_sp_shift,
                                                 - y_centre_offset + y_offset + xp_sp_shift),
                                         add_to_image=True)
                    
                    


        # Record all data used for this galaxy in the output table
        if (not options['details_output_format']=='none') and (is_target_gal or (options['mode'] == 'field')):
            add_row(details_table,
                     ID=galaxy.get_full_ID(),
                     x_center_pix=xc + xp_sp_shift,
                     y_center_pix=yc + yp_sp_shift,
                     psf_x_center_pix=psf_xc,
                     psf_y_center_pix=psf_yc,
                     hlr_bulge_arcsec=bulge_size,
                     hlr_disk_arcsec=disk_size,
                     magnitude=galaxy.get_param_value('apparent_mag_vis'),
                     sersic_index=n,
                     bulge_ellipticity=g_ell,
                     bulge_axis_ratio=galaxy.get_param_value('bulge_axis_ratio'),
                     bulge_fraction=bulge_fraction,
                     disk_height_ratio=disk_height_ratio,
                     rotation=rotation,
                     tilt=tilt,
                     spin=spin,
                     shear_magnitude=g_shear,
                     shear_angle=beta_shear,
                     is_target_galaxy=is_target_gal)

        if is_target_gal and not options['details_only']:
            
            # Add to detections table only if it's a target galaxy
            add_row(detections_table,
                    ID=galaxy.get_full_ID(),
                    x_center_pix=int(xc + xp_sp_shift),
                    y_center_pix=int(yc + yp_sp_shift),
                    psf_x_center_pix=psf_xc,
                    psf_y_center_pix=psf_yc,
                    )
            
            del final_disk, disk_psf_profile

    logger.info("Finished printing galaxies.")

    return galaxies


def add_image_header_info(image, gain, stamp_size, full_options, model_seed):
    """
        @brief Adds various information to the image's header.

        @param image
            <galsim.Image> Galsim image object.
        @param full_options
            <dict> Full options dictionary
    """
    
    logger = getLogger(mv.logger_name)
    logger.debug("Entering add_image_header_info method.")

    # Add a header attribute if needed
    if not hasattr(image, "header"):
        image.header = galsim.FitsHeader()

    # SHE_GST_PhysicalModel package version label
    image.header[mv.version_label] = mv.version_str

    # Galsim version label
    if hasattr(galsim, "__version__"):
        image.header[mv.galsim_version_label] = galsim.__version__
    else:
        image.header[mv.galsim_version_label] = '<1.2'
        
    # Gain
    image.header[gain_label] = (gain,'e-/ADU')
    
    # Stamp size
    image.header[stamp_size_label] = stamp_size
    
    # Model hash
    image.header[model_hash_label] = hash(frozenset(full_options),format="base64")
    
    # Seeds
    image.header[model_seed_label] = model_seed
    image.header[noise_seed_label] = full_options["noise_seed"]
    
    logger.debug("Exiting add_image_header_info method.")
    
    return

def generate_image(image, options):
    """
        @brief Creates a single image of galaxies

        @details If successful, generates an image and corresponding details according to
            the configuration stored in the image and options objects.

        @param image
            <SHE_GST_PhysicalModel.Image> The Image-level object which specifies how galaxies are to be generated.
        @param options
            <dict> The options dictionary for this run.
    """

    logger = getLogger(mv.logger_name)
    logger.debug("Entering generate_image method.")

    logger.debug("# Printing image " + str(image.get_local_ID()) + " #")

    # Magic numbers

    centre_offset = -0.5

    # Setup
    
    image.autofill_children()

    # General setup from config
    num_dithers = len(get_dither_scheme(options['dithering_scheme']))

    # Setup for the file
    dithers = []
    noisemaps = []
    maskmaps = []
    model_hash = hash_any(frozenset(options.items()),format="base64")

    # Create the image object, using the appropriate method for the image type
    full_x_size = int(image.get_param_value("image_size_xp"))
    full_y_size = int(image.get_param_value("image_size_yp"))
    pixel_scale = image.get_param_value("pixel_scale")
    if not options['details_only']:
        for _ in xrange(num_dithers):
            if options['image_datatype'] == '32f':
                dithers.append(galsim.ImageF(full_x_size , full_y_size, scale=pixel_scale))
            elif options['image_datatype'] == '64f':
                dithers.append(galsim.ImageD(full_x_size , full_y_size, scale=pixel_scale))
            else:
                raise Exception("Bad image type slipped through somehow.")
    if options['mode']=='field':
        stamp_size_pix = 0
    else:
        stamp_size_pix = options['stamp_size']

    # Set up a table for output if necessary
    if options['details_output_format']=='none':
        detections_table = None
        details_table = None
    else:
        full_options = get_full_options(options,image)
        detections_table = initialise_detections_table(image, full_options)
        details_table = initialise_details_table(image, full_options)

    # Print the galaxies and psfs
    p_bulge_psf_image = []
    p_disk_psf_image = []
    galaxies = print_galaxies_and_psfs(image, options, centre_offset, num_dithers, dithers,
                                       p_bulge_psf_image, p_disk_psf_image,
                                       full_x_size, full_y_size, pixel_scale,
                                       detections_table, details_table)

    sky_level_subtracted = image.get_param_value('subtracted_background')
    sky_level_subtracted_pixel = sky_level_subtracted * pixel_scale ** 2
    sky_level_unsubtracted_pixel = image.get_param_value('unsubtracted_background') * pixel_scale ** 2

    # Get the initial noise deviate
    if not options['suppress_noise']:
        if options['noise_seed'] != 0:
            base_deviate = galsim.BaseDeviate(options['noise_seed'])
        else:
            base_deviate = galsim.BaseDeviate(image.get_full_seed() + 1)

    # For each dither
    for di, (x_offset, y_offset) in zip(range(num_dithers), get_dither_scheme(options['dithering_scheme'])):

        logger.debug("Printing dither " + str(di) + ".")
        
        # Make mock noise and mask maps for this dither
        if options['image_datatype'] == '32f':
            noisemaps.append(galsim.ImageF(np.ones_like(dithers[di].array), scale=pixel_scale))
        elif options['image_datatype'] == '64f':
            noisemaps.append(galsim.ImageD(np.ones_like(dithers[di].array), scale=pixel_scale))
            
        maskmaps.append(galsim.ImageS(np.zeros_like(dithers[di].array, dtype=np.int16), scale=pixel_scale))

        # If we're using cutouts, make the cutout image now
        if options['mode'] == 'cutouts':
            dithers[di] = make_cutout_image(dithers[di],
                                            options,
                                            galaxies,
                                            detections_table,
                                            details_table,
                                            centre_offset)
            noisemaps[di] = make_cutout_image(noisemaps[di],
                                              options,
                                              galaxies,
                                              detections_table,
                                              details_table,
                                              centre_offset)
            maskmaps[di] = make_cutout_image(maskmaps[di],
                                             options,
                                             galaxies,
                                             detections_table,
                                             details_table,
                                             centre_offset)

        
        if not options['details_only']:
            
            dither = dithers[di]
            dither += sky_level_unsubtracted_pixel
                
            # Add a header containing version info
            add_image_header_info(dither,options['gain'],stamp_size_pix,full_options,image.get_full_seed())

            if not options['suppress_noise']:
                
                noisemaps[di] = get_var_ADU_per_pixel(pixel_value_ADU=sky_level_unsubtracted_pixel*np.ones_like(dither.array),
                                                        sky_level_ADU_per_sq_arcsec=sky_level_subtracted,
                                                        read_noise_count=options['read_noise'],
                                                        pixel_scale=pixel_scale,
                                                        gain=options['gain'])
                
                if options['stable_rng']:
                    var_array = get_var_ADU_per_pixel(pixel_value_ADU=dither.array,
                                    sky_level_ADU_per_sq_arcsec=sky_level_subtracted,
                                    read_noise_count=options['read_noise'],
                                    pixel_scale=pixel_scale,
                                    gain=options['gain'])
                    noise = galsim.VariableGaussianNoise(base_deviate,
                                                         var_array)
                else:
                    noise = galsim.CCDNoise(base_deviate,
                                            gain=options['gain'],
                                            read_noise=options['read_noise'],
                                            sky_level=sky_level_subtracted_pixel)
                
                # Set up noise inversion as necessary
                if options['noise_cancellation']=='false':
                    dither.addNoise(noise)
                    dithers[di] = [(dither, '')]
                elif options['noise_cancellation']=='true':
                    dither_copy = deepcopy(dither)
                    dither.addNoise(noise)
                    dithers[di] = [(2*dither_copy-dither, 'i')]
                    del dither_copy
                elif options['noise_cancellation']=='both':
                    dither_copy = deepcopy(dither)
                    dither.addNoise(noise)
                    dithers[di] = [(dither, ''),
                                   (2*dither_copy-dither, 'i')]
                    del dither_copy
                else:
                    raise Exception("Invalid value for noise_cancellation: " + str(options['noise_cancellation']))
            else:
                noisemaps[di] *= 0
                dithers[di] = [(dithers[di], '')]
    
            for dither_and_flag in dithers[di]:
                    
                dither = dither_and_flag[0]
                flag = dither_and_flag[1]
            
                dither_file_name = get_allowed_filename( "GST-SCI-"+str[di]+flag, model_hash, extension=".fits")
                noise_file_name = get_allowed_filename( "GST-RMS-"+str[di]+flag, model_hash, extension=".fits")
                mask_file_name = get_allowed_filename( "GST-MSK-"+str[di]+flag, model_hash, extension=".fits")
                    
                galsim.fits.write(dither, dither_file_name, clobber=True)
                galsim.fits.write(noisemaps[di], noise_file_name, clobber=True)
                galsim.fits.write(maskmaps[di], mask_file_name, clobber=True)
        
                # Compress the image if necessary
                if options['compress_images'] >= 1:
                    compress_image(dither_file_name, lossy=(options['compress_images'] >= 2))
                    compress_image(noise_file_name, lossy=(options['compress_images'] >= 2))
                    compress_image(mask_file_name, lossy=(options['compress_images'] >= 2))


        # Output the datafile if necessary

        if not options['details_output_format']=='none':
            # Temporarily adjust centre positions by dithering
            details_table[datf.gal_x] += x_offset
            details_table[datf.gal_y] += y_offset
            
            detections_file_name = get_allowed_filename( "GST-DTC-"+str[di], model_hash, extension=".fits" )
            details_file_name = get_allowed_filename( "GST-DAL-"+str[di], model_hash, extension=".fits" )
    
            output_tables(detections_table, detections_file_name, options)
            output_tables(details_table, details_file_name, options)
    
            # Undo dithering adjustment
            details_table[datf.gal_x] -= x_offset
            details_table[datf.gal_y] -= y_offset

        logger.info("Finished printing dither " + str(di) + ".")

    logger.info("Finished printing image " + str(image.get_local_ID()) + ".")
    
    # Output the psf images
    for tag, label, p_psf_image in (("B","bulge", p_bulge_psf_image), ("D","disk", p_disk_psf_image) ):
        for psf_image in p_psf_image:
            logger.debug("Printing "+label+" psf image")
            
            add_image_header_info(psf_image,1.,options['psf_stamp_size'],full_options,image.get_full_seed())
        
            # Get the base name for this combined image
            psf_file_name = get_allowed_filename("GST-PSF-"+tag, model_hash, extension=".fits" )
        
            # Output the new image
            galsim.fits.write(psf_image, psf_file_name, clobber=True)
        
            logger.info("Finished printing "+label+" psf image.")

    # We no longer need this image's children, so clear it to save memory
    image.clear()

    logger.debug("Exiting generate_image method.")
    return
