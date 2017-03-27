""" @file extract_stamps.py

    Created 27 Mar 2017

    Function to extract galaxy stamps from an image.

    ---------------------------------------------------------------------

    Copyright (C) 2017 Bryan R. Gillis

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

import galsim

class Stamp(object):
    pass

def extract_stamps(galaxies_hdulist):
    
    galaxies_hdu = galaxies_hdulist[0]
    
    # Get the stamp size from the header
    stamp_size_px = galaxies_hdu.header["STAMP_PX"]
    
    # Get the image shape
    im_nx = galaxies_hdu.header["NAXIS1"]
    im_ny = galaxies_hdu.header["NAXIS2"]
    
    # Get the number of stamps in each dimension and check that the shape is
    # evenly divisible by the stamp size
    n_stamp_x = im_nx // stamp_size_px
    n_stamp_y = im_ny // stamp_size_px
    
    if (n_stamp_x*stamp_size_px != im_nx) or (n_stamp_y*stamp_size_px != im_ny):
        raise Exception("Bad stamp or image size.")
    
    image = galsim.fits.read(hdu_list=galaxies_hdulist)
    
    stamps = []
    for ix in range(n_stamp_x):
        for iy in range(n_stamp_y):
            
            # Set up bounds for the stamp
            stamp_bounds = galsim.BoundsI(xmin=1+stamp_size_px*ix,
                                          xmax=stamp_size_px*(ix+1),
                                          ymin=1+stamp_size_px*iy,
                                          ymax=stamp_size_px*(iy+1),)
            
            # Get a subimage from these bounds
            galaxy_image = image.subImage(stamp_bounds)
            
            # Set this up, along with its center, as an object to output
            stamp = Stamp()
            stamp.image = galaxy_image
            stamp.center = stamp_bounds.center()
            
            stamps.append(stamp)
            
    return stamps
    