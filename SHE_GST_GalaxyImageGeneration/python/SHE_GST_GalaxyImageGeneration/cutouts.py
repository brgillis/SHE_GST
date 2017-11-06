""" @file /disk2/brg/Program_Files/workspace/Generate_GalSim_Images/SHE_GST_GalaxyImageGeneration/cutouts.py

    Created 14 Mar 2016

    @TODO: File docstring
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

import galsim

from SHE_GST_GalaxyImageGeneration.galaxy import is_target_galaxy
import numpy as np

from SHE_PPT.details_table_format import details_table_format as datf
from SHE_PPT.detections_table_format import detections_table_format as detf

def make_cutout_image(image,
                      options,
                      galaxies,
                      detections_table=None,
                      details_table=None,
                      centre_offset=0):

    # Get a list of only the target galaxies
    target_galaxies = []
    for galaxy in galaxies:
        if is_target_galaxy(galaxy, options):
            target_galaxies.append(galaxy)

    # Figure out how to set up the grid, making it as square as possible
    num_target_galaxies = len(target_galaxies)

    ncols = int(np.ceil(np.sqrt(num_target_galaxies)))
    nrows = int(np.ceil(num_target_galaxies / ncols))

    stamp_size_pix = options['stamp_size']

    cutout_image_npix_x = ncols * stamp_size_pix
    cutout_image_npix_y = nrows * stamp_size_pix

    cutout_image = galsim.Image(cutout_image_npix_x,
                                cutout_image_npix_y,
                                dtype=image.dtype,
                                scale=image.scale)

    # Add each target galaxy to the cutout image

    icol = -1
    irow = 0

    full_x_size = image.xmax
    full_y_size = image.ymax

    for galaxy in target_galaxies:

        # Increment position first
        icol += 1
        if icol >= ncols:
            icol = 0
            irow += 1
            if irow >= nrows:
                raise Exception("More galaxies than expected when printing cutouts.")

        # Determine cutout's bounds
        cutout_bounds = galsim.BoundsI(icol * stamp_size_pix + 1, (icol + 1) * stamp_size_pix,
                                       irow * stamp_size_pix + 1, (irow + 1) * stamp_size_pix)

        # Determine galaxy's bounds
        xp = galaxy.get_param_value("xp")
        yp = galaxy.get_param_value("yp")

        xp_i = int(xp)
        yp_i = int(yp)

        x_sp_shift = xp - xp_i
        y_sp_shift = yp - yp_i

        # Determine boundaries
        xl = xp_i - stamp_size_pix // 2
        xh = xl + stamp_size_pix - 1
        yl = yp_i - stamp_size_pix // 2
        yh = yl + stamp_size_pix - 1

        # Check if the stamp crosses an edge and adjust as necessary
        x_shift = 0
        if xl < 1:
            x_shift = 1 - xl
        elif xh > full_x_size:
            x_shift = full_x_size - xh
        xh += x_shift
        xl += x_shift

        y_shift = 0
        if yl < 1:
            y_shift = 1 - yl
        elif yh > full_y_size:
            y_shift = full_y_size - yh
        yh += y_shift
        yl += y_shift

        gal_bounds = galsim.BoundsI(xl, xh, yl, yh)

        # Add the galaxy's stamp to the cutout image
        cutout_image[cutout_bounds] += image[gal_bounds]

        # Adjust the galaxy's x and y centre coordinates in output tables if necessary
        for (otable, tf, dtype) in ((detections_table, detf, int),
                                    (details_table, datf, float)):
            if otable is not None:
                index = (otable[tf.ID] == galaxy.get_full_ID())
                otable[tf.gal_x][index] = dtype(icol * stamp_size_pix + 1 + stamp_size_pix // 2 - x_shift +
                    x_sp_shift + centre_offset)
                otable[tf.gal_y][index] = dtype(irow * stamp_size_pix + 1 + stamp_size_pix // 2 - y_shift + \
                    y_sp_shift + centre_offset)

    return cutout_image
