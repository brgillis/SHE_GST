""" @file psf.py

    Created 11 Dec 2015

    @TODO: File docstring
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

from os.path import join

import galsim

import SHE_GST_GalaxyImageGeneration.magic_values as mv
from SHE_GST_IceBRGpy.function_cache import lru_cache
import numpy as np


sed_names = {'ell':'el_cb2004a_001',
             'sbc':'sbc_cb2004a_001',
             'scd':'scd_cb2004a_001',
             'sb2':'sb2_b2004a_001',
             'sb3':'sb3_b2004a_001',
             }

seds = {4.0:'sbc',
        3.5:'sbc',
        3.0:'sbc',
        2.71:'scd',
        2.56:'sb2',
        2.0:'sb3',
        1.8:'sb3',
        }

allowed_ns = np.array((1.8, 2.0, 2.56, 2.71, 3.0, 3.5, 4.0))
allowed_zs = np.array((0., 0.5, 1.0, 1.5, 2.0))

@lru_cache()
def load_psf_model_from_sed_z(sed, z=0.0, data_dir=mv.default_data_dir):

    z_str = "%0.2f" % z

    model_filename = join(data_dir, "psf_models", sed_names[sed] + ".fits_0.000_0.804_" + z_str + ".fits")
    
    return load_psf_model_from_file(model_filename, scale=mv.psf_model_scale,
                                    offset=mv.default_psf_center_offset)

@lru_cache()
def load_psf_model_from_file(file_name, scale, offset):

    model = galsim.fits.read(file_name)

    return galsim.InterpolatedImage(model, scale=scale,
                                    offset=offset)

@lru_cache()
def get_background_psf_profile():

    prof = galsim.OpticalPSF(lam=725, # nm
                             diam=1.2, # m
                             defocus=0,
                             obscuration=0.33,
                             nstruts=3,
                             )

    return prof

def get_psf_profile(n, z, bulge, use_background_psf=False, data_dir=mv.default_data_dir, 
                    model_psf_file_name=None, model_psf_scale=mv.psf_model_scale,
                    model_psf_offset=mv.default_psf_center_offset):

    if use_background_psf:
        return get_background_psf_profile()
    
    if model_psf_file_name is not None:
        return load_psf_model_from_file(model_psf_file_name, model_psf_scale, model_psf_offset)

    diffs = np.abs(allowed_zs - z)
    zi_best = np.argmin(diffs)

    if(bulge):
        sed = 'ell'
    else:
        diffs = np.abs(allowed_ns - n)
        ni_best = np.argmin(diffs)
        sed = seds[allowed_ns[ni_best]]

    return load_psf_model_from_sed_z(sed, allowed_zs[zi_best], data_dir=data_dir)
