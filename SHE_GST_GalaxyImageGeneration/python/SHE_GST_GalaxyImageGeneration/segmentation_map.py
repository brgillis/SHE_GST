""" @file segmentation_map.py

    Created 5 Oct 2017

    Functions to generate mock segmentation maps.
"""

__updated__ = "2020-11-12"

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

from SHE_PPT.table_formats.mer_final_catalog import tf as detf
import galsim

import numpy as np


def get_seg_ID():

    seg_ID = 1

    while True:
        yield seg_ID
        seg_ID += 1


seg_ID_gen = get_seg_ID()


def make_segmentation_map(noisefree_image,
                          detections_table,
                          wcs,
                          options,
                          threshold=0,
                          r_max_factor=5,):
    """
        @TODO Docstring
    """

    if detf.hlr not in detections_table.columns:
        raise ValueError(detf.hlr + " must be in detections table for make_segmentation_map")

    if detf.FLUX_VIS_APER in detections_table.columns:
        sorted_dtc_table = deepcopy(detections_table)
        sorted_dtc_table[detf.FLUX_VIS_APER] *= -1  # Sort in descending flux
        loop_counter = 0
        while len(sorted_dtc_table.indices) > 0:
            for index in sorted_dtc_table.indices:
                sorted_dtc_table.remove_indices(index.columns[0].name)  # Remove the index to allow for safe sorting
            loop_counter += 1
            if loop_counter > 1000:
                raise Exception("Cannot remove all indices from sorted_dtc_table object to allow sorting.")
        sorted_dtc_table.sort(detf.FLUX_VIS_APER)
        sorted_dtc_table[detf.FLUX_VIS_APER] *= -1
    else:
        raise ValueError(detf.FLUX_VIS_APER + " must be in detections table for make_segmentation_map")

    detections_table.add_index(detf.ID)

    segmentation_map = galsim.Image(np.zeros_like(noisefree_image.array, dtype=np.int32), wcs=noisefree_image.wcs)

    # We'll use special speedups for stamps mode, since we know overlaps are impossible with it
    stamps_mode = options['mode'] == 'stamps'

    full_noisefree_image = noisefree_image
    full_segmentation_map = segmentation_map
    if not stamps_mode:
        noisefree_image = full_noisefree_image
        segmentation_map = full_segmentation_map
        y_image, x_image = np.indices(np.shape(noisefree_image.array))
        threshold_mask = np.ravel(noisefree_image.array) <= threshold
        claimed_mask = np.zeros_like(threshold_mask, dtype=bool)

    scale, _, _, _ = noisefree_image.wcs.jacobian().getDecomposition()

    # Convert the scale to arcsec
    scale *= 3600

    r_max_factor_scaled = r_max_factor / scale

    for i in range(len(sorted_dtc_table)):

        # Get the seg_ID from the table if it's already been set
        table_seg_ID = detections_table.loc[sorted_dtc_table[detf.ID][i]][detf.seg_ID]
        if table_seg_ID > 0:
            seg_ID = table_seg_ID
        else:
            # Not set, so generate a new one
            seg_ID = next(seg_ID_gen)

        # For each object, look for pixels near it above the threshold value
        gal_xy = wcs.toImage(galsim.PositionD(float(sorted_dtc_table[detf.gal_x_world][i]),
                                              float(sorted_dtc_table[detf.gal_y_world][i])))

        if stamps_mode:
            # In stamps mode, just work with a cutout image
            stamp_size_pix = options['stamp_size']

            xl = int(1 + gal_xy.x - stamp_size_pix / 2)
            xh = int(gal_xy.x + stamp_size_pix / 2)
            yl = int(1 + gal_xy.y - stamp_size_pix / 2)
            yh = int(gal_xy.y + stamp_size_pix / 2)

            # Check if the stamp crosses an edge and adjust as necessary
            full_x_size = full_noisefree_image.array.shape[1]
            full_y_size = full_noisefree_image.array.shape[0]
            if xl < 1:
                x_shift = 1 - xl
            elif xh > full_x_size:
                x_shift = full_x_size - xh
            else:
                x_shift = 0
            xh += x_shift
            xl += x_shift

            if yl < 1:
                y_shift = 1 - yl
            elif yh > full_y_size:
                y_shift = full_y_size - yh
            else:
                y_shift = 0
            yh += y_shift
            yl += y_shift

            stamp_bounds = galsim.BoundsI(xmin=xl, xmax=xh, ymin=yl, ymax=yh)

            noisefree_image = full_noisefree_image.subImage(stamp_bounds)
            segmentation_map = full_segmentation_map.subImage(stamp_bounds)

            y_image, x_image = np.indices(np.shape(noisefree_image.array))
            threshold_mask = np.ravel(noisefree_image.array) <= threshold
            claimed_mask = np.zeros_like(threshold_mask, dtype=bool)
        else:
            xl = 1
            yl = 1

        dx_image = x_image - (gal_xy.x - xl + 1)  # Need to correct for potential stamp shift of xp_l-1
        dy_image = y_image - (gal_xy.y - yl + 1)

        r2_image = dx_image ** 2 + dy_image ** 2

        r2_max = r_max_factor_scaled**2 * sorted_dtc_table[detf.hlr][i]**2

        region_mask = np.ravel(r2_image) > r2_max

        claimed_threshold_mask = np.logical_or(threshold_mask, claimed_mask)

        full_mask = np.logical_or(region_mask, claimed_threshold_mask)

        # Set the unmasked values to the object's seg_ID
        if not stamps_mode:
            segmentation_map.array.ravel()[~full_mask] = seg_ID
        else:
            # In stamps mode, ravel will create a copy, so we need to handle that
            ravelled_map = segmentation_map.array.ravel()
            ravelled_map[~full_mask] = seg_ID
            new_map = ravelled_map.reshape(segmentation_map.array.shape)
            segmentation_map.array[:] += new_map

        # Store this seg_ID in the table
        detections_table.loc[sorted_dtc_table[detf.ID][i]][detf.seg_ID] = seg_ID

        # Add those values to the claimed mask
        if not stamps_mode:
            claimed_mask = np.logical_or(claimed_mask, ~full_mask)

    return full_segmentation_map
