#!/usr/bin/env python

""" @file /disk2/brg/Program_Files/workspace/Generate_GalSim_Images/test_profile_generation_time.py

    Created 10 Mar 2016

    @TODO: File docstring

    ---------------------------------------------------------------------

    Copyright (C) 2016 user_name

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

import sys

import galsim
import pyfftw
import numpy as np
import cProfile

num_runs = 1000
gsparams = galsim.GSParams()

n = 32
scale = 1

center = n - (n+1)//2

def run_generation_tests():
    
    psf_image = galsim.fits.read("/home/brg/Data/test_psf.fits")
    psf_prof = galsim.InterpolatedImage(psf_image,scale=0.02)
    
    for _ in range(num_runs):
        
        gal_prof = galsim.Sersic(n=2,scale_radius=2,trunc=9,flux=1.)
        gal_prof = gal_prof.shear(g1=0.3,g2=0)
        
        convolved_image = galsim.Image(n,n,scale=scale)
        
        convolved_prof = galsim.Convolve([gal_prof,psf_prof],gsparams=gsparams)
        convolved_prof.drawImage(convolved_image,offset=(-0.5,-0.5))
        
    galsim.fits.write(convolved_image,"/home/brg/Data/test_galaxy.fits")
    
    print("Done!")
        
    return

def main(argv):
    """ @TODO main docstring
    """
    
    import cProfile
    cProfile.runctx("run_generation_tests()",{},
                    {"run_generation_tests":run_generation_tests},filename="/home/brg/Data/profile_gen.prof")

if __name__ == "__main__":
    main(sys.argv)
