""" @file psf_test.py

    Created 13 June 2018

    Tests of functions dealing with creating and managing PSF profiles and images.
"""

__updated__ = "2018-10-25"

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

from os.path import join

import galsim
import h5py
from numpy.testing import assert_almost_equal, assert_allclose
import pytest

from SHE_GST_GalaxyImageGeneration.psf import (get_psf_profile, get_background_psf_profile,
                                               add_psf_to_archive, get_psf_from_archive,
                                               load_psf_model_from_sed_z)
import numpy as np


class TestPSF:
    """


    """

    @classmethod
    def setup_class(cls):

        cls.pixel_scale = 0.01 / 3600
        cls.stamp_size = 128

        cls.z = 0
        cls.n = 1

        return

    @classmethod
    def teardown_class(cls):

        return

    @pytest.fixture(autouse=True)
    def setup(self, tmpdir):
        self.workdir = tmpdir.strpath

    def test_get_psf_profile(self):
        """ Some generic tests of get_psf_profile.
        """

        # Test that we get the expected foreground galaxy

        fg1 = get_psf_profile(self.n, self.z, bulge=False)
        fg2 = load_psf_model_from_sed_z("sb3", 0)

        assert fg1 == fg2

        # Make sure we get the expected background PSF when we request it.

        bkg1 = get_psf_profile(self.n, self.z, bulge=False, use_background_psf=True)
        bkg2 = get_background_psf_profile()

        assert bkg1 == bkg2

        # Make sure we get different PSFs for the bulge and disk

        chrom_d = get_psf_profile(self.n, self.z, bulge=False)
        chrom_b = get_psf_profile(self.n, self.z, bulge=True)

        assert chrom_d != chrom_b

        return

    def test_psf_archive(self):

        archive_filename = "psf_archive.hdf5"

        # Create some psfs and add them to the archive
        psf00 = get_background_psf_profile()
        psf10 = get_psf_profile(self.n, self.z, bulge=False)
        psf11 = get_psf_profile(self.n, self.z, bulge=True)
        
        archive_filehandle = h5py.File(join(self.workdir, archive_filename), 'a')

        add_psf_to_archive(psf00, archive_filehandle, galaxy_id=0, exposure_index=0, psf_type='bulge',
                           stamp_size=self.stamp_size, scale=self.pixel_scale)
        add_psf_to_archive(psf10, archive_filehandle, galaxy_id=1, exposure_index=0, psf_type='disk',
                           stamp_size=self.stamp_size, scale=self.pixel_scale)
        add_psf_to_archive(psf11, archive_filehandle, galaxy_id=1, exposure_index=1, psf_type='bulge',
                           stamp_size=self.stamp_size, scale=self.pixel_scale)

        # Read each of these back
        psf00_r = get_psf_from_archive(archive_filehandle, galaxy_id=0, exposure_index=0, psf_type='bulge')
        psf10_r = get_psf_from_archive(archive_filehandle, galaxy_id=1, exposure_index=0, psf_type='disk')
        psf11_r = get_psf_from_archive(archive_filehandle, galaxy_id=1, exposure_index=1, psf_type='bulge')

        # Test that we get an exception if searching for a psf that doesn't exist
        with pytest.raises(KeyError):
            get_psf_from_archive(archive_hdulist, galaxy_id=0, exposure_index=1)
        with pytest.raises(KeyError):
            get_psf_from_archive(archive_hdulist, galaxy_id=2, exposure_index=0)

        # Check that each of the ones we did read in is correct

        psf00_i = galsim.ImageF(self.stamp_size, self.stamp_size, scale=self.pixel_scale)
        psf00.drawImage(psf00_i)
        assert_allclose(psf00_i.array, psf00_r.array)
        assert_almost_equal(psf00_i.scale, psf00_r.scale)

        psf10_i = galsim.ImageF(self.stamp_size, self.stamp_size, scale=self.pixel_scale)
        psf10.drawImage(psf10_i)
        assert_allclose(psf10_i.array, psf10_r.array)
        assert_almost_equal(psf10_i.scale, psf11_r.scale)

        psf11_i = galsim.ImageF(self.stamp_size, self.stamp_size, scale=self.pixel_scale)
        psf11.drawImage(psf11_i)
        assert_allclose(psf11_i.array, psf11_r.array)
        assert_almost_equal(psf10_i.scale, psf11_r.scale)

        return
