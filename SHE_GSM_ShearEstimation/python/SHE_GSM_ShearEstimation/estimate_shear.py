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

import galsim

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
    
    galsim_shear_estimate = galsim.hsm.EstimateShear(gal_image=galaxy_image, psf_image=psf_image,
                                                     sky_var=0., shear_est='KSB')
    
    shear_estimate = object()
    shear_estimate.g1 = galsim_shear_estimate.corrected_g1
    shear_estimate.g2 = galsim_shear_estimate.corrected_g2
    shear_estimate.gerr = galsim_shear_estimate.corrected_shape_err
    
    return shear_estimate
    