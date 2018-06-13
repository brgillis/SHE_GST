""" @file wcs_test.py

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
from SHE_GST_GalaxyImageGeneration.magic_values import image_gap_x_pix, image_gap_y_pix

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
                                       x_i = cls.xi2,
                                       y_i = cls.yi2,
                                       full_x_size = cls.full_x_size,
                                       full_y_size = cls.full_y_size)

        cls.test_wcs3 = get_offset_wcs(pixel_scale = cls.pixel_scale,
                                       x_i = cls.xi3,
                                       y_i = cls.yi3,
                                       full_x_size = cls.full_x_size,
                                       full_y_size = cls.full_y_size)

        return

    @classmethod
    def teardown_class(cls):

        return

    def test_get_wcs_basic(self):

        x_step = 1000
        y_step = 1000

        expected_dist = np.sqrt(x_step ** 2 + y_step ** 2) * self.pixel_scale

        # Test that each wcs behaves as expected
        for wcs in (self.test_wcs1, self.test_wcs2, self.test_wcs3):

            uv0 = wcs.toWorld(galsim.PositionD(0, 0))
            uv1 = wcs.toWorld(galsim.PositionD(x_step, y_step))

            dist = np.sqrt((uv1.x - uv0.x) ** 2 + (uv1.y - uv0.y) ** 2)

            assert_almost_equal(expected_dist, dist)

        return

    def test_wcs_difference(self):

        # Set up coordinates of the corners
        c00 = galsim.PositionD(0, 0)
        c10 = galsim.PositionD(self.full_x_size, 0)
        c01 = galsim.PositionD(0, self.full_y_size)
        c11 = galsim.PositionD(self.full_x_size, self.full_y_size)

        # Get transformed coordinates of each corner for each wcs
        wcs1_trans = []
        wcs2_trans = []
        wcs3_trans = []

        for wcs, trans in ((self.test_wcs1, wcs1_trans),
                           (self.test_wcs2, wcs2_trans),
                           (self.test_wcs3, wcs3_trans),):

            trans.append(wcs.toWorld(c00))
            trans.append(wcs.toWorld(c10))
            trans.append(wcs.toWorld(c01))
            trans.append(wcs.toWorld(c11))

        # Check that the gaps are correct

        assert_almost_equal(wcs2_trans[0].x - wcs1_trans[1].x , image_gap_x_pix * self.pixel_scale)
        assert_almost_equal(wcs2_trans[2].x - wcs1_trans[3].x , image_gap_x_pix * self.pixel_scale)

        assert_almost_equal(wcs3_trans[0].y - wcs1_trans[2].y , image_gap_y_pix * self.pixel_scale)
        assert_almost_equal(wcs3_trans[1].y - wcs1_trans[3].y , image_gap_y_pix * self.pixel_scale)

        return
