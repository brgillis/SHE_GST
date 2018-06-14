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

import copy

import galsim
from numpy.lib.stride_tricks import as_strided

import numpy as np

from SHE_PPT.details_table_format import details_table_format as datf
from SHE_PPT.detections_table_format import detections_table_format as detf


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

        ll_data = dithers[0].array
        lr_data = dithers[1].array
        ul_data = dithers[2].array
        ur_data = dithers[3].array

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
            lower_left_corners = np.max(ll_data,
                                        lr_data,
                                        ul_data,
                                        ur_data)
            lower_right_corners = np.max(np.roll(ll_data, -1, axis = 1),
                                         lr_data,
                                         np.roll(ul_data, -1, axis = 1),
                                         ur_data)
            upper_left_corners = np.max(np.roll(ll_data, -1, axis = 0),
                                        np.roll(lr_data, -1, axis = 0),
                                        ul_data,
                                        ur_data)
            upper_right_corners = np.max(np.roll(np.roll(ll_data, -1, axis = 1), -1, axis = 0),
                                         np.roll(lr_data, -1, axis = 0),
                                         np.roll(ul_data, -1, axis = 1),
                                         ur_data)

        # Discard the final row and column of the combined image, which will contain junk values
        combined_data = combined_data[0:-1, 0:-1]
        
        if mode == "MEAN":
            combined_data /= num_dithers

        # Make a Galsim image from this data
        combined_image = galsim.Image(combined_data, scale = dithers[0].scale / 2)
        
    else:
        raise Exception("Unrecognized dithering scheme: " + dithering_scheme)

    return combined_image
