""" @file unweighted_moments.py

    Created 3 Apr 2017

    Functions to estimate unweighted moments and ellipticity for a galsim
    SBProfile.
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

import galsim
import numpy as np

image_size = 1023
image_scale = 0.02

def calculate_unweighted_ellipticity( prof ):
    """
    @brief
        Calculates the unweighted moment ellipticity of a profile by drawing it.
        
    @param prof <galsim.SBProfile> The profile to calculate the ellipticity of
    
    @return <ShearEstimate>
    """

    image = draw_prof( prof )

    g1, g2 = calculate_unweighted_ellipticity_from_image( image )

    return g1, g2

def draw_prof( prof ):
    """
    @brief
        Draws a profile onto a image appropriate for measuring its moments.
        
    @param prof <galsim.SBProfile> The profile to draw
    
    @return <np.ndarray> The image of the profile
    """

    galsim_image = galsim.Image( image_size, image_size, scale = image_scale )
    prof = galsim.Convolve( [prof], gsparams = galsim.GSParams( maximum_fft_size = 20000 ) )
    prof.drawImage( galsim_image, method = 'no_pixel' )

    return galsim_image.array

def get_g_from_e( e1, e2 ):
    """
    @brief
        Calculates the g-style shear from e-style
        
    @param e1
    @param e2
    
    @return g1, g2
    """

    e = np.sqrt( np.square( e1 ) + np.square( e2 ) )
    beta = np.arctan2( e2, e1 )

    r2 = ( 1. - e ) / ( 1. + e )

    r = np.sqrt( r2 )

    g = ( 1. - r ) / ( 1. + r )

    return g * np.cos( beta ), g * np.sin( beta )

def calculate_unweighted_ellipticity_from_image( image ):
    """
    @brief
        Calculates the unweighted moment ellipticity of an image, using the true center of it as
        the center for calculations
        
    @param image <np.ndarray> The image to calculate the ellipticity of
    
    @return <ShearEstimate>
    """

    shape = np.shape( image )

    # Note inversion of x and y due to reading it in in Fortran ordering
    yc = ( shape[0] - 1 ) / 2.
    xc = ( shape[1] - 1 ) / 2.

    indices = np.indices( shape, dtype = int )
    y_array = indices[0] - yc
    x_array = indices[1] - xc

    x2_array = np.square( x_array )
    y2_array = np.square( y_array )
    xy_array = x_array * y_array

    mxx = ( x2_array * image ).sum()
    myy = ( y2_array * image ).sum()
    mxy = ( xy_array * image ).sum()

    if not mxx + myy > 0:
        raise Exception( "Cannot calculate moments for image of all zeroes." )

    e1 = ( mxx - myy ) / ( mxx + myy )
    e2 = 2 * mxy / ( mxx + myy )

    g1, g2 = get_g_from_e( e1, e2 )

    return g1, g2
