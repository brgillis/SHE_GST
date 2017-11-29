""" @file PrepareConfigs.py

    Created 25 Aug 2017

    Main program for preparing configuration files for parallel runs.
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

import argparse
from SHE_PPT.logging import getLogger

from SHE_GST_PrepareConfigs import magic_values as mv
from SHE_GST_PrepareConfigs.write_configs import write_configs_from_plan

def defineSpecificProgramOptions():
    """
    @brief
        Defines options for this program.

    @return
        An ArgumentParser.
    """
    
    logger = getLogger(mv.logger_name)

    logger.debug('#')
    logger.debug('# Entering SHE_GST_PrepareConfigs defineSpecificProgramOptions()')
    logger.debug('#')

    parser = argparse.ArgumentParser()
    
    # Add program-specific arguments
    parser.add_argument("--simulation_plan", type=str,
                        "Plan file for how to run simulations; can be either XML product or fits table.")
    
    parser.add_argument("--config_template", default="AUX/SHE_GST_PrepareConfigs/StampsTemplate.conf",
                        "Template configuration file to use.")
        
    parser.add_argument("--simulation_configs", default="config_files.json", type=str,
                        help="Filename to which to output list of generated config files.")
    
    # Arguments needed by the pipeline runner
    parser.add_argument('--workdir',type=str,default=".")
    parser.add_argument('--logdir',type=str,default=".")

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

    logger = getLogger(mv.logger_name)

    logger.debug('#')
    logger.debug('# Entering SHE_CTE_PrepareConfigs mainMethod()')
    logger.debug('#')
        
    write_configs_from_plan(plan_filename = args.simulation_plan,
                            template_filename = args.config_template,
                            listfile_filename = args.simulation_configs,
                            workdir = args.workdir)

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