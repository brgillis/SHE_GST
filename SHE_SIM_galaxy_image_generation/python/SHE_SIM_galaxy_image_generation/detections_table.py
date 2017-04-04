""" @file detections_table.py

    Created 4 Apr 2015

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

from SHE_SIM_galaxy_image_generation import magic_values as mv
from SHE_SIM_galaxy_image_generation.tables import get_dtypes, get_names


detections_table_names_and_dtypes = (('ID', 'i8', 'K'),
                                    ('x_center_pix', 'i8', 'K'),
                                    ('y_center_pix', 'i8', 'K'),
                                    ('psf_x_center_pix', 'f4', 'E'),
                                    ('psf_y_center_pix', 'f4', 'E'),)

def make_detections_table_header(subtracted_sky_level,
                                 unsubtracted_sky_level,
                                 read_noise,
                                 gain,):
    header = {}
    header["S_SKYLV"] = subtracted_sky_level
    header["US_SKYLV"] = unsubtracted_sky_level
    header["RD_NOISE"] = read_noise
    header["CCDGAIN"] = gain
    
    return header

def initialise_detections_table(image, options):
    
    init_cols = []
    for _ in xrange(len(detections_table_names_and_dtypes)):
        init_cols.append([])
    
    detections_table = Table(init_cols, names=get_names(detections_table_names_and_dtypes),
                          dtype=get_dtypes(detections_table_names_and_dtypes))
    detections_table.meta[mv.version_label] = mv.version_str
    detections_table.meta["S_SKYLV"] = image.get_param_value('subtracted_background'), 'ADU/arcsec^2'
    detections_table.meta["US_SKYLV"] = image.get_param_value('unsubtracted_background'), 'ADU/arcsec^2'
    detections_table.meta["RD_NOISE"] = options['read_noise'], 'e-/pixel'
    detections_table.meta["CCDGAIN"] = options['gain'], 'e-/ADU'
    
    return detections_table
