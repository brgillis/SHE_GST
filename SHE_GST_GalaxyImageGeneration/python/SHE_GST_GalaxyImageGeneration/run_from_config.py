""" @file run_from_config.py

    Created 27 Mar 2017

    Functions to apply configurations and run one of the programs
"""

__updated__ = "2021-08-17"

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

import subprocess

from SHE_PPT.logging import getLogger

from SHE_GST_GalaxyImageGeneration.config.config_default import (allowed_survey_settings,
                                                                 generation_levels_inverse,)
from SHE_GST_GalaxyImageGeneration.config.parse_config import (set_up_from_config_file,
                                                               load_default_configurations,
                                                               get_cfg_args,
                                                               apply_args)


def run_from_config_file(func, config_file_name, *args, **kwargs):

    survey, options = set_up_from_config_file(config_file_name)

    return run_from_survey_and_options(func, survey, options, *args, **kwargs)


def run_from_args(func, cline_args, *args, **kwargs):

    logger = getLogger(__name__)
    logger.debug("# Entering run_from_args method.")

    # Load defaults
    survey, options = load_default_configurations()

    # Apply arguments in extra config files specified
    for config_file_name in cline_args.config_files:
        logger.debug('Applying arguments from config file: ' + config_file_name)
        cfg_args = get_cfg_args(config_file_name, cline_args.workdir)
        apply_args(survey, options, cfg_args)

    # Apply cline-args
    apply_args(survey, options, cline_args)

    results = run_from_survey_and_options(func, survey, options, *args, **kwargs)

    logger.debug("# Exiting run_from_args method.")

    return results


def run_from_config_file_and_args(func, config_file_name, cline_args, *args, **kwargs):

    survey, options = set_up_from_config_file(config_file_name)

    # Apply cline-args
    apply_args(survey, options, cline_args)

    return run_from_survey_and_options(func, survey, options, *args, **kwargs)


def run_from_survey_and_options(func, survey, options, *args, **kwargs):

    # Check if the folder path was given with a slash at the end. If so, trim it
    if(options['workdir'][-1] == '/'):
        options['workdir'] = options['workdir'][0:-1]

    logger = getLogger(__name__)

    # Print all options we're using to the logger
    logger.debug("# Running with the following options: #")
    logger.debug("")
    for name in options:
        logger.debug(name + ": " + str(options[name]))
        logger.debug("")

    # Print survey settings and levels too
    logger.debug("# And using the following settings for the physical model: #")
    logger.debug("")
    for name in allowed_survey_settings:

        gen_level = generation_levels_inverse[int(survey.get_generation_level(name))]
        logger.debug(name + " generation level: " + gen_level)

        param_params = survey.get_param(name).get_params()
        pp_mode = param_params.name()
        logger.debug(name + " generation mode: " + pp_mode)
        pp_params = param_params.get_parameters_string()
        logger.debug(name + " generation parameters: " + pp_params)

        logger.debug("")

    # Ensure that the output folder exists
    cmd = 'mkdir -p ' + options['workdir']
    subprocess.call(cmd, shell=True)

    # We have the input we want, now run the program
    results = func(survey, options, *args, **kwargs)

    return results
