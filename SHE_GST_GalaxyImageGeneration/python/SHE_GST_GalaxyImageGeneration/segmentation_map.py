""" @file segmentation_map.py

    Created 5 Oct 2017

    Functions to generate mock segmentation maps.
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

import numpy as np
from copy import deepcopy
import galsim

from SHE_PPT.table_formats.detections import tf as detf

def make_segmentation_map(noisefree_image,
                           detections_table,
                           wcs,
                           threshold = 0,
                           r_max_factor = 5):
    """
        @TODO Docstring
    """

    if detf.Isoarea not in detections_table.columns:
        raise ValueError(detf.Isoarea + " must be in detections table for make_segmentation_map")

    if detf.MagStarGal in detections_table.columns:
        sorted_dtc_table = deepcopy(detections_table)
        sorted_dtc_table.sort(detf.MagStarGal)
    else:
        raise ValueError(detf.MagStarGal + " must be in detections table for make_segmentation_map")

    segmentation_map = galsim.Image(-np.ones_like(noisefree_image.array, dtype = np.int32), scale = noisefree_image.scale)

    y_image, x_image = np.indices(np.shape(noisefree_image.array))

    threshold_mask = np.ravel(noisefree_image.array) <= threshold
    claimed_mask = np.zeros_like(threshold_mask, dtype = bool)

    r_max_factor_scaled = r_max_factor / noisefree_image.scale

    for i in range(len(sorted_dtc_table)):

        # For each object, look for pixels near it above the threshold value
        gal_xy = wcs.toImage(galsim.PositionD(float(sorted_dtc_table[detf.gal_x_world][i]),
                                              float(sorted_dtc_table[detf.gal_y_world][i])))
        
        dx_image = x_image - gal_xy.x
        dy_image = y_image - gal_xy.y

        r2_image = dx_image ** 2 + dy_image ** 2

        r2_max = r_max_factor_scaled**2 * sorted_dtc_table[detf.Isoarea][i]

        region_mask = np.ravel(r2_image) > r2_max

        claimed_threshold_mask = np.logical_or(threshold_mask, claimed_mask)

        full_mask = np.logical_or(region_mask, claimed_threshold_mask)

        # Set the unmasked values to the object's ID
        segmentation_map.array.ravel()[~full_mask] = sorted_dtc_table[detf.ID][i]

        # Add those values to the claimed mask
        claimed_mask = np.logical_or(claimed_mask, ~full_mask)

    return segmentation_map
