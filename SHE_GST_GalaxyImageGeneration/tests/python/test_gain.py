""" @file test_gain.py

    Created 6 July 2017

    Tests of functions dealing with gain calculations.

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

import unittest
from numpy.testing import assert_almost_equal
from SHE_GST_GalaxyImageGeneration.gain import get_ADU_from_count, get_count_from_ADU

class GainTestCase(unittest.TestCase):
    
    def setUp(self):
        self.count = 1000
        self.gain = 2.5
        self.ADU = 400

    def test_get_ADU_from_count(self):
        check_ADU = get_ADU_from_count(self.count,self.gain)
        assert_almost_equal(check_ADU, self.ADU)

    def test_get_count_from_ADU(self):
        check_count = get_count_from_ADU(self.ADU,self.gain)
        assert_almost_equal(check_count, self.count)
        
