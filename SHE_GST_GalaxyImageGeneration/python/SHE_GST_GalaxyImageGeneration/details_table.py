""" @file details_table.py

    Created 23 Jul 2015

    Functions related to output of details tables.

    ---------------------------------------------------------------------

    Copyright (C) 2015-2017 Bryan R. Gillis

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

from astropy.table import Table

from SHE_GST_GalaxyImageGeneration import magic_values as mv
from SHE_GST_GalaxyImageGeneration.tables import get_dtypes, get_names


details_table_names_and_dtypes = (('ID', 'i8', 'K'),
                                    ('x_center_pix', 'f4', 'E'),
                                    ('y_center_pix', 'f4', 'E'),
                                    ('psf_x_center_pix', 'f4', 'E'),
                                    ('psf_y_center_pix', 'f4', 'E'),
                                    ('hlr_bulge_arcsec', 'f4', 'E'),
                                    ('hlr_disk_arcsec', 'f4', 'E'),
                                    ('bulge_ellipticity', 'f4', 'E'),
                                    ('bulge_axis_ratio', 'f4', 'E'),
                                    ('bulge_fraction', 'f4', 'E'),
                                    ('disk_height_ratio', 'f4', 'E'),
                                    ('magnitude', 'f4', 'E'),
                                    ('sersic_index', 'f4', 'E'),
                                    ('rotation', 'f4', 'E'),
                                    ('spin', 'f4', 'E'),
                                    ('tilt', 'f4', 'E'),
                                    ('shear_magnitude', 'f4', 'E'),
                                    ('shear_angle', 'f4', 'E'),
                                    ('is_target_galaxy', 'b1', 'L'))

def make_details_table_header(subtracted_sky_level,
                               unsubtracted_sky_level,
                               read_noise,
                               gain,):
    header = {}
    header[mv.fits_header_subtracted_sky_level_label] = subtracted_sky_level
    header[mv.fits_header_unsubtracted_sky_level_label] = unsubtracted_sky_level
    header[mv.fits_header_read_noise_label] = read_noise
    header[mv.fits_header_gain_label] = gain
    
    return header

def initialise_details_table(image, options):
    
    init_cols = []
    for _ in xrange(len(details_table_names_and_dtypes)):
        init_cols.append([])
    
    details_table = Table(init_cols, names=get_names(details_table_names_and_dtypes),
                          dtype=get_dtypes(details_table_names_and_dtypes))
    details_table.meta[mv.version_label] = mv.version_str
    details_table.meta[mv.fits_header_subtracted_sky_level_label] = image.get_param_value('subtracted_background'), 'ADU/arcsec^2'
    details_table.meta[mv.fits_header_unsubtracted_sky_level_label] = image.get_param_value('unsubtracted_background'), 'ADU/arcsec^2'
    details_table.meta[mv.fits_header_read_noise_label] = options['read_noise'], 'e-/pixel'
    details_table.meta[mv.fits_header_gain_label] = options['gain'], 'e-/ADU'
    
    return details_table
