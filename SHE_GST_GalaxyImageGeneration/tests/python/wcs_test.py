""" @file wcs_test.py

    Created 12 June 2018

    Tests of functions dealing with creating a GalSim WCS.
"""

__updated__ = "2018-12-13"

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

from numpy.testing import assert_almost_equal
import pytest

import galsim

from SHE_GST_GalaxyImageGeneration.magic_values import image_gap_x_pix, image_gap_y_pix
from SHE_GST_GalaxyImageGeneration.wcs import get_offset_wcs, get_wcs_from_image_phl, get_affine_wcs
import numpy as np


class TestWCS:
    """


    """

    @classmethod
    def setup_class(cls):

        # Set up basic testing data
        cls.pixel_scale = 0.05
        cls.full_x_size = 1024
        cls.full_y_size = 2048

        cls.xi00 = 0
        cls.yi00 = 0

        cls.xi10 = 1
        cls.yi10 = 0

        cls.xi01 = 0
        cls.yi01 = 1

        # Set up coordinates of the corners
        cls.c00 = galsim.PositionD(0, 0)
        cls.c10 = galsim.PositionD(cls.full_x_size, 0)
        cls.c01 = galsim.PositionD(0, cls.full_y_size)
        cls.c11 = galsim.PositionD(cls.full_x_size, cls.full_y_size)

        # Test a 30-degree rotation for easy but non-trivial maths
        cls.world2image_theta = 30
        cls.costheta = np.cos(cls.world2image_theta * np.pi / 180)
        cls.sintheta = np.sin(cls.world2image_theta * np.pi / 180)

        # Test a 10% g1 shear
        cls.g1_1 = 0.1
        cls.g2_1 = 0.

        # Test a 20% g2 shear
        cls.g1_2 = 0.
        cls.g2_1 = 0.2

        cls.offset_wcs_00 = get_offset_wcs(pixel_scale=cls.pixel_scale,
                                           x_i=cls.xi00,
                                           y_i=cls.yi00,
                                           full_x_size=cls.full_x_size,
                                           full_y_size=cls.full_y_size)

        cls.offset_wcs_10 = get_offset_wcs(pixel_scale=cls.pixel_scale,
                                           x_i=cls.xi10,
                                           y_i=cls.yi10,
                                           full_x_size=cls.full_x_size,
                                           full_y_size=cls.full_y_size)

        cls.offset_wcs_01 = get_offset_wcs(pixel_scale=cls.pixel_scale,
                                           x_i=cls.xi01,
                                           y_i=cls.yi01,
                                           full_x_size=cls.full_x_size,
                                           full_y_size=cls.full_y_size)

        cls.affine_wcs_rot_00 = get_affine_wcs(pixel_scale=cls.pixel_scale,
                                               x_i=cls.xi00,
                                               y_i=cls.yi00,
                                               full_x_size=cls.full_x_size,
                                               full_y_size=cls.full_y_size,
                                               theta=cls.world2image_theta)

        cls.affine_wcs_rot_10 = get_affine_wcs(pixel_scale=cls.pixel_scale,
                                               x_i=cls.xi10,
                                               y_i=cls.yi10,
                                               full_x_size=cls.full_x_size,
                                               full_y_size=cls.full_y_size,
                                               theta=cls.world2image_theta)

        cls.affine_wcs_rot_01 = get_affine_wcs(pixel_scale=cls.pixel_scale,
                                               x_i=cls.xi01,
                                               y_i=cls.yi01,
                                               full_x_size=cls.full_x_size,
                                               full_y_size=cls.full_y_size,
                                               g1=cls.g1_1)

        cls.affine_wcs_g1_00 = get_affine_wcs(pixel_scale=cls.pixel_scale,
                                              x_i=cls.xi00,
                                              y_i=cls.yi00,
                                              full_x_size=cls.full_x_size,
                                              full_y_size=cls.full_y_size,
                                              g1=cls.g1_1)

        cls.affine_wcs_g1_10 = get_affine_wcs(pixel_scale=cls.pixel_scale,
                                              x_i=cls.xi10,
                                              y_i=cls.yi10,
                                              full_x_size=cls.full_x_size,
                                              full_y_size=cls.full_y_size,
                                              g1=cls.g1_1)

        cls.affine_wcs_g1_01 = get_affine_wcs(pixel_scale=cls.pixel_scale,
                                              x_i=cls.xi01,
                                              y_i=cls.yi01,
                                              full_x_size=cls.full_x_size,
                                              full_y_size=cls.full_y_size,
                                              g1=cls.g1_1)

        return

    @classmethod
    def teardown_class(cls):

        return

    def test_wcs_basic(self):

        x_step = 1000
        y_step = 1000

        expected_dist = np.sqrt(x_step ** 2 + y_step ** 2) * self.pixel_scale

        # Test that each wcs behaves as expected. Don't test shear wcs here, as they don't maintain distance as simply
        for wcs in (self.offset_wcs_00, self.offset_wcs_10, self.offset_wcs_01,
                    self.affine_wcs_rot_00, self.affine_wcs_rot_10, self.affine_wcs_rot_01,):

            uv0 = wcs.toWorld(galsim.PositionD(0, 0))
            uv1 = wcs.toWorld(galsim.PositionD(x_step, y_step))

            dist = np.sqrt((uv1.x - uv0.x) ** 2 + (uv1.y - uv0.y) ** 2)

            assert_almost_equal(expected_dist, dist)

        return

    def test_wcs_difference(self):

        # Get transformed coordinates of each corner for each wcs
        offset_wcs_00_trans = []
        offset_wcs_10_trans = []
        offset_wcs_01_trans = []
        affine_wcs_rot_00_trans = []
        affine_wcs_rot_10_trans = []
        affine_wcs_rot_01_trans = []
        affine_wcs_g1_00_trans = []
        affine_wcs_g1_10_trans = []
        affine_wcs_g1_01_trans = []

        for wcs, trans in ((self.offset_wcs_00, offset_wcs_00_trans),
                           (self.offset_wcs_10, offset_wcs_10_trans),
                           (self.offset_wcs_01, offset_wcs_01_trans),
                           (self.affine_wcs_rot_00, affine_wcs_rot_00_trans),
                           (self.affine_wcs_rot_10, affine_wcs_rot_10_trans),
                           (self.affine_wcs_rot_01, affine_wcs_rot_01_trans),
                           (self.affine_wcs_g1_00, affine_wcs_g1_00_trans),
                           (self.affine_wcs_g1_10, affine_wcs_g1_10_trans),
                           (self.affine_wcs_g1_01, affine_wcs_g1_01_trans),):

            trans.append(wcs.toWorld(self.c00))
            trans.append(wcs.toWorld(self.c10))
            trans.append(wcs.toWorld(self.c01))
            trans.append(wcs.toWorld(self.c11))

        # Check that the corners are correct for the offset WCSes

        for trans in (offset_wcs_00_trans, offset_wcs_10_trans, offset_wcs_01_trans):

            assert_almost_equal(trans[1].x - trans[0].x, self.full_x_size * self.pixel_scale)
            assert_almost_equal(trans[2].x - trans[0].x, 0.)
            assert_almost_equal(trans[3].x - trans[0].x, self.full_x_size * self.pixel_scale)

            assert_almost_equal(trans[1].y - trans[0].y, 0.)
            assert_almost_equal(trans[2].y - trans[0].y, self.full_y_size * self.pixel_scale)
            assert_almost_equal(trans[3].y - trans[0].y, self.full_y_size * self.pixel_scale)

        # Check that the corners are correct for the rotation WCSes

        for trans in (affine_wcs_rot_00_trans, affine_wcs_rot_10_trans, affine_wcs_rot_01_trans):

            assert_almost_equal(trans[1].x - trans[0].x, self.costheta * self.full_x_size * self.pixel_scale)
            assert_almost_equal(trans[2].x - trans[0].x, -self.sintheta * self.full_y_size * self.pixel_scale)
            assert_almost_equal(trans[3].x - trans[0].x, self.costheta * self.full_x_size * self.pixel_scale +
                                -self.sintheta * self.full_y_size * self.pixel_scale)

            assert_almost_equal(trans[1].y - trans[0].y, self.sintheta * self.full_x_size * self.pixel_scale)
            assert_almost_equal(trans[2].y - trans[0].y, self.costheta * self.full_y_size * self.pixel_scale)
            assert_almost_equal(trans[3].y - trans[0].y, self.sintheta * self.full_x_size * self.pixel_scale +
                                self.costheta * self.full_y_size * self.pixel_scale)

        # Check that the corners are correct for the g1 WCSes

        for trans in (affine_wcs_g1_00_trans, affine_wcs_g1_10_trans, affine_wcs_g1_01_trans):

            assert_almost_equal(trans[1].x - trans[0].x, (1 + self.g1) * self.full_x_size * self.pixel_scale)
            assert_almost_equal(trans[2].x - trans[0].x, 0.)
            assert_almost_equal(trans[3].x - trans[0].x, (1 + self.g1) * self.full_x_size * self.pixel_scale)

            assert_almost_equal(trans[1].y - trans[0].y, 0.)
            assert_almost_equal(trans[2].y - trans[0].y, (1 - self.g1) * self.full_y_size * self.pixel_scale)
            assert_almost_equal(trans[3].y - trans[0].y, (1 - self.g1) * self.full_y_size * self.pixel_scale)

        # Check that the gaps are correct for the offset WCSes

        assert_almost_equal(offset_wcs_10_trans[0].x - offset_wcs_00_trans[1].x, image_gap_x_pix * self.pixel_scale)
        assert_almost_equal(offset_wcs_10_trans[2].x - offset_wcs_00_trans[3].x, image_gap_x_pix * self.pixel_scale)
        assert_almost_equal(offset_wcs_10_trans[0].y - offset_wcs_00_trans[1].y, 0.)
        assert_almost_equal(offset_wcs_10_trans[2].y - offset_wcs_00_trans[3].y, 0.)

        assert_almost_equal(offset_wcs_01_trans[0].x - offset_wcs_00_trans[2].x, 0.)
        assert_almost_equal(offset_wcs_01_trans[1].x - offset_wcs_00_trans[3].x, 0.)
        assert_almost_equal(offset_wcs_01_trans[0].y - offset_wcs_00_trans[2].y, image_gap_y_pix * self.pixel_scale)
        assert_almost_equal(offset_wcs_01_trans[1].y - offset_wcs_00_trans[3].y, image_gap_y_pix * self.pixel_scale)

        # Check that the gaps are correct for the rotation WCSes

        assert_almost_equal(affine_wcs_rot_10_trans[0].x - affine_wcs_rot_00_trans[1].x,
                            self.costheta * image_gap_x_pix * self.pixel_scale)
        assert_almost_equal(affine_wcs_rot_10_trans[2].x - affine_wcs_rot_00_trans[3].x,
                            self.costheta * image_gap_x_pix * self.pixel_scale)
        assert_almost_equal(affine_wcs_rot_10_trans[0].y - affine_wcs_rot_00_trans[1].y,
                            self.sintheta * image_gap_x_pix * self.pixel_scale)
        assert_almost_equal(affine_wcs_rot_10_trans[2].y - affine_wcs_rot_00_trans[3].y,
                            self.sintheta * image_gap_x_pix * self.pixel_scale)

        assert_almost_equal(affine_wcs_rot_01_trans[0].x - affine_wcs_rot_00_trans[2].x,
                            -self.sintheta * image_gap_y_pix * self.pixel_scale)
        assert_almost_equal(affine_wcs_rot_01_trans[1].x - affine_wcs_rot_00_trans[3].x,
                            -self.sintheta * image_gap_y_pix * self.pixel_scale)
        assert_almost_equal(affine_wcs_rot_01_trans[0].y - affine_wcs_rot_00_trans[2].y,
                            self.costheta * image_gap_y_pix * self.pixel_scale)
        assert_almost_equal(affine_wcs_rot_01_trans[1].y - affine_wcs_rot_00_trans[3].y,
                            self.costheta * image_gap_y_pix * self.pixel_scale)

        # Check that the gaps are correct for the g1 WCSes

        assert_almost_equal(affine_wcs_g1_10_trans[0].x - affine_wcs_g1_00_trans[1].x,
                            (1 + self.g1) * image_gap_x_pix * self.pixel_scale)
        assert_almost_equal(affine_wcs_g1_10_trans[2].x - affine_wcs_g1_00_trans[3].x,
                            (1 + self.g1) * image_gap_x_pix * self.pixel_scale)
        assert_almost_equal(affine_wcs_g1_10_trans[0].y - affine_wcs_g1_00_trans[1].y, 0.)
        assert_almost_equal(affine_wcs_g1_10_trans[2].y - affine_wcs_g1_00_trans[3].y, 0.)

        assert_almost_equal(affine_wcs_g1_01_trans[0].x - affine_wcs_g1_00_trans[2].x, 0.)
        assert_almost_equal(affine_wcs_g1_01_trans[1].x - affine_wcs_g1_00_trans[3].x, 0.)
        assert_almost_equal(affine_wcs_g1_01_trans[0].y - affine_wcs_g1_00_trans[2].y,
                            (1 - self.g1) * image_gap_y_pix * self.pixel_scale)
        assert_almost_equal(affine_wcs_g1_01_trans[1].y - affine_wcs_g1_00_trans[3].y,
                            (1 - self.g1) * image_gap_y_pix * self.pixel_scale)

        return
