"""
    @file test_import.py

    Created 21 Aug 2017
    
    Tests of various imports, to make sure galsim is installed and other parts of the project are
    linked correctly.

    ---------------------------------------------------------------------

    Copyright (C) 2012-2020 Euclid Science Ground Segment      
       
    This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General    
    Public License as published by the Free Software Foundation; either version 3.0 of the License, or (at your option)    
    any later version.    
       
    This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied    
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more    
    details.    
       
    You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to    
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
"""

import unittest


class ImportTestCase(unittest.TestCase):

    # External imports

    def testGalSimImport(self):
        import galsim
        
    # Internal python imports

    def testSHE_GST_IceBRGpyImport(self):
        import SHE_GST_IceBRGpy
        
    # Internal C++ imports

    def testcIceBRGpyImport(self):
        import SHE_GST_cIceBRGpy

    def testSHESIMImport(self):
        import SHE_GST_PhysicalModel
