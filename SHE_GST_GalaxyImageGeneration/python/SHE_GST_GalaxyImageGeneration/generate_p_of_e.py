""" @file generate_p_of_e.py

    Created 11 Apr 2017

    This module contains the functions which do the heavy lifting of
    generating P(e)
"""

__updated__ = "2018-07-03"

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

from builtins import isinstance
from os.path import join

import galsim

from SHE_GST_GalaxyImageGeneration import magic_values as mv
from SHE_GST_GalaxyImageGeneration.galaxy import (get_bulge_galaxy_profile,
                                             get_disk_galaxy_profile,
                                             is_target_galaxy)
from SHE_GST_GalaxyImageGeneration.magnitude_conversions import get_I
from SHE_GST_GalaxyImageGeneration.p_of_e_io import output_p_of_e
from SHE_GST_GalaxyImageGeneration.unweighted_moments import calculate_unweighted_ellipticity
from SHE_PPT.logging import getLogger
import numpy as np


def generate_p_of_e(survey, options, output_file_name, header_items, e_bins):
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
    logger.debug("Entering generate_p_of_e method.")

    # Seed the survey
    if options['seed'] == 0:
        survey.set_seed()  # Seed from the time
    else:
        survey.set_seed(options['seed'])

    # Set up the bins for e
    pe_bins = np.zeros(e_bins, dtype = int)
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
    for i in range(len(header_items) // 2):
        header[header_items[2 * i]] = header_items[2 * i + 1]

    joined_file_name = join(options["output_folder"], output_file_name)

    output_p_of_e(pe_bins, e_samples, joined_file_name, header = header)

    logger.debug("Exiting generate_p_of_e method.")

def get_pe_bins_for_image(image, options, e_bins):

    logger = getLogger(mv.logger_name)
    logger.debug("Entering get_pe_bins_for_image method.")

    # Set up empty bins
    e_bin_size = 1. / e_bins

    image_pe_bins = np.zeros(e_bins, dtype = int)
    image_e_samples = []

    # Fill up galaxies in this image
    image.autofill_children()

    i = 0

    while i < options['num_target_galaxies']:
        galaxy = image.add_galaxy()

        # Sort out target galaxies
        if not is_target_galaxy(galaxy, options):
            continue

        if i % 10 == 0:
            logger.info("Calculating P(e) for galaxy " + str(i) + ".")
        i += 1

        # Store galaxy data to save calls to the class

        gal_I = get_I(galaxy.get_param_value('apparent_mag_vis'),
                      'mag_vis',
                      gain = options['gain'],
                      exp_time = options['exp_time'])

        rotation = galaxy.get_param_value('rotation')
        tilt = galaxy.get_param_value('tilt')

        g_shear = galaxy.get_param_value('shear_magnitude')
        beta_shear = galaxy.get_param_value('shear_angle')

        g_ell = galaxy.get_param_value('bulge_ellipticity')

        bulge_fraction = galaxy.get_param_value('bulge_fraction')
        n = galaxy.get_param_value('sersic_index')

        bulge_size = galaxy.get_param_value('apparent_size_bulge')
        disk_size = galaxy.get_param_value('apparent_size_disk')
        disk_height_ratio = galaxy.get_param_value('disk_height_ratio')

        gsparams = galsim.GSParams(maxk_threshold = 5e-2)

        bulge_gal_profile = get_bulge_galaxy_profile(sersic_index = n,
                                        half_light_radius = bulge_size,
                                        flux = gal_I * bulge_fraction,
                                        g_ell = g_ell,
                                        beta_deg_ell = rotation,
                                        g_shear = g_shear,
                                        beta_deg_shear = beta_shear,
                                        data_dir = options['data_dir'],
                                        gsparams = gsparams)
        disk_gal_profile = get_disk_galaxy_profile(half_light_radius = disk_size,
                                           rotation = rotation,
                                           tilt = tilt,
                                           flux = gal_I * (1 - bulge_fraction),
                                           g_shear = g_shear,
                                           beta_deg_shear = beta_shear,
                                           height_ratio = disk_height_ratio,
                                           gsparams = gsparams)

        gal_profile = bulge_gal_profile + disk_gal_profile

        try:
            e1, e2 = calculate_unweighted_ellipticity(gal_profile)

            e = np.sqrt(e1 ** 2 + e2 ** 2)

            bin_index = int(e / e_bin_size)
            image_pe_bins[bin_index] += 1

            image_e_samples.append(e)
        except Exception as e:
            if not ("image of all zeroes" in str(e) or isinstance(e, ValueError)):
                raise
            warn_str = ("Bad galaxy ellipticity." +
                        "\nBulge: " +
                        "\nsersic_index = " + str(n) +
                        "\nhalf_light_radius = " + str(bulge_size) +
                        "\nflux = " + str(gal_I * bulge_fraction) +
                        "\ng_ell = " + str(g_ell) +
                        "\nbeta_deg_ell = " + str(rotation) +
                        "\ng_shear = " + str(g_shear) +
                        "\nbeta_deg_shear = " + str(beta_shear) +
                        "\nDisk: " +
                        "\nhalf_light_radius = " + str(disk_size) +
                        "\ntilt = " + str(tilt) +
                        "\nflux = " + str(gal_I * (1 - bulge_fraction)) +
                        "\ng_shear = " + str(g_shear) +
                        "\nbeta_deg_shear = " + str(beta_shear) +
                        "\nheight_ratio = " + str(disk_height_ratio))
            logger.warn(warn_str)


    # We no longer need this image's children, so clear it to save memory
    image.clear()

    logger.debug("Exiting get_pe_bins_for_image method.")

    return image_pe_bins, image_e_samples

