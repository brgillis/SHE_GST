""" @file gain.py

    Created 22 Mar 2017

    Functions to handle needed conversions and calculations for noise
    in simulated images.
"""

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

import galsim

from SHE_GST_GalaxyImageGeneration.gain import get_ADU_from_count, get_count_from_ADU
import numpy as np


def get_sky_level_ADU_per_pixel(sky_level_ADU_per_sq_arcsec,
                                pixel_scale):
    """ Calculate the sky level in units of ADU per pixel from the sky level per square arcsecond.

        @param sky_level_ADU_per_sq_arcsec The sky level in units of ADU/arcsec^2
        @param pixel_scale The pixel scale in units of arcsec/pixel

        @return The sky level in units of ADU/pixel
    """

    sky_level_ADU_per_pixel = sky_level_ADU_per_sq_arcsec * pixel_scale ** 2

    return sky_level_ADU_per_pixel


def get_sky_level_count_per_pixel(sky_level_ADU_per_sq_arcsec,
                                  pixel_scale,
                                  gain):
    """ Calculate the sky level in units of count per pixel from the sky level per square arcsecond.

        @param sky_level_ADU_per_sq_arcsec The sky level in units of ADU/arcsec^2
        @param pixel_scale The pixel scale in units of arcsec/pixel
        @param gain The gain in units of e-/ADU

        @return The sky level in units of e-/pixel
    """

    sky_level_ADU_per_pixel = get_sky_level_ADU_per_pixel(sky_level_ADU_per_sq_arcsec, pixel_scale)
    sky_level_count_per_pixel = get_count_from_ADU(sky_level_ADU_per_pixel, gain)

    return sky_level_count_per_pixel


def get_count_lambda_per_pixel(pixel_value_ADU,
                               sky_level_ADU_per_sq_arcsec,
                               pixel_scale,
                               gain):
    """ Calculate the lambda of the Poisson distribution for a pixel's noise.

        @param pixel_value The expected value of a pixel in ADU. Can be a scalar or array
        @param sky_level_ADU_per_sq_arcsec The sky level in units of ADU/arcsec^2
        @param pixel_scale The pixel scale in units of arcsec/pixel
        @param gain The gain in units of e-/ADU

        @return The lambda of the Poisson distribution in units of e-
    """

    pixel_value_count = get_count_from_ADU(pixel_value_ADU, gain)

    sky_level_count_per_pixel = get_sky_level_count_per_pixel(sky_level_ADU_per_sq_arcsec,
                                                              pixel_scale, gain)

    count_lambda = pixel_value_count + sky_level_count_per_pixel

    return count_lambda


def get_read_noise_ADU_per_pixel(read_noise_count,
                                 gain):
    """ Calculate the read noise per pixel in units of ADU

        @param read_noise_count The read noise in e-/pixel
        @param gain The gain in units of e-/ADU

        @return The read noise per pixel in units of ADU
    """

    read_noise_ADU_per_pixel = get_ADU_from_count(read_noise_count, gain)

    return read_noise_ADU_per_pixel


def get_var_ADU_per_pixel(pixel_value_ADU,
                          sky_level_ADU_per_sq_arcsec,
                          read_noise_count,
                          pixel_scale,
                          gain):
    """ Calculate the sigma for Gaussian-like noise in units of ADU per pixel.

        @param pixel_value The expected value of a pixel in ADU. Can be a scalar or array
        @param sky_level_ADU_per_sq_arcsec The sky level in units of ADU/arcsec^2
        @param read_noise_count The read noise in e-/pixel
        @param pixel_scale The pixel scale in units of arcsec/pixel
        @param gain The gain in units of e-/ADU

        @return The sigma of the total noise in units of ADU per pixel
    """

    pois_count_lambda = get_count_lambda_per_pixel(pixel_value_ADU,
                                                   sky_level_ADU_per_sq_arcsec, pixel_scale, gain)
    # Apply twice since it's squared
    pois_ADU_var = get_ADU_from_count(get_ADU_from_count(pois_count_lambda, gain), gain)

    read_noise_ADU_sigma = get_read_noise_ADU_per_pixel(read_noise_count, gain)

    total_var = pois_ADU_var + read_noise_ADU_sigma ** 2

    return total_var


def add_stable_noise(image,
                     base_deviate,
                     var_array,
                     image_phl,
                     options):
    """ Adds stable noise to an image.
    """

    assert options['shape_noise_cancellation']

    # If not in stamp mode, add noise simply
    if not options['mode'] == 'stamps':
        image.addNoise(galsim.VariableGaussianNoise(base_deviate,
                                                    var_array))
        return

    # Figure out how to set up the grid for galaxy stamps, making it as square as possible
    num_target_galaxies = len(image.get_galaxy_descendants())
    ncols = int(np.ceil(np.sqrt(num_target_galaxies)))
    if ncols == 0:
        ncols = 1
    nrows = int(np.ceil(num_target_galaxies / ncols))
    if nrows == 0:
        nrows = 1

    # Indices to keep track of row and column we're drawing galaxy/psf to
    icol = -1
    irow = 0

    stamp_size_pix = options['stamp_size']

    # In stamp mode, add to each galaxy group's stamps the same way

    galaxy_groups = image_phl.get_galaxy_group_descendants()
    for galaxy_group in galaxy_groups:

        # Only want to advance deviate once per group, which we do by copying again here
        base_deviate_backup = base_deviate

        # Add the same noise to each galaxy's stamp
        galaxies = galaxy_group.get_galaxy_descendents()
        for _galaxy in galaxies:

            # Increment position
            icol += 1
            if icol >= ncols:
                icol = 0
                irow += 1
                if irow >= nrows:
                    raise Exception("More galaxies than expected when printing stamps.")

            base_deviate = deepcopy(base_deviate_backup)

            xp_l = 1 + icol * stamp_size_pix
            xp_h = stamp_size_pix + icol * stamp_size_pix
            yp_l = 1 + irow * stamp_size_pix
            yp_h = stamp_size_pix + irow * stamp_size_pix

            stamp_bounds = galsim.BoundsI(xmin=xp_l, xmax=xp_h, ymin=yp_l, ymax=yp_h)
            image_stamp = image.subImage(stamp_bounds)
            var_array_stamp = var_array.subImage(stamp_bounds)

            image_stamp.addNoise(galsim.VariableGaussianNoise(base_deviate,
                                                              var_array_stamp))

    return
