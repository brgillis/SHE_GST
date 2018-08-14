"""
    @file dither_schemes.py

    Created 5 Oct 2015

    Pixel shifts for different dithering schemes
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

def get_dither_scheme(scheme_name):
    """
        @brief Gets the pixel shifts for a given dither scheme.

        @param scheme_name
            <string> The name of the dithering scheme

        @return tuple<tuple<float,float>,...> x,y shifts for each dither in the scheme
    """

    if scheme_name == '2x2':
        return ((0.0, 0.0),
                (0.5, 0.0),
                (0.0, 0.5),
                (0.5, 0.5),
               )
    elif scheme_name == '4':
        return ((0.0, 0.0),
                (0.0, 0.0),
                (0.0, 0.0),
                (0.0, 0.0),
               )
    else:
        return ((0.0, 0.0),)
