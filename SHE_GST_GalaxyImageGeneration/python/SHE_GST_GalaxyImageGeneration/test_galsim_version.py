""" @file test_galsim_version.py

    Created 23 Jul 2015

    Contains a function to test if an early version of GalSim is used, to
    determine which interface should be used when calling it.

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

import galsim


def is_early_galsim_version():
    """ Tests if the installed version of galsim is 1.0.x or earlier.

        Requires: (nothing)

        Returns: <bool> (True if galsim.version is 1.0.x or lower, otherwise False)
    """

    # Try to check for a version tag first
    if(hasattr(galsim, 'version')):
        ver = galsim.version
    elif(hasattr(galsim, '__version__')):
        ver = galsim.__version__
    else:
        # No version tag. So, check for the presence of the 'wcs' attribute, which
        # was added in version 1.1
        if(hasattr(galsim, 'wcs')):
            return False
        else:
            return True

    # Continuing the path where we did find a version tag and set it to the ver variable
    if(ver[0] == '0'): # Version 0.X.X
        return True
    if(ver[0:3] == '1.0'): # Version 1.0.X
        return True
    return False # Version 1.1.0 or higher
