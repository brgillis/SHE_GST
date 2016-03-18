""" @file dither_schemes.py

    Created 5 Oct 2015

    Pixel shifts for different dithering schemes

    ---------------------------------------------------------------------

    Copyright (C) 2015 Bryan R. Gillis

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

def get_dither_scheme(scheme_name):
    """ Gets the pixel shifts for a given dither scheme.
    
        @param scheme_name The name of the dithering scheme
        
        @return tuple of tuples of x,y shifts
    """
    
    if(scheme_name=='2x2'):
        return ((0.0,0.0),
                (0.5,0.0),
                (0.0,0.5),
                (0.5,0.5))
    else:
        return ((0.0,0.0),)