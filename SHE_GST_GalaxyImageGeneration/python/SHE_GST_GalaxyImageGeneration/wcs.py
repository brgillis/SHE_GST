"""
    @file wcs.py

    Created 12 Jun 2018

    Functions related to determining a WCS for an image.
"""

__updated__ = "2018-12-13"

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
import numpy as np


def get_wcs_from_image_phl(image_phl,
                           dither_offset=(0, 0)):
    """Creates a galsim WCS from an image PHL.
    """

    im_id = image_phl.get_local_ID()
    x_i = im_id % 6
    y_i = im_id // 6

    full_x_size = int(image_phl.get_param_value("image_size_xp"))
    full_y_size = int(image_phl.get_param_value("image_size_yp"))

    pixel_scale = image_phl.get_param_value("pixel_scale")

    wcs_g1 = image_phl.get_param_value("wcs_g1")
    wcs_g2 = image_phl.get_param_value("wcs_g1")

    wcs_theta = image_phl.get_param_value("wcs_theta")

    return get_affine_wcs(pixel_scale=pixel_scale,
                          x_i=x_i,
                          y_i=y_i,
                          full_x_size=full_x_size,
                          full_y_size=full_y_size,
                          dither_offset=dither_offset,
                          g1=wcs_g1,
                          g2=wcs_g2,
                          theta=wcs_theta)


def get_offset_wcs(pixel_scale,
                   x_i,
                   y_i,
                   full_x_size,
                   full_y_size,
                   dither_offset=(0, 0)):
    """Creates a galsim Offset WCS from required information.
    """

    x_offset = x_i * (full_x_size + image_gap_x_pix) + dither_offset[0]
    y_offset = y_i * (full_y_size + image_gap_y_pix) + dither_offset[1]

    # TODO: Check actual arrangement of CCDs
    wcs = galsim.wcs.OffsetWCS(scale=pixel_scale, origin=-galsim.PositionD(x_offset, y_offset))

    return wcs


def get_affine_wcs(pixel_scale,
                   x_i,
                   y_i,
                   full_x_size,
                   full_y_size,
                   dither_offset=(0, 0),
                   g1=0,
                   g2=0,
                   theta=0):
    """Creates a galsim Affine WCS from required information. Note that angle and shear here describe the
    image-to-world transformation.
    """

    x_offset = x_i * (full_x_size + image_gap_x_pix) + dither_offset[0]
    y_offset = y_i * (full_y_size + image_gap_y_pix) + dither_offset[1]

    theta_rad = theta * np.pi / 180.
    cos_theta = np.cos(theta_rad)
    sin_theta = np.sin(theta_rad)

    shear_matrix = np.matrix([[1 + g1, g2],
                              [g2, 1 - g1]])
    rotation_matrix = np.matrix([[cos_theta, -sin_theta],
                                 [sin_theta, cos_theta]])

    transform_matrix = pixel_scale / np.sqrt(1 - g1**2 - g2**2) * shear_matrix @ rotation_matrix

    # TODO: Check actual arrangement of CCDs
    wcs = galsim.wcs.AffineTransform(origin=-galsim.PositionD(x_offset, y_offset),
                                     dudx=transform_matrix[0, 0], dudy=transform_matrix[0, 1],
                                     dvdx=transform_matrix[1, 0], dvdy=transform_matrix[1, 1])

    return wcs
