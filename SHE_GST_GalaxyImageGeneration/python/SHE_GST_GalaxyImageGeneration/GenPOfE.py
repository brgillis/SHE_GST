""" @file GenPOfE.py

    Created 23 Mar 2016

    Elements program for generating galaxy images.
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

import argparse

from EL_PythonUtils.utilities import get_arguments_string
from SHE_PPT.logging import getLogger

import SHE_GST
from SHE_GST_GalaxyImageGeneration.config.config_default import (allowed_options,
                                                                 allowed_fixed_params,
                                                                 allowed_survey_settings)
from SHE_GST_GalaxyImageGeneration.generate_p_of_e import generate_p_of_e
from SHE_GST_GalaxyImageGeneration.run_from_config import run_from_args


def defineSpecificProgramOptions():
    """
    @brief
        Defines options for this program, using all possible configurations.

    @return
        An  ArgumentParser.
    """

    parser = argparse.ArgumentParser()

    # Option for profiling
    parser.add_argument('--profile', action='store_true',
                        help='Store profiling data for execution.')

    # Extra configuration files
    parser.add_argument('--config_files', nargs='*', default=[],
                        help='Extra configuration files. Each will overwrite an values specified in previous ' +
                        'files, but NOT the one specified with the --config-file option.')

    # Header items
    parser.add_argument('--output_file_name', default="p_of_e.fits",
                        help='File name to store P(e) data in.')
    parser.add_argument('--header_items', nargs='*', default=[],
                        help='Items to be put in the header of the output table. Must be specified in pairs, eg. ' +
                        '--header_items Foo 117 Bar 0.1')
    parser.add_argument('--e_bins', type=int, default=100,
                        help='Number of bins in e (always from 0 to 1). Default = 100.')

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

    logger = getLogger(__name__)

    logger.debug('#')
    logger.debug('# Entering GenPOfE mainMethod()')
    logger.debug('#')

    exec_cmd = get_arguments_string(args, cmd="E-Run SHE_GST " + SHE_GST.__version__ + " SHE_GST_GenGalaxyImages",
                                    store_true=["profile", "debug"])
    logger.info('Execution command for this step:')
    logger.info(exec_cmd)

    if len(args.header_items) % 2 != 0:
        raise ValueError("An even number of items must be passed to the header_items argument.")
    header_items = args.header_items

    if(args.config_file is None and len(args.config_files) == 0):
        logger.info('Using default configurations.')
    else:
        if args.config_file is not None:
            logger.info('Using primary configuration file: ' + args.config_file)
        if len(args.config_files) > 0:
            logger.info('Using configurations in file(s): ')
            for config_file in args.config_files:
                logger.info('* ' + config_file)

    if args.profile:
        import cProfile
        cProfile.runctx("run_from_args(generate_images,args,output_file_name=output_file_name," +
                        "header_items=header_items,e_bins=e_bins)", {},
                        {"run_from_args": run_from_args,
                         "args": args,
                         "generate_p_of_e": generate_p_of_e,
                         "output_file_name": args.output_file_name,
                         "header_items": header_items,
                         "e_bins": args.e_bins},
                        filename="gen_galsim_images.prof")
    else:
        run_from_args(generate_p_of_e, args,
                      output_file_name=args.output_file_name,
                      header_items=header_items,
                      e_bins=args.e_bins)

    logger.debug('Exiting GenGalsimImages mainMethod()')
