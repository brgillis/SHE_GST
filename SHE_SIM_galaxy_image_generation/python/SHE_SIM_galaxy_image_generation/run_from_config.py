""" @file run_from_config.py

    Created 27 Mar 2017

    Functions to apply configurations and run one of the programs

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

import subprocess

from SHE_SIM_galaxy_image_generation import magic_values as mv
from SHE_SIM_galaxy_image_generation.config.config_default import (allowed_survey_settings,
                                                                   generation_levels_inverse,)
from SHE_SIM_galaxy_image_generation.config.parse_config import (set_up_from_config_file,
                                                                 load_default_configurations,
                                                                 get_cfg_args,
                                                                 apply_args)
from icebrgpy.logging import getLogger


try:
    import pyfftw
    import pickle
    have_pyfftw = True
except ImportError as _e:
    have_pyfftw = False


def run_from_config_file(func, config_file_name, *args, **kwargs):

    survey, options = set_up_from_config_file(config_file_name)

    run_from_survey_and_options(func, survey, options, *args, **kwargs)

    return

def run_from_args(func, cline_args, *args, **kwargs):

    logger = getLogger(mv.logger_name)
    logger.debug("# Entering run_from_args method.")

    # Load defaults
    survey, options = load_default_configurations()
    
    # Apply arguments in extra config files specified
    for config_file_name in cline_args.config_files:
        logger.debug('Applying arguments from config file: ' + config_file_name)
        cfg_args = get_cfg_args(config_file_name)
        apply_args(survey, options, cfg_args)
    
    # Apply cline-args
    apply_args(survey, options, cline_args)

    run_from_survey_and_options(func, survey, options, *args, **kwargs)
    
    logger.debug("# Exiting run_from_args method.")

    return

def run_from_config_file_and_args(func, config_file_name, cline_args, *args, **kwargs):

    survey, options = set_up_from_config_file(config_file_name)
    
    # Apply cline-args
    apply_args(survey, options, cline_args)

    run_from_survey_and_options(func, survey, options, *args, **kwargs)

    return

def run_from_survey_and_options(func, survey, options, *args, **kwargs):

    # Check if the folder path was given with a slash at the end. If so, trim it
    if(options['output_folder'][-1] == '/'):
        options['output_folder'] = options['output_folder'][0:-1]

    logger = getLogger(mv.logger_name)

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
    cmd = 'mkdir -p ' + options['output_folder']
    subprocess.call(cmd, shell=True)

    # Set up pyfftw
    if have_pyfftw:
        pyfftw.interfaces.cache.enable()
        try:
            pyfftw.import_wisdom(pickle.load(open(mv.fftw_wisdom_filename, "rb")))
        except IOError as _e:
            pass

    # We have the input we want, now run the program
    func(survey, options, *args, **kwargs)

    # Save fftw wisdom
    if have_pyfftw:
        pickle.dump(pyfftw.export_wisdom(), open(mv.fftw_wisdom_filename, "wb"))

    return
