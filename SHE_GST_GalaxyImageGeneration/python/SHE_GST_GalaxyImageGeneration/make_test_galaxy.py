#!/usr/bin/env python

"""
    @file make_test_galaxy.py

    Created 11 Mar 2016

    A python program used for testing of the time it takes to generate a
    galaxy image.
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

import sys

from astropy.io import fits
import galsim

import numpy as np
import pyfftw


def main(argv):
    """ @TODO main docstring
    """


    input_gal_array = fits.open("test_galaxy.fits")[0].data

    for interp in ('quintic', 'nearest'):

        input_gal = galsim.InterpolatedImage(galsim.Image(input_gal_array, scale = 0.1),
                                             x_interpolant = interp)

        output_gal_array = np.zeros((34, 34))
        output_gal = galsim.Image(output_gal_array)

        input_gal.drawImage(output_gal, scale = 0.1,
                         offset = (0.0, 0.0),
                         add_to_image = True,
                         method = 'no_pixel')

        galsim.fits.write(output_gal, "test_galaxy_" + interp + ".fits")

    return

if __name__ == "__main__":
    main(sys.argv)
