""" @file /disk2/brg/git/brg_library/IceBRGpy/SHE_GST_IceBRGpy/rebin.py

    Created 4 Mar 2016

    @TODO: File docstring
"""

__updated__ = "2018-07-03"

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

from copy import deepcopy
import numpy as np

try:
    import SHE_GST_cIceBRGpy as cIceBRGpy
except ImportError as _e:
    from Release import cIceBRGpy


def rebin(a, x_shift = 0, y_shift = 0, subsampling_factor = 5, conserve = False):
    """ Rebins an array with a given offset and subsampling factor. Note that
        unless 'conserve' is set to True, the input array will be overwritten.
    """

    # If we want to conserve, do so by operating on a copy of the array
    if(conserve):
        a = deepcopy(a)
    else:
        # Ensure it's contiguous
        a = np.ascontiguousarray(a)

    # Use the proper function for the data type
    if a.dtype == 'float32':
        f = cIceBRGpy.rebin_float
    elif a.dtype == 'float64':
        f = cIceBRGpy.rebin_double
    elif a.dtype == 'int32':
        f = cIceBRGpy.rebin_int
    elif a.dtype == 'int64':
        f = cIceBRGpy.rebin_long
    elif a.dtype == 'uint32':
        f = cIceBRGpy.rebin_uint
    elif a.dtype == 'uint64':
        f = cIceBRGpy.rebin_ulong
    else:
        raise Exception("Unsupported data type for rebinning: " + str(a.dtype))

    new_shape = f(a, x_shift, y_shift, subsampling_factor)

    # Resort the new array into the proper shape
    new_size = np.product(new_shape)

    rebinned_array = np.reshape(np.ravel(a)[0:new_size], new_shape)

    return rebinned_array
