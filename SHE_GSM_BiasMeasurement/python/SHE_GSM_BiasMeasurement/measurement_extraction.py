""" @file measurement_extraction.py

    Created 10 Apr 2017

    Functions to get shear measurements and actual values from tables.

    ---------------------------------------------------------------------

    Copyright (C) 2017 Bryan R. Gillis

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

from astropy.table import Table, join, vstack
import numpy as np

from SHE_SIM_galaxy_image_generation import magic_values as sim_mv
from SHE_GSM_ShearEstimation import magic_values as est_mv

from SHE_GSM_BiasMeasurement import magic_values as mv

def get_all_shear_measurements(input_files):
    """
    @brief
        Extract shear measurements and actual values from the input files.
    @param input_files
        <list<tuple>> List of tuples of detections and details files.
    @return <astropy.table.Table>
        Table of shear measurements.
    """
    
    joined_tables = []
    
    for measurements_table_filename, details_table_filename in input_files:
        
        # Load each table
        measurements_table = Table.read(measurements_table_filename)
        details_table = Table.read(details_table_filename)
        
        # Join the tables
        joined_table = join(measurements_table,
                            details_table,
                            keys=sim_mv.detections_table_ID_label)
        
        # Check that the tables were joined properly
        if len(joined_table) != len(measurements_table):
            raise Exception(measurements_table_filename + " could not be joined to " + details_table_filename + ".")
        
        # Trim the joined table to only have the needed columns
        
        colnames = joined_table.colnames
        for colname in colnames:
            if colname not in [sim_mv.detections_table_ID_label,
                               est_mv.fits_table_gal_g1_label,
                               est_mv.fits_table_gal_g2_label,
                               est_mv.fits_table_gal_gerr_label,
                               mv.details_table_g_label,
                               mv.details_table_beta_label,]:
                joined_table.remove_column(colname)
                
        joined_table[mv.fits_table_sim_g1_label] = ( joined_table[mv.details_table_g_label] *
                                                     np.cos(np.pi/90.*joined_table[mv.details_table_beta_label]) )
        joined_table[mv.fits_table_sim_g2_label] = ( joined_table[mv.details_table_g_label] *
                                                     np.sin(np.pi/90.*joined_table[mv.details_table_beta_label]) )
        
        joined_table.remove_column(mv.details_table_g_label)
        joined_table.remove_column(mv.details_table_beta_label)
        
        # Rename columns as necessary
        def rename_column(old_name, new_name):
            if old_name != new_name:
                joined_table.rename_column(old_name, new_name)
                
        rename_column(sim_mv.detections_table_ID_label,    mv.fits_table_ID_label)
        rename_column(est_mv.fits_table_gal_g1_label,      mv.fits_table_est_g1_label)
        rename_column(est_mv.fits_table_gal_g2_label,      mv.fits_table_est_g2_label)
        rename_column(est_mv.fits_table_gal_gerr_label,    mv.fits_table_est_gerr_label)
        
        joined_tables.append(joined_table)
        
    # Append to the comparisons table
    comparison_table = vstack(joined_tables)
    
    return comparison_table