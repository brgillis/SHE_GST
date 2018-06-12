""" @file gain_test.py

    Created 12 June 2018

    Tests of functions dealing with creating a GalSim WCS.
"""

# Copyright (C) 2012-2020 Euclid Science Ground Segment
#
# This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General
# Public License as published by the Free Software Foundation; either version 3.0 of the License, or (at your option)
# any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

import pytest

import numpy as np
import galsim
from numpy.testing import assert_almost_equal

from SHE_GST_GalaxyImageGeneration.wcs import get_offset_wcs, get_wcs_from_image_phl

class TestWCS:
    """


    """
    
    @classmethod
    def setup_class(cls):
        
        cls.pixel_scale = 0.05
        cls.full_x_size = 1024
        cls.full_y_size = 1024
        
        cls.xi1 = 0
        cls.yi1 = 0
        
        cls.xi2 = 1
        cls.yi2 = 0
        
        cls.xi3 = 0
        cls.yi3 = 1
        
        cls.test_wcs1 = get_offset_wcs(pixel_scale = cls.pixel_scale,
                                       x_i = cls.xi1,
                                       y_i = cls.yi1,
                                       full_x_size = cls.full_x_size,
                                       full_y_size = cls.full_y_size)
        
        cls.test_wcs2 = get_offset_wcs(pixel_scale = cls.pixel_scale,
                                       x_i = cls.xi1,
                                       y_i = cls.yi1,
                                       full_x_size = cls.full_x_size,
                                       full_y_size = cls.full_y_size)
        
        cls.test_wcs3 = get_offset_wcs(pixel_scale = cls.pixel_scale,
                                       x_i = cls.xi1,
                                       y_i = cls.yi1,
                                       full_x_size = cls.full_x_size,
                                       full_y_size = cls.full_y_size)

        return

    @classmethod
    def teardown_class(cls):

        return
    
    def test_get_wcs_basic(self):
        
        x_step = 1000
        y_step = 1000
            
        expected_dist = np.sqrt(x_step**2+y_step**2)*self.pixel_scale
        
        # Test that each wcs behaves as expected
        for wcs in (self.test_wcs1, self.test_wcs2, self.test_wcs3):
            
            uv0 = wcs.toWorld(galsim.PositionD(0, 0))
            uv1 = wcs.toWorld(galsim.PositionD(x_step, y_step))
            
            dist = np.sqrt((uv1.x-uv0.x)**2+(uv1.x-uv0.y)**2)
            
            assert_almost_equal(expected_dist,dist)
        
        return
    
    