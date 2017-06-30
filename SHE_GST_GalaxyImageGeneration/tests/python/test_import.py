"""
    Tests of various imports, to make sure galsim is installed and other parts of the project are
    linked correctly.
"""

import unittest


class TestCase(unittest.TestCase):

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
