""" @file psf.py

    Created 11 Dec 2015

    @TODO: File docstring

    ---------------------------------------------------------------------

    Copyright (C) 2015 Bryan R. Gillis

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

import galsim
import numpy as np
from os.path import join

import galsim_images_generation.magic_values as mv

from icebrgpy.function_cache import lru_cache

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

allowed_ns = np.array(( 1.8, 2.0, 2.56, 2.71, 3.0, 3.5, 4.0))
allowed_zs = np.array(( 0., 0.5, 1.0, 1.5, 2.0))

@lru_cache()
def load_psf_model_from_file( sed, z=0.0, data_dir=mv.default_data_dir ):

    z_str = "%0.2f" % z

    model_filename = sed_names[sed] + ".fits_0.000_0.804_" + z_str + ".fits"
    
    model = galsim.fits.read(join(data_dir,mv.psf_model_path,model_filename))
    
    return galsim.InterpolatedImage(model,scale=mv.psf_model_scale)

@lru_cache()
def get_background_psf_profile():
    
    prof = galsim.OpticalPSF(lam=725, # nm
                             diam=1.2, # m
                             defocus=0,
                             obscuration=0.33,
                             nstruts=3,
                             )
    
    return prof

def get_psf_profile(n, z, bulge, use_background_psf=False, data_dir=mv.default_data_dir):
    
    if use_background_psf:
        return get_background_psf_profile()
    
    diffs = np.abs(allowed_zs - z)
    zi_best = np.argmin(diffs)
    
    if(bulge):
        sed = 'ell'
    else:
        diffs = np.abs(allowed_ns - n)
        ni_best = np.argmin(diffs)
        sed = seds[allowed_ns[ni_best]]
        
    return load_psf_model_from_file(sed, allowed_zs[zi_best], data_dir=data_dir)
