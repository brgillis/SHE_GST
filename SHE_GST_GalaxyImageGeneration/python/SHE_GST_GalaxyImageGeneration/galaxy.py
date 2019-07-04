""" @file galaxy.py

    Created 11 Dec 2015

    @TODO: File docstring
"""

__updated__ = "2019-07-04"

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

from functools import lru_cache
from os.path import join

import galsim

import SHE_GST_GalaxyImageGeneration.magic_values as mv
from SHE_PPT.file_io import find_file
from SHE_PPT.logging import getLogger
import numpy as np


__all__ = ['get_galaxy_profile']

try:
    from galsim import InclinedSersic
except ImportError:
    err = "GalSim's InclinedSersic profile is not available."
    getLogger(__name__).error(err)
    raise ImportError(err)

allowed_ns = np.array((1.8, 2.0, 2.56, 2.71, 3.0, 3.5, 4.0))


def is_target_galaxy(galaxy, options):
    return galaxy.get_param_value('apparent_mag_vis') <= options['magnitude_limit']


def rotate(x, y, theta_deg):

    theta = theta_deg * np.pi / 180
    sin_theta = np.sin(theta)
    cos_theta = np.cos(theta)

    new_x, new_y = (x * cos_theta - y * sin_theta,
                    x * sin_theta + y * cos_theta)

    return new_x, new_y


def shear(x, y, g, beta_deg):

    beta = beta_deg * np.pi / 180
    sin_2beta = np.sin(2 * beta)
    cos_2beta = np.cos(2 * beta)

    new_x, new_y = (x + g * (x * cos_2beta + y * sin_2beta),
                    y + g * (x * sin_2beta - y * cos_2beta))

    return new_x, new_y


def get_target_galaxy_profile(sersic_index,
                              half_light_radius,
                              bulge,
                              **kwargs):
    """
    """

    if bulge:
        return get_bulge_galaxy_profile(sersic_index, half_light_radius, **kwargs)
    else:
        return get_disk_galaxy_image(sersic_index, half_light_radius, **kwargs)


def get_background_galaxy_profile(sersic_index,
                                  half_light_radius,
                                  bulge,
                                  **kwargs):
    """
    """

    # Always use the faster get_bulge_galaxy_profile for background galaxies
    return get_bulge_galaxy_profile(sersic_index, half_light_radius, **kwargs)


def discretize(n, res=0.05):
    return res * (int(n / res) + 0.5)


def get_bulge_galaxy_profile(sersic_index,
                             half_light_radius,
                             flux=1.,
                             g_ell=0.,
                             beta_deg_ell=0.,
                             g_shear=0.,
                             beta_deg_shear=0.,
                             trunc_factor=4.5,
                             gsparams=galsim.GSParams()):
    n = discretize(sersic_index)
    
    scale_radius_deg = half_light_radius / (3600 * galsim.Exponential._hlr_factor)

    gal_profile = galsim.Sersic(n=n,
                                half_light_radius=half_light_radius / 3600,  # Convert to deg
                                flux=flux,
                                trunc=trunc_factor * scale_radius_deg,
                                gsparams=gsparams)

    shear_ell = galsim.Shear(g=g_ell, beta=beta_deg_ell * galsim.degrees)
    shear_lensing = galsim.Shear(g=g_shear, beta=beta_deg_shear * galsim.degrees)

    gal_profile = gal_profile.shear(shear_lensing + shear_ell)

    return gal_profile


def get_disk_galaxy_profile(half_light_radius,
                            rotation=0.,
                            tilt=0.,
                            flux=1.,
                            g_shear=0.,
                            beta_deg_shear=0.,
                            height_ratio=0.1,
                            trunc_factor=4.5,
                            gsparams=galsim.GSParams()):

    # Use galsim's hardcoded half-light-radius factor to get scale radius
    # (where hlr is hlr for face-on profile specifically)
    scale_radius_deg = half_light_radius / (3600 * galsim.Exponential._hlr_factor)  # Convert to deg as well

    base_prof = InclinedSersic(n=1.,
                               inclination=tilt * galsim.degrees,
                               scale_radius=scale_radius_deg,
                               trunc=trunc_factor * scale_radius_deg,
                               flux=flux,
                               scale_h_over_r=height_ratio,
                               gsparams=gsparams)

    rotated_prof = base_prof.rotate(rotation * galsim.degrees)

    final_prof = rotated_prof.shear(g=g_shear,
                                    beta=beta_deg_shear * galsim.degrees)

    return final_prof
