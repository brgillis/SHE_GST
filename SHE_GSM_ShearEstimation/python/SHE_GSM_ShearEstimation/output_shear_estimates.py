""" @file output_shear_estimates.py

    Created 27 Mar 2017

    Function to output shear estimates.

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

from astropy.table import Table
from SHE_GSM_ShearEstimation import magic_values as mv

def output_shear_estimates(stamps,args):
    """
    @brief
        Outputs shear estimates into the desired fits table.
        
    @param stamps <list> List of stamp objects containing shear estimates
    
    @param args Parsed command-line arguments
    
    @return None
    """
    
    # Get the desired output filename
    if args.output_file_name is not None:
        output_file_name = args.output_file_name
    else:
        # Replace the image tail with output tail
        output_file_name = args.galaxies_image_file_name.replace(mv.image_tail,mv.output_tail)
        
        # If that failed, make sure we don't overwrite the image
        if output_file_name == args.galaxies_image_file_name:
            output_file_name = args.galaxies_image_file_name + mv.output_tail
    
    # Initialize a table for output        
    otable = Table(names=["GAL_X1","GAL_X2","GAL_G1","GAL_G2","GAL_GERR",],
                   dtype=[   float,   float,   float,   float,     float,])
    
    # Add each stamp's data to it in turn
    for stamp in stamps:
        otable.add_row({"GAL_X1"  : stamp.center.x,
                        "GAL_X2"  : stamp.center.y,
                        "GAL_G1"  : stamp.shear_estimate.g1,
                        "GAL_G2"  : stamp.shear_estimate.g2,
                        "GAL_GERR": stamp.shear_estimate.gerr,})
            
    # Output the table
    otable.write(output_file_name,format='fits')
    
    return