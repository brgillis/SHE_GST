""" @file compress_image.py

    Created 23 Jul 2015

    Contains a function to compress a fits image with fpack.
"""

__updated__ = "2021-08-30"

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

import subprocess

from . import magic_values as mv


def compress_image(image_name, nx=None, lossy=False):
    """ Compresses an image using fpack.

        Requires: image_name <string>

        Optional: nx <int> (number of pixels per tile, assuming square tiles)
                  lossy <bool> (whether or not to allow lossy compression)

    """

    cmd = mv.rm_command + image_name + ".fz"
    subprocess.call(cmd, shell=True)

    if lossy:
        cmd = mv.fpack_lossy_command + image_name
    else:
        if nx is None:
            cmd = mv.fpack_lossless_command + image_name
        else:
            cmd = mv.fpack_lossless_command + "-t " + str(nx[0]) + "," + str(nx[1]) + " " + image_name

    subprocess.call(cmd, shell=True)
    cmd = mv.rm_command + image_name
    subprocess.call(cmd, shell=True)
