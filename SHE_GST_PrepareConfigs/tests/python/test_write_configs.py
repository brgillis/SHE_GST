""" @file test_write_configs.py

    Created 11 Jun 2018

    Contains unit tests of functions in SHE_GST_PrepareConfigs/write_configs.py
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

from SHE_GST_GalaxyImageGeneration.config.parse_config import get_cfg_args
from SHE_PPT.file_io import (find_file, read_listfile)


class TestWriteConfigs:
    """


    """
    
    @classmethod
    def setup_class(cls):

        return

    @classmethod
    def teardown_class(cls):

        # Delete all potentially created files:
        for testfilepath in os.path.list("."):
            if ".junk" in testfilepath:
                os.remove(testfilepath)


    def test_write_config(self):
        
        filename = "test_config.junk"
        template_filename = find_file("AUX/SHE_GST_PrepareConfigs/StampsTemplate.conf")
        
        model_seed = 1234
        noise_seed = 2345
        suppress_noise = True
        num_detectors = 14
        num_galaxies = 62
        render_background = True
        
        write_config( filename,
                  template_filename,
                  model_seed=model_seed,
                  noise_seed=noise_seed,
                  suppress_noise=suppress_noise,
                  num_detectors=num_detectors,
                  num_galaxies=num_galaxies,
                  render_background=render_background)
        
        # Load the config and see if it matches our expectations
        cfg_args = get_cfg_args(filename, workdir=".")
        
        assert cfg_args["model_seed"] == model_seed
        assert cfg_args["noise_seed"] == noise_seed
        assert cfg_args["suppress_noise"] == suppress_noise
        assert cfg_args["num_detectors"] == num_detectors
        assert cfg_args["num_galaxies"] == num_galaxies
        assert cfg_args["render_background"] == render_background

        return
