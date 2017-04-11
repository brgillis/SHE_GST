#!/usr/bin/env python

"""gen_galsim_images.py
   Created by Bryan Gillis, March 2014
   Last edited by brg, 11 Aug 2015

   Contact: brg@roe.ac.uk

   Requirements: GalSim toolkit (and its requirements).
   GalSim can be downloaded from https://github.com/GalSim-developers/GalSim, and see
   https://github.com/GalSim-developers/GalSim/wiki/Installation-Instructions for its
   installation requirements.

   This is a script to generate a set of seeded random images using the GalSim toolkit,
   suitable for Euclid benchmarking tests of shear-measurement algorithms.
   This script should be invoked with a command that points it to an appropriate
   configuration script, for instance through:

   $ python gen_galsim_images.py my_config.cfg

   See the sample configuration files included with this script for the proper format.
   It is recommended that you start with an appropriate configuration script and modify
   it to suit your purposes.

   If a configuration file is not passed to this script, it will use the set of default
   configuration values assigned in the load_default_configurations(...) function below.

   NOTE 1: If you got this script or the configuration file from someone else, check the
   output directory and change it if necessary. It can use absolute paths, and the
   folders will be created if necessary, which may result in creating a file structure
   you don't want.

   NOTE 2: If you see odd bugs in running this, it's possibly due to a parallelization bug.
   This script assumes that all of the process it calls are not parallelized, and if they
   are, it can lead to clashes between threads. You can try fixing this by setting the
   following environmental variables before run (which one is actually needed depends on
   the specifics of your installation):
       export MKL_NUM_THREADS=1
       export NUMEXPR_NUM_THREADS=1
       export OPENBLAS_NUM_THREADS=1
    If this doesn't work, please let me know at brg@roe.ac.uk, and I'll try to figure out
    what's going on.
"""

import subprocess

from SHE_SIM_galaxy_image_generation import magic_values as mv
from SHE_SIM_galaxy_image_generation.config.config_default import (allowed_survey_settings,
                                                                   generation_levels_inverse,)
from SHE_SIM_galaxy_image_generation.config.parse_config import (set_up_from_config_file,
                                                                 load_default_configurations,
                                                                 get_cfg_args,
                                                                 apply_args)
from SHE_SIM_galaxy_image_generation.generate_images import generate_images
from icebrgpy.logging import getLogger


try:
    import pyfftw
    import pickle
    have_pyfftw = True
except ImportError as _e:
    have_pyfftw = False


def run_from_config_file(config_file_name):

    survey, options = set_up_from_config_file(config_file_name)

    run_from_survey_and_options(survey, options)

    return

def run_from_args(cline_args):

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

    run_from_survey_and_options(survey, options)
    
    logger.debug("# Exiting run_from_args method.")

    return

def run_from_config_file_and_args(config_file_name, cline_args):

    survey, options = set_up_from_config_file(config_file_name)
    
    # Apply cline-args
    apply_args(survey, options, cline_args)

    run_from_survey_and_options(survey, options)

    return

def run_from_survey_and_options(survey, options):

    # Check if the folder path was given with a slash at the end. If so, trim it
    if(options['output_folder'][-1] == '/'):
        options['output_folder'] = options['output_folder'][0:-1]

    logger = getLogger(mv.logger_name)

    # Print all options we're using to the logger
    logger.debug("# Generating images with the following options: #")
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

    # We have the input we want, now generate the images
    generate_images(survey, options)

    # Save fftw wisdom
    if have_pyfftw:
        pickle.dump(pyfftw.export_wisdom(), open(mv.fftw_wisdom_filename, "wb"))

    return
