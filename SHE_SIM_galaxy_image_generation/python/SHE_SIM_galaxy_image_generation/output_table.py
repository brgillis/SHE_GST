""" @file output_table.py

    Created 23 Jul 2015

    Functions related to output of details tables.

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

from astropy.io import fits

from SHE_SIM_galaxy_image_generation import magic_values as mv


names_and_dtypes = (('ID', 'i8', 'K'),
            ('x_center_pix', 'f4', 'E'),
            ('y_center_pix', 'f4', 'E'),
            ('psf_x_center_pix', 'f4', 'E'),
            ('psf_y_center_pix', 'f4', 'E'),
            ('hlr_bulge_arcsec', 'f4', 'E'),
            ('hlr_disk_arcsec', 'f4', 'E'),
            ('bulge_ellipticity', 'f4', 'E'),
            ('bulge_axis_ratio', 'f4', 'E'),
            ('bulge_fraction', 'f4', 'E'),
            ('magnitude', 'f4', 'E'),
            ('sersic_index', 'f4', 'E'),
            ('rotation', 'f4', 'E'),
            ('spin', 'f4', 'E'),
            ('tilt', 'f4', 'E'),
            ('shear_magnitude', 'f4', 'E'),
            ('shear_angle', 'f4', 'E'),
            ('is_target_galaxy', 'b1', 'L'))

def get_names():
    """ Get the column names for the details table.

        Requires: (nothing)

        Return: <list of strings>
    """

    return zip(*names_and_dtypes)[0]

def get_dtypes():
    """ Get the data types for the details table, in the format for an astropy table.

        Requires: (nothing)

        Return: <list of strings>
    """

    return zip(*names_and_dtypes)[1]

def get_fits_dtypes():
    """ Get the data types for the details table, in the format for a fits table

        Requires: (nothing)

        Return: <list of strings>
    """

    return zip(*names_and_dtypes)[2]

def size():
    """ Get the number of columns for the details table.

        Requires: (nothing)

        Return: <int>
    """

    return len(names_and_dtypes)

def add_row(table, **kwargs):
    """ Add a row to a table by packing the keyword arguments and passing them as a
        dict to its 'vals' keyword argument.

        Requires: table <astropy.tables.Table> (table to add the row to)
                  **kwargs <...> (one or more keyword arguments corresponding to columns
                                  in the table)

        Returns: (nothing)

        Side-effects: Row is appended to end of table.
    """


    table.add_row(vals=kwargs)
    return

def make_otable_header(subtracted_sky_level,
                       unsubtracted_sky_level,
                       read_noise,
                       gain,):
    header = {}
    header["S_SKYLV"] = subtracted_sky_level
    header["US_SKYLV"] = unsubtracted_sky_level
    header["RD_NOISE"] = read_noise
    header["CCDGAIN"] = gain
    
    return header

def output_as_fits(table, filename, header=None):
    """ Output an astropy table as a fits binary table.

        Requires: table <astropy.tables.Table> (table to be output)
                  filename <string> (Name of file to output this table to)

        Returns: (nothing)

        Side-effects: Overwrites the file at 'filename'

    """
    fits_cols = []
    for name, _, my_format in names_and_dtypes:
        fits_cols.append(fits.Column(name=name, format=my_format, array=table[name]))

    # Set up the binary extension HDU with the correct data and filenames
    my_bin_hdu = fits.BinTableHDU.from_columns(fits_cols)

    my_bin_hdu.header[mv.version_label] = mv.version_str

    # Output it to the desired filename
    my_bin_hdu.writeto(filename, clobber=True)

    return

def output_details_tables(otable, file_name_base, options):

    if ((options['details_output_format'] == 'ascii') or (options['details_output_format'] == 'both')):
        text_file_name = file_name_base + mv.ascii_details_file_tail
        otable.write(text_file_name, format='ascii.ecsv')
        # Allow group access to it
        cmd = 'chmod g+rw ' + text_file_name
        subprocess.call(cmd, shell=True)

    if ((options['details_output_format'] == 'fits') or (options['details_output_format'] == 'both')):
        fits_file_name = file_name_base + mv.fits_details_file_tail
        otable.write(fits_file_name, format='fits', overwrite=True)
        # Allow group access to it
        cmd = 'chmod g+rw ' + fits_file_name
        subprocess.call(cmd, shell=True)

    return
