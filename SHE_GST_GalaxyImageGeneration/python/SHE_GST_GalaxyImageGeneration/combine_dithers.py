"""
    @file combine_dithers.py

    Created 6 Oct 2015

    Function to combine various dithers into a stacked image.
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

import os
import galsim
from numpy.lib.stride_tricks import as_strided

import numpy as np
from SHE_GST_GalaxyImageGeneration import magic_values as mv
from SHE_PPT.file_io import read_listfile, read_xml_product, get_allowed_filename, write_xml_product, append_hdu
from SHE_PPT import products
from SHE_PPT import magic_values as ppt_mv
from astropy.io import fits

products.mosaic.init()
products.stack_mosaic.init()


def combine_dithers(dithers,
                    dithering_scheme,
                    mode="SUM"):
    """
        @brief Combine the dithered images in a list, according to the specific plan for a
            given dithering scheme.

        @param dithers List of galsim Image objects of the same size/shape/dtype.
        @param dithering_scheme String representing the name of the dithering scheme
        @param mode How to combine dithers - SUM, MEAN, or MAX

        @returns Combined image
        @returns Modified output table
    """
    
    if mode not in ("SUM", "NOISE_SUM", "MEAN", "MAX"):
        raise ValueError("Invalid combine mode for combine_dithers: " + str(mode) + ". " +
                         "Allowed modes are SUM, NOISE_SUM, MEAN, and MAX.")

    # Check which dithering scheme we're using
    if dithering_scheme == '2x2':

        # Check we have the right number of dithers
        num_dithers = 4
        assert len(dithers) == num_dithers

        # For this scheme, the offsets are (in x,y):
        # 0: (0.0,0.0) (Lower-left)
        # 1: (0.5,0.0) (Lower-right)
        # 2: (0.0,0.5) (Upper-left)
        # 3: (0.5,0.5) (Upper-right)

        ll_data = dithers[0]
        lr_data = dithers[1]
        ul_data = dithers[2]
        ur_data = dithers[3]

        # Initialize the combined image
        dither_shape = np.shape(ll_data)
        combined_shape = (2 * dither_shape[0], 2 * dither_shape[1])
        combined_data = np.zeros(shape = combined_shape, dtype = ll_data.dtype)

        # We'll use strides to represent each corner of the combined image
        base_strides = combined_data.strides
        dither_strides = (2 * base_strides[0], 2 * base_strides[1])

        lower_left_corners = as_strided(combined_data[:-1, :-1],
                                        shape = dither_shape,
                                        strides = dither_strides)
        lower_right_corners = as_strided(combined_data[:-1, 1:],
                                        shape = dither_shape,
                                        strides = dither_strides)
        upper_left_corners = as_strided(combined_data[1:, :-1],
                                        shape = dither_shape,
                                        strides = dither_strides)
        upper_right_corners = as_strided(combined_data[1:, 1:],
                                        shape = dither_shape,
                                        strides = dither_strides)

        # We'll combine four arrays for each corner of the dithering (remeber x-y ordering swap!)
        # We use roll here to shift by 1 pixel left/down. Since it's all initially zero, we can use +=
        # to assign the values we want to it
        if mode == "SUM" or mode == "MEAN":
            lower_left_corners += (ll_data +
                                   lr_data +
                                   ul_data +
                                   ur_data)
            lower_right_corners += (np.roll(ll_data, -1, axis = 1) +
                                    lr_data +
                                    np.roll(ul_data, -1, axis = 1) +
                                    ur_data)
            upper_left_corners += (np.roll(ll_data, -1, axis = 0) +
                                   np.roll(lr_data, -1, axis = 0) +
                                   ul_data +
                                   ur_data)
            upper_right_corners += (np.roll(np.roll(ll_data, -1, axis = 1), -1, axis = 0) +
                                    np.roll(lr_data, -1, axis = 0) +
                                    np.roll(ul_data, -1, axis = 1) +
                                    ur_data)
        elif mode == "MAX":
            
            lower_left_corners += np.max((ll_data,
                                         lr_data,
                                         ul_data,
                                         ur_data),axis=0)
            lower_right_corners += np.max((np.roll(ll_data, -1, axis = 1),
                                          lr_data,
                                          np.roll(ul_data, -1, axis = 1),
                                          ur_data),axis=0)
            upper_left_corners += np.max((np.roll(ll_data, -1, axis = 0),
                                         np.roll(lr_data, -1, axis = 0),
                                         ul_data,
                                         ur_data),axis=0)
            upper_right_corners += np.max((np.roll(np.roll(ll_data, -1, axis = 1), -1, axis = 0),
                                          np.roll(lr_data, -1, axis = 0),
                                          np.roll(ul_data, -1, axis = 1),
                                          ur_data),axis=0)
        if mode == "NOISE_SUM":
            lower_left_corners += (ll_data +
                                   lr_data +
                                   ul_data +
                                   ur_data)**2
            lower_right_corners += (np.roll(ll_data, -1, axis = 1) +
                                    lr_data +
                                    np.roll(ul_data, -1, axis = 1) +
                                    ur_data)**2
            upper_left_corners += (np.roll(ll_data, -1, axis = 0) +
                                   np.roll(lr_data, -1, axis = 0) +
                                   ul_data +
                                   ur_data)**2
            upper_right_corners += (np.roll(np.roll(ll_data, -1, axis = 1), -1, axis = 0) +
                                    np.roll(lr_data, -1, axis = 0) +
                                    np.roll(ul_data, -1, axis = 1) +
                                    ur_data)**2

        # Discard the final row and column of the combined image, which will contain junk values
        combined_data = combined_data[0:-1, 0:-1]
        
        if mode == "MEAN":
            combined_data /= num_dithers
        
    else:
        raise Exception("Unrecognized dithering scheme: " + dithering_scheme)

    return combined_data

def combine_segmentation_dithers(segmentation_listfile_name,
                                 stacked_segmentation_filename,
                                 dithering_scheme,
                                 workdir):
    
    # Get the individual dithers
    segmentation_product_filenames = read_listfile(os.path.join(workdir,
                                                                segmentation_listfile_name))
    
    num_dithers = len(segmentation_product_filenames)
    
    # Get out the fits filenames and load them in memory-mapped mode
    segmentation_dithers = []
    max_len = 0
    max_x_size = 0
    max_y_size = 0
    
    for segmentation_product_filename in segmentation_product_filenames:
        
        p = read_xml_product(os.path.join(workdir,segmentation_product_filename),allow_pickled=True)
        f = fits.open(os.path.join(workdir,p.get_data_filename()),memmap=True,mode="denywrite")
        
        if len(f) > max_len:
            max_len = len(f)
            
        x_size = -mv.image_gap_x_pix
        y_size = -mv.image_gap_y_pix
        
        for i in range(len(f)):
            shape = f[i].data.shape
            
            if i < 6:
                x_size += shape[0] + mv.image_gap_x_pix
            
            if i % 6 == 0:
                y_size += shape[1] + mv.image_gap_y_pix
        
        if x_size > max_x_size:
            max_x_size = x_size
        if y_size > max_y_size:
            max_y_size = y_size
            
        segmentation_dithers.append(f)
        
    # Create the image we'll need
    if dithering_scheme=='2x2':
        max_x_size = 2*max_x_size + 2
        max_y_size = 2*max_y_size + 2
        pixel_factor = 2
        extra_pixels = 1
    
    full_image = np.zeros((max_x_size,max_y_size),dtype=np.int32)
    
    # Loop over hdus, combining them for each dither and adding to the full image
    x_offset = 0
    y_offset = 0
    for x in range(max_len):
        dithers = []
        for i in range(num_dithers):
            if x < len(segmentation_dithers[i]):
                dithers.append(segmentation_dithers[i][x].data)
                
        detector_stack = combine_dithers(dithers,
                                         dithering_scheme,
                                         mode="MAX")
        
        full_image[x_offset:x_offset+detector_stack.shape[0],
                   y_offset:y_offset+detector_stack.shape[1]] += detector_stack
                   
        if x % 6 != 5:
            x_offset += detector_stack.shape[0] + pixel_factor*mv.image_gap_x_pix - extra_pixels
        else:
            x_offset = 0
            y_offset += detector_stack.shape[1] + pixel_factor*mv.image_gap_y_pix - extra_pixels
            
    # Print out the stacked segmentation map
    data_filename = get_allowed_filename("SEG_STACK",
                                         segmentation_dithers[0][0].header['MHASH'],
                                         extension=".fits")
    data_hdu = fits.ImageHDU(data = full_image)
    data_hdu.header[ppt_mv.model_hash_label] = segmentation_dithers[0][0].header[ppt_mv.model_hash_label]
    data_hdu.header[ppt_mv.scale_label] = segmentation_dithers[0][0].header[ppt_mv.scale_label]/2
    append_hdu(os.path.join(workdir, data_filename), data_hdu)
    
    p = products.stack_mosaic.create_dpd_she_stack_mosaic(data_filename)
    write_xml_product(p, os.path.join(workdir,stacked_segmentation_filename))
    
    return

def combine_image_dithers(*args,**kwargs):
    return
    
    
        
