""" @file measure_bias.py

    Created 7 Apr 2017

    Primary execution loop for measuring bias in shear estimates.

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
from astropy.table import Table
import galsim

from icebrgpy.logging import getLogger

from SHE_GSM_BiasMeasurement import magic_values as mv
from SHE_SIM_galaxy_image_generation import magic_values as sim_mv

def measure_bias_from_args(kwargs):
    """
    @brief
        Perform bias measurement, given arguments from the command-line.
    
    @param kwargs <dict>
    
    @return None
    """
    
    # Load input files
    input_files = get_input_files(root_dir=kwargs["input_dir"],
                                  required_input_pattern=kwargs["required_input_pattern"],
                                  depth=kwargs["depth"])
    
    # Get shear measurement and actual value arrays from the combination of all input data
    all_shear_measurements = get_all_shear_measurements(input_files)
    
    # Calculate the bias
    bias_measurements = calculate_bias(all_shear_measurements)
    
    # Output the bias measurements
    output_bias_measurements(bias_measurements=bias_measurements,
                             output_file_name=kwargs["output_file_name"])
