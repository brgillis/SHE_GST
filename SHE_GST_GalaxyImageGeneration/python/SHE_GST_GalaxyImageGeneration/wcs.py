"""
    @file wcs.py

    Created 12 Jun 2018

    Functions related to determining a WCS for an image.
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
from SHE_GST_GalaxyImageGeneration.magic_values import image_gap_x_pix, image_gap_y_pix

def get_wcs_from_image_phl(image_phl):
    """Creates a galsim WCS from an image PHL.
    """

    im_id = image_phl.get_local_ID()
    x_i = im_id % 6
    y_i = im_id // 6

    full_x_size = int(image_phl.get_param_value("image_size_xp"))
    full_y_size = int(image_phl.get_param_value("image_size_yp"))

    pixel_scale = image_phl.get_param_value("pixel_scale")

    return get_offset_wcs(pixel_scale,
                          x_i,
                          y_i,
                          full_x_size,
                          full_y_size)

def get_offset_wcs(pixel_scale,
                   x_i,
                   y_i,
                   full_x_size,
                   full_y_size):
    """Creates a galsim Offset WCS from required information.
    """

    x_offset = x_i * (full_x_size + image_gap_x_pix)
    y_offset = y_i * (full_y_size + image_gap_y_pix)

    # TODO: Check actual arrangement of CCDs
    wcs = galsim.wcs.OffsetWCS(scale = pixel_scale, origin = -galsim.PositionD(x_offset, y_offset))

    return wcs



