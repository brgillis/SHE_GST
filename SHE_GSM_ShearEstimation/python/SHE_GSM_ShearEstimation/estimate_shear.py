""" @file estimate_shear.py

    Created 27 Mar 2017

    Provides functions to measure the shape of a galaxy image.

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

from SHE_GSM_ShearEstimation import magic_values as mv

class ShearEstimate(object):
    def __init__(self, g1, g2, gerr=None):
        self.g1 = g1
        self.g2 = g2
        self.gerr = gerr
        
def get_resampled_image(subsampled_psf_image, scale):
    
    resampled_nx = int(np.shape(subsampled_psf_image.array)[0] / (scale/subsampled_psf_image.scale))
    resampled_ny = int(np.shape(subsampled_psf_image.array)[1] / (scale/subsampled_psf_image.scale))
    
    resampled_image = galsim.Image(resampled_nx,resampled_ny)
    
    galsim.InterpolatedImage(subsampled_psf_image).drawImage(resampled_image)
    
    return resampled_image

def estimate_shear(method,*args,**kwargs):
    """
    @brief
        Determines which method to use for measuring shape and calls that
    
    @param method <string> Name of the method to call
    
    @param args <list> List of arguments to pass on
    
    @param kwargs <dict> Dictionary of keyword arguments to pass on
    
    @return <measurement> Shape measurement object
    """
    
    # Check the method and call the appropriate function
    if method == 'KSB':
        return estimate_shear_KSB(*args,**kwargs)
    else:
        raise Exception("Invalid shear estimation method: " + str(method))
    
def estimate_shear_KSB(galaxy_image, psf_image):

    logger = getLogger(mv.logger_name)
    
    logger.debug("Entering estimate_shear_KSB")
    
    resampled_psf_image = get_resampled_image(psf_image, galaxy_image.scale)
    
    try:
        galsim_shear_estimate = galsim.hsm.EstimateShear(gal_image=galaxy_image,
                                                         PSF_image=resampled_psf_image,
                                                         sky_var=100.,
                                                         guess_sig_gal=0.5/galaxy_image.scale,
                                                         guess_sig_PSF=0.2/resampled_psf_image.scale,
                                                         shear_est='KSB')
        
        shear_estimate = ShearEstimate(galsim_shear_estimate.corrected_g1,
                                       galsim_shear_estimate.corrected_g2,
                                       galsim_shear_estimate.corrected_shape_err,)
    except RuntimeError as e:
        
        if("HSM Error" not in str(e)):
            raise
        
        logger.info(str(e))
        
        shear_estimate = ShearEstimate(0, 0, 1e99)
        
    
    logger.debug("Exiting estimate_shear_KSB")
    
    return shear_estimate
    