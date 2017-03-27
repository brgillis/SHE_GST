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
import galsim

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
    
    # Load the galaxies image
    galaxies_hdulist = fits.open(file_name=args.galaxies_image_file_name)
    
    # Get a list of postage stamps
    stamps = extract_stamps(galaxies_hdulist=galaxies_hdulist)
    
    # Load the PSF image
    psf_image = galsim.fits.read(file_name=args.psf_image_file_name)
    
    # Estimate the shear for each stamp
    for stamp in stamps:
        shear_estimate = estimate_shear(galaxy_image=stamp.image,
                                        psf_image=psf_image,
                                        method=args.method)
        stamp.shear_estimate = shear_estimate
        
    # Output the measurements
    output_shear_estimates(stamps=stamps,args=args)
    
    return