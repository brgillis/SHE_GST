""" @file compress_image.py

    Created 23 Jul 2015

    Contains a function to compress a fits image with fpack.

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

import subprocess

import SHE_GST_GalaxyImageGeneration.magic_values as mv


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
