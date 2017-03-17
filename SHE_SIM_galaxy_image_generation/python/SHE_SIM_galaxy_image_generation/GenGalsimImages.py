"""
    @file GenGalsimImages.py

    Created 23 Mar 2016

    Elements program for generating galaxy images.

    ---------------------------------------------------------------------

    Copyright (C) 2016 Bryan R. Gillis

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

import argparse

from SHE_SIM_galaxy_image_generation import magic_values as mv
from SHE_SIM_galaxy_image_generation.config.config_default import (allowed_options,
                                                            allowed_fixed_params,
                                                            allowed_survey_settings)
from SHE_SIM_galaxy_image_generation.run_from_config import run_from_config_file_and_args
from icebrgpy.logging import getLogger

def defineSpecificProgramOptions():
    """
    @brief
        Defines options for this program, using all possible configurations.

    @return
        An  ArgumentParser.
    """

    parser = argparse.ArgumentParser()

    parser.add_argument('--config-file-name', type=str, default="",
                        help='Filename of the configuration file to use for values not specified ' +
                        'in the options here.')
    parser.add_argument('--profile',action='store_true',
                        help='Store profiling data for execution.')

    # Add in each allowed option, with a null default
    for option in allowed_options:
        option_type = allowed_options[option][1]
        parser.add_argument("--" + option, type=option_type)

    # Add allowed fixed params
    for allowed_fixed_param in allowed_fixed_params:
        parser.add_argument("--" + allowed_fixed_param, type=float)

    # Add allowed survey settings, with both level and setting possibilities
    for allowed_survey_setting in allowed_survey_settings:

        generation_level = allowed_survey_setting + "_level"
        parser.add_argument("--" + generation_level, type=str)

        settings = allowed_survey_setting + "_setting"
        parser.add_argument("--" + settings, type=str)

    return parser


def mainMethod(args):
    """
    @brief
        The "main" method for this program, to generate galaxy images.

    @details
        This method is the entry point to the program. In this sense, it is
        similar to a main (and it is why it is called mainMethod()).
    """

    logger = getLogger(mv.logger_name)

    logger.info('#')
    logger.info('# Entering GenGalsimImages mainMethod()')
    logger.info('#')

    config_file_name = args.config_file_name

    if(config_file_name == ""):
        logger.info('Using default configurations.')
    else:
        logger.info('Using configurations from file ' + config_file_name + '.')
        
    if args.profile:
        import cProfile
        cProfile.runctx("run_from_config_file_and_args(config_file_name, args)",{},
                        {"run_from_config_file_and_args":run_from_config_file_and_args,
                         "config_file_name":config_file_name,"args":args},filename="gen_galsim_images.prof")
    else:
        run_from_config_file_and_args(config_file_name, args)

    logger.info('Exiting GenGalsimImages mainMethod()')

    return

