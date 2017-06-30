#!/usr/bin/env python

"""
    @file gen_galsim_images.py

    Created Mar 2014

    Executable module to run image generation routine.

    ---------------------------------------------------------------------

    Copyright (C) 2014-2016 Bryan R. Gillis

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

import sys

from SHE_GST_GalaxyImageGeneration.run_from_config import run_from_config_file
from SHE_GST_GalaxyImageGeneration.utility import timing


def main(argv):

    # Check that a configuration file name was passed at command line
    if(len(argv)) <= 1:
        config_file_name = ""
    else:
        config_file_name = argv[1]

    run_from_config_file(config_file_name)

    print 'Execution complete.'


if __name__ == "__main__":
    main(sys.argv)
