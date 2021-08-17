#!/usr/bin/env python

"""
    @file gen_galsim_images.py

    Created Mar 2014

    Executable module to run image generation routine.
"""

__updated__ = "2021-08-17"

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

from .run_from_config import run_from_config_file


def main(argv):

    # Check that a configuration file name was passed at command line
    if(len(argv)) <= 1:
        config_file_name = ""
    else:
        config_file_name = argv[1]

    run_from_config_file(config_file_name)

    print('Execution complete.')


if __name__ == "__main__":
    main(sys.argv)
