""" @file unweighted_moments.py

    Created 3 Apr 2017

    Functions to estimate unweighted moments and ellipticity for a galsim
    SBProfile.

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

from SHE_GSM_ShearEstimation.estimate_shear import ShearEstimate

def calculate_unweighted_ellipticity(prof):
    """
    @brief
        Calculates the unweighted moment ellipticity of a profile by drawing it.
        
    @param prof <galsim.SBProfile> The profile to calculate the ellipticity of
    
    @return <ShearEstimate>
    """
    
    image = draw_prof(prof)
    
    shear_estimate = calculate_unweighted_ellipticity_from_image(image)
    
    return shear_estimate

def draw_prof(prof):
    """
    @brief
        Draws a profile onto a image appropriate for measuring its moments.
        
    @param prof <galsim.SBProfile> The profile to draw
    
    @return <np.ndarray> The image of the profile
    """

    # Draw the image.  Note: need a method that integrates over pixels to get flux right.
    galsim_image = prof.drawImage(method='no_pixel',dtype=float)
    
    return galsim_image.array

def calculate_unweighted_ellipticity_from_image(image):
    """
    @brief
        Calculates the unweighted moment ellipticity of an image, using the true center of it as
        the center for calculations
        
    @param image <np.ndarray> The image to calculate the ellipticity of
    
    @return <ShearEstimate>
    """
    
    shape = np.shape(image)
    
    # Note inversion of x and y due to reading it in in Fortran ordering
    yc = (shape[0] - 1) / 2.
    xc = (shape[1] - 1) / 2.
    
    indices = np.indices(shape, dtype=int)
    y_array = indices[0] - yc
    x_array = indices[1] - xc
    
    x2_array = np.square(x_array)
    y2_array = np.square(y_array)
    xy_array = x_array * y_array
    
    mxx = (x2_array * image).sum()
    myy = (y2_array * image).sum()
    mxy = (xy_array * image).sum()
    
    mxx_p_myy = mxx + myy
    
    g1 = (mxx-myy) / mxx_p_myy
    g2 = 2*mxy / mxx_p_myy
    
    return ShearEstimate(g1,g2,0)