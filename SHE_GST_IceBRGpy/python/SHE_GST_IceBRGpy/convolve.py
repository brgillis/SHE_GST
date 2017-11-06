""" @file /disk2/brg/git/brg_library/IceBRGpy/SHE_GST_IceBRGpy/convolve.py

    Created 8 Mar 2016

    Methods to convolve images using pyfftw.
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

from __future__ import division
import numpy as np

import pyfftw

def fftw_convolve(im1, im2, norm=True):

    im1 = pyfftw.byte_align(im1)
    im2 = pyfftw.byte_align(im2)

    if norm:
        init_norm = im1.sum() * im2.sum()

    # First, put the images onto an appropriately-size zero-padded grid

    # Determine the desired size of the padded images

    im1_shape = np.shape(im1)
    im2_shape = np.shape(im2)

    output_shape = np.add(im1_shape, im2_shape)

    p = np.asarray(np.ceil(np.log2(output_shape)), dtype='int')

    padded_shape = np.power(2, p)

    # Set up the images on arrays

    im1_padded = pyfftw.zeros_aligned(padded_shape, np.float32)
    im2_padded = pyfftw.zeros_aligned(padded_shape, np.float32)

    im1_transformed = pyfftw.empty_aligned((padded_shape[0], padded_shape[1] // 2 + 1), np.complex64)
    im2_transformed = pyfftw.empty_aligned((padded_shape[0], padded_shape[1] // 2 + 1), np.complex64)

    im_convolved = pyfftw.empty_aligned(padded_shape, np.float32)

    # Set up transform plans
    transform_1 = pyfftw.FFTW(im1_padded, im1_transformed, axes=(0, 1))
    transform_2 = pyfftw.FFTW(im2_padded, im2_transformed, axes=(0, 1))
    inv_transform = pyfftw.FFTW(im1_transformed, im_convolved, axes=(0, 1),
                                 direction='FFTW_BACKWARD')

    center = padded_shape - (padded_shape + 1) // 2
    im1_padded[[slice(center[0] - im1_shape[0] // 2, center[0] + (im1_shape[0] + 1) // 2),
                slice(center[1] - im1_shape[1] // 2, center[1] + (im1_shape[1] + 1) // 2)]] = im1
    im2_padded[[slice(center[0] - im2_shape[0] // 2, center[0] + (im2_shape[0] + 1) // 2),
                slice(center[1] - im2_shape[1] // 2, center[1] + (im2_shape[1] + 1) // 2)]] = im2

    # Shift im2 for proper alignment
    im2_padded = pyfftw.interfaces.scipy_fftpack.ifftshift(im2_padded)

    # Call the transforms

    transform_1()
    transform_2()

    # Multiply to convolve

    im1_transformed *= im2_transformed

    # Call the inverse transform

    inv_transform()

    # Shift to proper binning
    im_convolved = pyfftw.interfaces.scipy_fftpack.fftshift(im_convolved)

    # Normalize
    if norm:
        s = im_convolved.sum()
        if s != 0:
            im_convolved *= init_norm / s
        else:
            pass

    return im_convolved[(padded_shape[0] - output_shape[0]) / 2:(padded_shape[0] - output_shape[0]) / 2 + output_shape[0],
                        (padded_shape[1] - output_shape[1]) / 2:(padded_shape[1] - output_shape[1]) / 2 + output_shape[1]]
