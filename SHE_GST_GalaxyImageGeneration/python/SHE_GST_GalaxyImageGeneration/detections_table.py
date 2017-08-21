""" @file detections_table.py

    Created 4 Apr 2015

    Functions related to output of details tables.

    ---------------------------------------------------------------------

    Copyright (C) 2012-2020 Euclid Science Ground Segment      
       
    This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General    
    Public License as published by the Free Software Foundation; either version 3.0 of the License, or (at your option)    
    any later version.    
       
    This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied    
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more    
    details.    
       
    You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to    
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
"""

from astropy.table import Table

from SHE_GST_GalaxyImageGeneration import magic_values as mv
from SHE_GST_GalaxyImageGeneration.tables import get_dtypes, get_names


detections_table_names_and_dtypes = ((mv.detections_table_ID_label, 'i8', 'K'),
                                    (mv.detections_table_gal_xc_label, 'i8', 'K'),
                                    (mv.detections_table_gal_yc_label, 'i8', 'K'),
                                    (mv.detections_table_psf_xc_label, 'f4', 'E'),
                                    (mv.detections_table_psf_yc_label, 'f4', 'E'),)

def make_detections_table_header(subtracted_sky_level,
                                 unsubtracted_sky_level,
                                 read_noise,
                                 gain,):
    header = {}
    header[mv.fits_header_subtracted_sky_level_label] = subtracted_sky_level
    header[mv.fits_header_unsubtracted_sky_level_label] = unsubtracted_sky_level
    header[mv.fits_header_read_noise_label] = read_noise
    header[mv.fits_header_gain_label] = gain
    
    return header

def initialise_detections_table(image, options):
    
    init_cols = []
    for _ in xrange(len(detections_table_names_and_dtypes)):
        init_cols.append([])
    
    detections_table = Table(init_cols, names=get_names(detections_table_names_and_dtypes),
                          dtype=get_dtypes(detections_table_names_and_dtypes))
    detections_table.meta[mv.version_label] = mv.version_str
    detections_table.meta[mv.fits_header_subtracted_sky_level_label] = image.get_param_value('subtracted_background'), 'ADU/arcsec^2'
    detections_table.meta[mv.fits_header_unsubtracted_sky_level_label] = image.get_param_value('unsubtracted_background'), 'ADU/arcsec^2'
    detections_table.meta[mv.fits_header_read_noise_label] = options['read_noise'], 'e-/pixel'
    detections_table.meta[mv.fits_header_gain_label] = options['gain'], 'e-/ADU'
    
    return detections_table
