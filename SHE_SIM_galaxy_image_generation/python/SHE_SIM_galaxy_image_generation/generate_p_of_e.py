""" @file generate_p_of_e.py

    Created 11 Apr 2017

    This module contains the functions which do the heavy lifting of
    generating P(e)

    ---------------------------------------------------------------------

    Copyright (C) 2017 Bryan R. Gillis

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

import numpy as np
import galsim

from icebrgpy.logging import getLogger
from SHE_SIM_galaxy_image_generation import magic_values as mv
from SHE_SIM_galaxy_image_generation.galaxy import (get_bulge_galaxy_profile,
                                             get_disk_galaxy_profile,
                                             is_target_galaxy)
from SHE_SIM_galaxy_image_generation.magnitude_conversions import get_I
from SHE_SIM_galaxy_image_generation.unweighted_moments import calculate_unweighted_ellipticity
from SHE_SIM_galaxy_image_generation.p_of_e_io import output_p_of_e

def generate_p_of_e(survey, options, header_items, e_bins):
    """
        @brief This function handles assigning specific images to be created by different parallel
            threads.

        @details If successful, generates images and corresponding details according to
            the configuration stored in the survey and options objects.

        @param survey
            <SHE_SIM.Survey> The survey object which specifies parameters for generation
        @param options
            <dict> The options dictionary for this run
    """
    
    logger = getLogger(mv.logger_name)
    logger.debug("Entering generate_p_of_e method.")

    # Seed the survey
    if options['seed'] == 0:
        survey.set_seed() # Seed from the time
    else:
        survey.set_seed(options['seed'])
        
    # Set up the bins for e
    pe_bins = np.zeros(e_bins,dtype=int)
    e_samples = []

    # Create empty image objects for the survey
    survey.fill_images()
    images = survey.get_images()

    for image in images:
        image_pe_bins, image_e_samples = get_pe_bins_for_image(image, options, e_bins)
        pe_bins += image_pe_bins
        e_samples += image_e_samples
        
    # Set up output header as specified at input
    header = {}
    for i in range(len(header_items)//2):
        header[header_items[2*i]] = header_items[2*i+1]
        
    output_p_of_e(pe_bins,e_samples,options['p_of_e_output_file_name'],header=header)
        
    logger.debug("Exiting generate_p_of_e method.")
    
def get_pe_bins_for_image(image, options, e_bins):
    
    logger = getLogger(mv.logger_name)
    logger.debug("Entering get_pe_bins_for_image method.")
    
    # Set up empty bins
    e_bin_size = 1./e_bins
    
    image_pe_bins = np.zeros(e_bins,dtype=int)
    image_e_samples = []

    # Fill up galaxies in this image
    image.autofill_children()
    
    i=0
    
    while i<options['num_target_galaxies']:
        galaxy = image.add_galaxy()

        # Sort out target galaxies
        if not is_target_galaxy(galaxy, options):
            continue
        
        if i%10 == 0:
            logger.info("Calculating P(e) for galaxy " + str(i) + ".")
        i += 1
        
        # Store galaxy data to save calls to the class

        gal_I = get_I(galaxy.get_param_value('apparent_mag_vis'),
                      'mag_vis',
                      gain=options['gain'],
                      exp_time=options['exp_time'])
        
        rotation = galaxy.get_param_value('rotation')
        tilt = galaxy.get_param_value('tilt')

        g_shear = galaxy.get_param_value('shear_magnitude')
        beta_shear = galaxy.get_param_value('shear_angle')

        g_ell = galaxy.get_param_value('bulge_ellipticity')

        bulge_fraction = galaxy.get_param_value('bulge_fraction')
        n = galaxy.get_param_value('sersic_index')

        bulge_size = galaxy.get_param_value('apparent_size_bulge')
        disk_size = galaxy.get_param_value('apparent_size_disk')
        disk_height_ratio=galaxy.get_param_value('disk_height_ratio')
        
        gsparams = galsim.GSParams(maxk_threshold=5e-2)

        bulge_gal_profile = get_bulge_galaxy_profile(sersic_index=n,
                                        half_light_radius=bulge_size,
                                        flux=gal_I * bulge_fraction,
                                        g_ell=g_ell,
                                        beta_deg_ell=rotation,
                                        g_shear=g_shear,
                                        beta_deg_shear=beta_shear,
                                        data_dir=options['data_dir'],
                                        gsparams=gsparams)
        disk_gal_profile = get_disk_galaxy_profile(half_light_radius=disk_size,
                                           rotation=rotation,
                                           tilt=tilt,
                                           flux=gal_I * (1 - bulge_fraction),
                                           g_shear=g_shear,
                                           beta_deg_shear=beta_shear,
                                           height_ratio=disk_height_ratio,
                                           gsparams=gsparams)
        
        gal_profile = bulge_gal_profile + disk_gal_profile
        
        e1, e2 = calculate_unweighted_ellipticity(gal_profile)
        
        e = np.sqrt(e1**2 + e2**2)
        
        bin_index = int(e/e_bin_size)
        image_pe_bins[bin_index] += 1
        
        image_e_samples.append(e)
        

    # We no longer need this image's children, so clear it to save memory
    image.clear()

    logger.debug("Exiting get_pe_bins_for_image method.")

    return image_pe_bins, image_e_samples
    
    