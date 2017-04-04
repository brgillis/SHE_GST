""" @file estimate_shears.py

    Created 27 Mar 2017

    Primary execution loop for measuring galaxy shapes from an image file.

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

from SHE_GSM_ShearEstimation import magic_values as mv
from SHE_SIM_galaxy_image_generation import magic_values as sim_mv

from SHE_GSM_ShearEstimation.estimate_shear import estimate_shear
from SHE_GSM_ShearEstimation.extract_stamps import extract_stamps
from SHE_GSM_ShearEstimation.output_shear_estimates import output_shear_estimates

def estimate_shears_from_args(args):
    """
    @brief
        Perform shear estimation, given arguments from the command-line.
    
    @param args Command-line arguments object
    
    @return None
    """

    logger = getLogger(mv.logger_name)
    
    logger.debug("Entering estimate_shear_KSB")
    
    # Load the detections table
    detections_table = Table.read(args.detections_table_file_name)
    
    # Load the galaxies and psf images
    galaxies_hdulist = fits.open(args.galaxies_image_file_name)
    psf_hdulist = fits.open(args.psf_image_file_name)
    
    # Get a list of galaxy postage stamps
    gal_stamps = extract_stamps(detections_table,
                                galaxies_hdulist,
                                sim_mv.detections_table_gal_xc_label,
                                sim_mv.detections_table_gal_yc_label,)
    
    # Get a list of galaxy postage stamps
    psf_stamps = extract_stamps(detections_table,
                                psf_hdulist,
                                sim_mv.detections_table_psf_xc_label,
                                sim_mv.detections_table_psf_yc_label,)
    
    # Estimate the shear for each stamp
    for i, gal_stamp, psf_stamp in zip(range(len(gal_stamps)), gal_stamps, psf_stamps):
        if i % 10 == 0:
            logger.info("Measuring shear for galaxy " + str(i) + "/" + str(len(gal_stamps)) + ".")
        shear_estimate = estimate_shear(galaxy_image=gal_stamp.image,
                                        psf_image=psf_stamp.image,
                                        method=args.method)
        gal_stamp.shear_estimate = shear_estimate
        
    # Output the measurements
    output_shear_estimates(stamps=gal_stamps,args=args)
    
    logger.debug("Exiting estimate_shear_KSB")
    
    return