"""
    @file logging.py

    Created 11 Apr 2016

    Methods to get a logger automatically depending on if we're in an
    Elements project or not.

    ---------------------------------------------------------------------

    Copyright (C) 2016 Bryan R. Gillis

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

try:
    import ElementsKernel.Logging as log
except ImportError as _e:
    import logging as log
    
def getLogger(name=None):
    """
        @brief Forwards a request for a logger to the proper logging module.
        
        @param name
            The name of the program whose logger we want to get
        
        @returns
            The logger
    """
    return log.getLogger(name)