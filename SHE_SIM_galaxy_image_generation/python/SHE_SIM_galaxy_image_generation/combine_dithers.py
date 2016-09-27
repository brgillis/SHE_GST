"""
    @file combine_dithers.py

    Created 6 Oct 2015

    Function to combine various dithers into a stacked image.

    ---------------------------------------------------------------------

    Copyright (C) 2015, 2016 Bryan R. Gillis

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

import copy

import galsim
from numpy.lib.stride_tricks import as_strided

import numpy as np


def combine_dithers(dithers,
                    dithering_scheme,
                    output_table=None,
                    copy_otable=False):
    """
        @brief Combine the dithered images in a list, according to the specific plan for a
            given dithering scheme.

        @param dithers List of galsim Image objects of the same size/shape/dtype.
        @param dithering_scheme String representing the name of the dithering scheme
        @param output_table Output table to (possibly) modify for combined image
        @param copy_otable If False, output table will be modified in place. If True,
            it will be copied. In either case, the modified table will be returned

        @returns Combined image
        @returns Modified output table
    """

    # Set up the output table we'll modify, depending on whether or not we want to copy
    # the passed output table.
    if copy_otable:
        combined_otable = copy.deepcopy(output_table)
    else:
        combined_otable = output_table

    # Check which dithering scheme we're using
    if(dithering_scheme == '2x2'):

        # Check we have the right number of dithers
        num_dithers = 4
        assert(len(dithers) == num_dithers)

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
        combined_data = np.zeros(shape=combined_shape, dtype=ll_data.dtype)

        # We'll use strides to represent each corner of the combined image
        base_strides = combined_data.strides
        dither_strides = (2 * base_strides[0], 2 * base_strides[1])

        lower_left_corners = as_strided(combined_data[:-1, :-1],
                                        shape=dither_shape,
                                        strides=dither_strides)
        lower_right_corners = as_strided(combined_data[:-1, 1:],
                                        shape=dither_shape,
                                        strides=dither_strides)
        upper_left_corners = as_strided(combined_data[1:, :-1],
                                        shape=dither_shape,
                                        strides=dither_strides)
        upper_right_corners = as_strided(combined_data[1:, 1:],
                                        shape=dither_shape,
                                        strides=dither_strides)

        # We'll combine four arrays for each corner of the dithering (remeber x-y ordering swap!)
        # We use roll here to shift by 1 pixel left/down. Since it's all initially zero, we can use +=
        # to assign the values we want to it
        lower_left_corners += ll_data + \
                              lr_data + \
                              ul_data + \
                              ur_data
        lower_right_corners += np.roll(ll_data, -1, axis=1) + \
                               lr_data + \
                               np.roll(ul_data, -1, axis=1) + \
                               ur_data
        upper_left_corners += np.roll(ll_data, -1, axis=0) + \
                              np.roll(lr_data, -1, axis=0) + \
                              ul_data + \
                              ur_data
        upper_right_corners += np.roll(np.roll(ll_data, -1, axis=1), -1, axis=0) + \
                               np.roll(lr_data, -1, axis=0) + \
                               np.roll(ul_data, -1, axis=1) + \
                               ur_data

        # Discard the final row and column of the combined image, which will contain junk values
        combined_data = combined_data[0:-1, 0:-1]

        # Make a Galsim image from this data
        combined_image = galsim.Image(combined_data)

        # Now that we have the image, let's modify the output table
        if(combined_otable is not None):
            combined_otable['x_center_pix'] *= 2
            combined_otable['x_center_pix'] -= 0.5
            combined_otable['y_center_pix'] *= 2
            combined_otable['y_center_pix'] -= 0.5
            combined_otable['Read_noise'] *= 2

    else:
        raise Exception("Unrecognized dithering scheme: " + dithering_scheme)

    return combined_image, combined_otable
