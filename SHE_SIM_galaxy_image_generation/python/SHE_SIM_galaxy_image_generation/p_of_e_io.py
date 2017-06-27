""" @file p_of_e_io.py

    Created 12 Apr 2017

    This module contains functions for input/output of p_of_e histograms.

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

from astropy.io import fits

import numpy as np


fits_table_bin_low_label = "E_LOW"
fits_table_e_count_label = "E_COUNT"
fits_table_e_samples_label = "E_SAMPLES"

def output_p_of_e(p_of_e_bins, e_samples, output_file_name, header = {}):
    """
        @brief Output a histogram of P(e) to a file.
        
        @param p_of_e_bins <ndarray<int>>
            Histogram of P(e), unnormalized, with bins spanning 0 to 1.
        
        @param output_file_name <str>
            File name to be output to.
        
        @param output_format <str>
            Format to output in.
            
        @param header <dict>
            Header values to be printed to the output table.
    """
    
    # Get the bin limits
    N_bins = len(p_of_e_bins)
    bin_lows = np.linspace(0.,1.,N_bins,endpoint=False)
    
    # Set up the HDU for the bins
    p_of_e_hdu = fits.BinTableHDU.from_columns(
                    [fits.Column(name=fits_table_bin_low_label, format='E', array=bin_lows),
                     fits.Column(name=fits_table_e_count_label, format='K', array=p_of_e_bins)])
    
    # Set up the HDU for the samples
    e_sample_hdu = fits.BinTableHDU.from_columns(
                    [fits.Column(name=fits_table_e_samples_label, format='E', array=e_samples)])
    
    primary_hdu = fits.PrimaryHDU(header=fits.Header(header))
    
    hdu_list = fits.HDUList([primary_hdu,p_of_e_hdu,e_sample_hdu])
    
    # Print it
    hdu_list.writeto(output_file_name, clobber = True)
    
def load_p_of_e(input_file_name, input_format=None):
    """
        @brief Read in a histogram of P(e) from a file
        
        @param input_file_name <str>
            File name to load P(e) from.
        
        @param input_format <str>
            Format of input file
            
        @return <astropy.table.Table>
    """
    
    p_of_e_table = Table.read(input_file_name, format=input_format)
    
    # Check it's good
    if (fits_table_bin_low_label not in p_of_e_table or
        fits_table_e_count_label not in p_of_e_table):
        raise Exception("Table in file " + input_file_name + " does not have expected P(e) columns.")
    
    return p_of_e_table