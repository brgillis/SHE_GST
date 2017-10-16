"""
    @file GenGalaxyImages.py

    Created 23 Mar 2016

    Elements program for generating galaxy images.

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

import argparse

from SHE_GST_GalaxyImageGeneration import magic_values as mv
from SHE_GST_GalaxyImageGeneration.config.config_default import (allowed_options,
                                                                 allowed_fixed_params,
                                                                 allowed_survey_settings)
from SHE_GST_GalaxyImageGeneration.run_from_config import run_from_args
from SHE_GST_GalaxyImageGeneration.generate_images import generate_images
from SHE_GST_IceBRGpy.logging import getLogger

def defineSpecificProgramOptions():
    """
    @brief
        Defines options for this program, using all possible configurations.

    @return
        An  ArgumentParser.
    """

    parser = argparse.ArgumentParser()

    # Option for profiling
    parser.add_argument('--profile',action='store_true',
                        help='Store profiling data for execution.')
    
    # Extra configuration files
    parser.add_argument('--config_files', nargs='*', default=[],
                        help='Extra configuration files. Each will overwrite an values specified in previous ' +
                             'files, but NOT the one specified with the --config-file option.')

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

    logger.debug('#')
    logger.debug('# Entering GenGalaxyImages mainMethod()')
    logger.debug('#')

    if(args.config_file is None and len(args.config_files)==0):
        logger.info('Using default configurations.')
    else:
        if args.config_file is not None:
            logger.info('Using primary configuration file: ' + args.config_file)
        if len(args.config_files)>0:
            logger.info('Using configurations in file(s): ')
            for config_file in args.config_files:
                logger.info('* ' + config_file)
        
    if args.profile:
        import cProfile
        cProfile.runctx("run_from_args(generate_images,args)",{},
                        {"run_from_args":run_from_args,
                         "args":args,
                         "generate_images":generate_images},
                        filename="gen_galaxy_images.prof")
    else:
        run_from_args(generate_images, args)

    logger.debug('Exiting GenGalaxyImages mainMethod()')

    return

