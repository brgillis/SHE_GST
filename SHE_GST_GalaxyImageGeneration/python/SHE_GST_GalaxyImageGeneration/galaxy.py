""" @file galaxy.py

    Created 11 Dec 2015

    @TODO: File docstring

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

from os.path import join

import galsim

import SHE_GST_GalaxyImageGeneration.magic_values as mv
from SHE_GST_IceBRGpy.function_cache import lru_cache
from SHE_GST_IceBRGpy.logging import getLogger
import numpy as np

__all__ = ['get_galaxy_profile']

try:
    from galsim import InclinedSersic
except ImportError:
    err = "GalSim's InclinedSersic profile is not available."
    getLogger(mv.logger_name).error(err)
    raise ImportError(err)

allowed_ns = np.array((1.8, 2.0, 2.56, 2.71, 3.0, 3.5, 4.0))

def is_target_galaxy(galaxy, options):
    return galaxy.get_param_value('apparent_mag_vis') <= options['magnitude_limit']

@lru_cache()
def load_galaxy_model_from_file(n, bulge=True, data_dir=mv.default_data_dir):

    n_str = "%0.2f" % n

    if bulge:
        model_filename = mv.bulge_model_head + n_str + mv.galaxy_model_tail
    else:
        model_filename = mv.disk_model_head + n_str + mv.galaxy_model_tail

    model = np.loadtxt(join(data_dir, mv.galaxy_model_path, model_filename))

    x = model[:, 0]
    y = model[:, 1]
    if bulge:
        z = None
        I = model[:, 2]
    else:
        z = model[:, 2]
        I = model[:, 3]

    return x, y, z, I

def load_galaxy_model(n, bulge=True, data_dir=mv.default_data_dir):
    diffs = np.abs(allowed_ns - n)
    i_best = np.argmin(diffs)

    return load_galaxy_model_from_file(allowed_ns[i_best], bulge, data_dir=data_dir)

def rotate(x, y, theta_deg):


    theta = theta_deg * np.pi / 180
    sin_theta = np.sin(theta)
    cos_theta = np.cos(theta)

    new_x, new_y = (x * cos_theta - y * sin_theta ,
                     x * sin_theta + y * cos_theta)

    return new_x, new_y

def shear(x, y, g, beta_deg):


    beta = beta_deg * np.pi / 180
    sin_2beta = np.sin(2 * beta)
    cos_2beta = np.cos(2 * beta)

    new_x, new_y = (x + g * (x * cos_2beta + y * sin_2beta) ,
                     y + g * (x * sin_2beta - y * cos_2beta))

    return new_x, new_y

def get_half_light_radius(x, y, I):
    """ NOTE: Requires I to be normalized
    """

    r = np.sqrt(np.square(x) + np.square(y))

    r_step = 0.001 * np.max(r)

    goal_I = 0.5
    cur_I = 0

    test_r = r_step
    while cur_I < goal_I:
        cur_I = I[r < test_r].sum()
        test_r += r_step

    return test_r - r_step / 2

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
                             data_dir=mv.default_data_dir,
                             gsparams=galsim.GSParams()):
    n = discretize(sersic_index)

    gal_profile = galsim.Sersic(n=n,
                                half_light_radius=half_light_radius,
                                flux=flux,
                                gsparams=gsparams)

    shear_ell = galsim.Shear(g=g_ell, beta=beta_deg_ell * galsim.degrees)
    shear_lensing = galsim.Shear(g=g_shear, beta=beta_deg_shear * galsim.degrees)

    gal_profile = gal_profile.shear(shear_ell + shear_lensing)

    return gal_profile

def get_disk_galaxy_profile(half_light_radius,
                            rotation=0.,
                            tilt=0.,
                            flux=1.,
                            g_shear=0.,
                            beta_deg_shear=0.,
                            height_ratio=0.1,
                            gsparams=galsim.GSParams()):

    # Use galsim's hardcoded half-light-radius factor to get scale radius
    # (where hlr is hlr for face-on profile specifically)
    scale_radius = half_light_radius / galsim.Exponential._hlr_factor

    base_prof = InclinedSersic(n=1.,
                               inclination=tilt * galsim.degrees,
                               half_light_radius=half_light_radius,
                               trunc=mv.default_truncation_radius_factor*scale_radius,
                               flux=flux,
                               scale_h_over_r=height_ratio,
                               gsparams=gsparams)

    rotated_prof = base_prof.rotate(rotation * galsim.degrees)

    final_prof = rotated_prof.shear(g=g_shear,
                                 beta=beta_deg_shear * galsim.degrees)

    return final_prof

def get_disk_galaxy_image(sersic_index,
                          half_light_radius,
                          stamp_size_factor=4.5,
                          rotation=0.,
                          tilt=0.,
                          spin=0.,
                          flux=1.,
                          data_dir=mv.default_data_dir,
                          image_scale=mv.default_pixel_scale,
                          g_shear=0.,
                          beta_deg_shear=0.,
                          xp_sp_shift=0,
                          yp_sp_shift=0,
                          subsampling_factor=1,
                          height_ratio=0.1):

    gal_x, gal_y, gal_z, gal_I = load_galaxy_model(sersic_index, False, data_dir)
    
    # Adjust for the height ratio
    gal_z *= height_ratio/0.1

    # Apply spin, tilt, and rotation
    gal_x, gal_y = rotate(gal_x, gal_y, spin)
    gal_x, gal_z = rotate(gal_x, gal_z, tilt)
    gal_x, gal_y = rotate(gal_x, gal_y, -rotation) # Rotation is opposite here due to transposed ordering

    # Apply shear

    gal_x, gal_y = shear(gal_x, gal_y, g_shear, 90 - beta_deg_shear) # Again, transposed ordering

    # Normalize the flux
    gal_I /= np.sum(gal_I)

    # Get the half-light radius
    gal_hlr = get_half_light_radius(gal_x, gal_y, gal_I)
    image_hlr = subsampling_factor * half_light_radius / image_scale # hlr in pixels
    gal_scale = image_hlr / gal_hlr

    image_size = 2 * subsampling_factor * int(stamp_size_factor * image_hlr + 1)

    # Create a binned image of the galaxy
    gal_data = np.zeros((image_size, image_size))
    image_midpoint = (image_size - 1) // 2

    for x, y, I in zip(gal_x, gal_y, gal_I):
        dx = x * gal_scale + xp_sp_shift
        dy = y * gal_scale + yp_sp_shift

        xp = image_midpoint + int(dx + 0.5 * (1 if dx >= 0 else -1))
        yp = image_midpoint + int(dy + 0.5 * (1 if dy >= 0 else -1))

        if (xp < 0) or (yp < 0) or (xp >= image_size) or (yp >= image_size):
            continue

        gal_data[xp, yp] += I

    return gal_data
