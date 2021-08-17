""" @file PrepareConfigs.py

    Created 25 Aug 2017

    Main program for preparing configuration files for parallel runs.
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
from SHE_GST_PrepareConfigs.write_configs import write_configs_from_plan


def defineSpecificProgramOptions():
    """
    @brief
        Defines options for this program.

    @return
        An ArgumentParser.
    """

    logger = getLogger(__name__)

    logger.debug('#')
    logger.debug('# Entering SHE_GST_PrepareConfigs defineSpecificProgramOptions()')
    logger.debug('#')

    parser = argparse.ArgumentParser()

    # Add program-specific arguments
    parser.add_argument("--simulation_plan", default="AUX/SHE_GST_PrepareConfigs/sample_simulation_plan.ecsv", type=str,
                        help="Plan file for how to run simulations; can be either XML product or fits table.")

    parser.add_argument("--config_template", default="AUX/SHE_GST_PrepareConfigs/StampsTemplate.conf",
                        help="Template configuration file to use.")

    parser.add_argument("--simulation_configs", default="config_files.json", type=str,
                        help="Filename to which to output list of generated config files.")

    parser.add_argument("--pipeline_config", default=None, type=str,
                        help="Pipeline-wide configuration file.")

    # Arguments needed by the pipeline runner
    parser.add_argument('--workdir', type=str, default=".")
    parser.add_argument('--logdir', type=str, default=".")

    logger.debug('# Exiting SHE_CTE_PrepareConfigs defineSpecificProgramOptions()')

    return parser


def mainMethod(args):
    """
    @brief
        The "main" method for this program, to estimate shears.

    @details
        This method is the entry point to the program. In this sense, it is
        similar to a main (and it is why it is called mainMethod()).
    """

    logger = getLogger(__name__)

    logger.debug('#')
    logger.debug('# Entering SHE_CTE_PrepareConfigs mainMethod()')
    logger.debug('#')

    exec_cmd = get_arguments_string(args, cmd="E-Run SHE_GST " + SHE_GST.__version__ + " SHE_GST_PrepareConfigs",
                                    store_true=["profile", "debug"])
    logger.info('Execution command for this step:')
    logger.info(exec_cmd)

    write_configs_from_plan(plan_filename=args.simulation_plan,
                            template_filename=args.config_template,
                            listfile_filename=args.simulation_configs,
                            workdir=args.workdir)

    logger.debug('# Exiting SHE_CTE_PrepareConfigs mainMethod()')

    return


def main():
    """
    @brief
        Alternate entry point for non-Elements execution.
    """

    parser = defineSpecificProgramOptions()

    args = parser.parse_args()

    mainMethod(args)

    return


if __name__ == "__main__":
    main()
