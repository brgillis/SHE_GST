""" @file test_write_configs.py

    Created 11 Jun 2018

    Contains unit tests of functions in SHE_GST_PrepareConfigs/write_configs.py
"""

__updated__ = "2018-09-21"

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

import os
import pytest

from SHE_PPT.file_io import (find_file, read_listfile, get_data_filename)

from SHE_GST_GalaxyImageGeneration.config.parse_config import get_cfg_args
from SHE_GST_PrepareConfigs.write_configs import write_config, write_configs_from_plan
from astropy.table import Table


class TestWriteConfigs:
    """


    """

    @classmethod
    def setup_class(cls):

        return

    @classmethod
    def teardown_class(cls):

        return

    @pytest.fixture(autouse=True)
    def setup(self, tmpdir):
        self.workdir = tmpdir.strpath
        os.mkdir(os.path.join(self.workdir, "data"))

    def test_write_config(self):
        """ General tests of the write_config function.
        """

        filename = os.path.join(self.workdir, "test_config.junk")
        template_filename = find_file("AUX/SHE_GST_PrepareConfigs/StampsTemplate.conf")

        model_seed = 1234
        noise_seed = 2345
        suppress_noise = True
        num_detectors = 14
        num_galaxies = 62
        render_background = True

        write_config(filename,
                     template_filename,
                     model_seed=model_seed,
                     noise_seed=noise_seed,
                     suppress_noise=suppress_noise,
                     num_detectors=num_detectors,
                     num_galaxies=num_galaxies,
                     render_background=render_background)

        # Load the config and see if it matches our expectations
        cfg_args = get_cfg_args(filename, workdir=self.workdir)

        assert cfg_args["seed"] == model_seed
        assert cfg_args["noise_seed"] == noise_seed
        assert cfg_args["suppress_noise"] == suppress_noise
        assert cfg_args["num_images"] == num_detectors
        assert cfg_args["num_target_galaxies"] == num_galaxies
        assert cfg_args["render_background_galaxies"] == render_background

        return

    def test_write_configs_from_plan(self):
        """ General tests of the write_configs_from_plan function.
        """

        listfile_filename = os.path.join(self.workdir, "test_listfile.junk")
        template_filename = find_file("AUX/SHE_GST_PrepareConfigs/StampsTemplate.conf")
        plan_filename = find_file("AUX/SHE_GST_PrepareConfigs/test_simulation_plan.fits")

        write_configs_from_plan(plan_filename,
                                template_filename,
                                listfile_filename,
                                workdir=self.workdir)

        config_filenames = read_listfile(listfile_filename)

        assert len(config_filenames) == 20

        for i in range(10):

            # Check first set of 10
            config_p_filename = config_filenames[i]
            config_filename = get_data_filename(config_p_filename, workdir=self.workdir)

            cfg_args = get_cfg_args(config_filename, workdir=self.workdir)

            assert cfg_args["seed"] == i + 1
            assert cfg_args["noise_seed"] == i + 1
            assert cfg_args["suppress_noise"] == False
            assert cfg_args["num_images"] == 36
            assert cfg_args["num_target_galaxies"] == 1024
            assert cfg_args["render_background_galaxies"] == True

            # Check second set of 10
            config_p_filename = config_filenames[i + 10]
            config_filename = get_data_filename(config_p_filename, workdir=self.workdir)

            cfg_args = get_cfg_args(config_filename, workdir=self.workdir)

            assert cfg_args["seed"] == i + 1
            assert cfg_args["noise_seed"] == i + 1
            assert cfg_args["suppress_noise"] == True
            assert cfg_args["num_images"] == 36
            assert cfg_args["num_target_galaxies"] == 1024
            assert cfg_args["render_background_galaxies"] == False

        return

    def test_zero_steps(self):
        """ Tests of write_configs_from_plan that test zero step size for model/noise seed.
        """

        listfile_filename = os.path.join(self.workdir, "test_listfile.junk")
        template_filename = find_file("AUX/SHE_GST_PrepareConfigs/StampsTemplate.conf")
        base_plan_filename = find_file("AUX/SHE_GST_PrepareConfigs/test_simulation_plan.fits")
        plan_filename = os.path.join(self.workdir, "test_plan.fits")

        num_seeds = 5

        # Test for zero model seed step

        base_plan = Table.read(base_plan_filename, format="fits")
        base_plan.remove_row(1)  # Remove the second row for these tests
        base_plan[0]["MSEED_MIN"] = 15
        base_plan[0]["MSEED_MAX"] = 15
        base_plan[0]["MSEED_STEP"] = 0
        base_plan[0]["NSEED_MIN"] = 2
        base_plan[0]["NSEED_MAX"] = num_seeds * 2
        base_plan[0]["NSEED_STEP"] = 2

        base_plan.write(plan_filename, format="fits", overwrite=True)

        write_configs_from_plan(plan_filename,
                                template_filename,
                                listfile_filename,
                                workdir=self.workdir)

        config_filenames = read_listfile(listfile_filename)

        assert len(config_filenames) == num_seeds

        for i in range(num_seeds):

            config_p_filename = config_filenames[i]
            config_filename = get_data_filename(config_p_filename, workdir=self.workdir)

            cfg_args = get_cfg_args(config_filename, workdir=self.workdir)

            assert cfg_args["seed"] == 15
            assert cfg_args["noise_seed"] == 2 * i + 2
            assert cfg_args["suppress_noise"] == False
            assert cfg_args["num_images"] == 36
            assert cfg_args["num_target_galaxies"] == 1024
            assert cfg_args["render_background_galaxies"] == True

        # Test for zero noise seed step

        base_plan[0]["MSEED_MIN"] = 4
        base_plan[0]["MSEED_MAX"] = 2 + num_seeds * 2
        base_plan[0]["MSEED_STEP"] = 2
        base_plan[0]["NSEED_MIN"] = 3
        base_plan[0]["NSEED_MAX"] = 3
        base_plan[0]["NSEED_STEP"] = 0

        base_plan.write(plan_filename, format="fits", overwrite=True)

        write_configs_from_plan(plan_filename,
                                template_filename,
                                listfile_filename,
                                workdir=self.workdir)

        config_filenames = read_listfile(listfile_filename)

        assert len(config_filenames) == num_seeds

        for i in range(num_seeds):

            config_p_filename = config_filenames[i]
            config_filename = get_data_filename(config_p_filename, workdir=self.workdir)

            cfg_args = get_cfg_args(config_filename, workdir=self.workdir)

            assert cfg_args["seed"] == 2 * i + 4
            assert cfg_args["noise_seed"] == 3
            assert cfg_args["suppress_noise"] == False
            assert cfg_args["num_images"] == 36
            assert cfg_args["num_target_galaxies"] == 1024
            assert cfg_args["render_background_galaxies"] == True

        # Test for zero model and noise seed steps

        base_plan[0]["MSEED_MIN"] = 15
        base_plan[0]["MSEED_MAX"] = 15
        base_plan[0]["MSEED_STEP"] = 0
        base_plan[0]["NSEED_MIN"] = 3
        base_plan[0]["NSEED_MAX"] = 3
        base_plan[0]["NSEED_STEP"] = 0

        base_plan.write(plan_filename, format="fits", overwrite=True)

        write_configs_from_plan(plan_filename,
                                template_filename,
                                listfile_filename,
                                workdir=self.workdir)

        config_filenames = read_listfile(listfile_filename)

        assert len(config_filenames) == 1

        for i in range(1):

            config_p_filename = config_filenames[i]
            config_filename = get_data_filename(config_p_filename, workdir=self.workdir)

            cfg_args = get_cfg_args(config_filename, workdir=self.workdir)

            assert cfg_args["seed"] == 15
            assert cfg_args["noise_seed"] == 3
            assert cfg_args["suppress_noise"] == False
            assert cfg_args["num_images"] == 36
            assert cfg_args["num_target_galaxies"] == 1024
            assert cfg_args["render_background_galaxies"] == True

        return
