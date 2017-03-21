""" @file parse_config.py

    Created 8 Dec 2015

    @TODO: File docstring

    ---------------------------------------------------------------------

    Copyright (C) 2015 Bryan R. Gillis

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

from string import split

import SHE_SIM
from SHE_SIM_galaxy_image_generation import magic_values as mv
from SHE_SIM_galaxy_image_generation.config.config_default import (allowed_options,
                                                            allowed_fixed_params,
                                                            allowed_survey_settings,
                                                            generation_levels,
                                                            load_default_configurations)
from icebrgpy.logging import getLogger

__all__ = ['get_cfg_args','set_up_from_config_file','apply_args','clean_quotes']

def get_cfg_args(config_filename):
    
    cfg_args = {}

    with open(config_filename, 'r') as config_file:
        # Read in the file, except for comment lines
        for config_line in config_file:
            stripped_line = config_line.strip()
            if (config_line[0] != '#') and (len(stripped_line) > 0):
                parse_line(stripped_line, cfg_args)

    return cfg_args

def set_up_from_config_file(config_file_name):

    survey, options = load_default_configurations()

    if config_file_name is not None:
        cfg_args = get_cfg_args(config_file_name)
        apply_args(survey, options, cfg_args)
    
    return survey, options

def parse_line(line, args):

    # Split by comments
    no_comments_line = split(line, sep='#')[0]

    # Split by =
    eq_split_line = split(no_comments_line, sep=':')

    # Check if this looks good
    if len(eq_split_line) != 2:
        if len(eq_split_line) < 2:
            # It's empty or only a comment, so ignore it
            return
        else:
            # More than one = outside of comments - it's malformatted
            raise Exception("Comment line is malformatted: '" + line + "'")

    option = eq_split_line[0].strip()
    setting_str = eq_split_line[1].strip()
    
    # Check possible validity of this option
    if option in allowed_options:
        
        # Convert to proper type and store in args
        args[option] = allowed_options[option][1](setting_str)
        
    elif option in allowed_fixed_params:
        
        # Convert to float and store
        args[option] = float(setting_str)
        
    elif option.replace("_setting","") in allowed_survey_settings:
        
        args[option] = str(setting_str)
        
    elif option.replace("_level","") in allowed_survey_settings:
        if setting_str not in generation_levels:
            raise Exception("Invalid generation level: " + setting_str)
        args[option] = str(setting_str)
    else:
        raise Exception("Unrecognized configuration option: " + option)

def apply_args(survey, options, args):

    logger = getLogger(mv.logger_name)
    
    logger.debug("# Entering apply_args method.")

    arg_lib = vars(args)

    # Check if each option was overriden in the args
    for option in allowed_options:
        if option in arg_lib:
            logger.debug("Applying option " + option + ": " + str(arg_lib[option]))
            if arg_lib[option] is not None:
                options[option] = clean_quotes(arg_lib[option])

    # Add allowed fixed params
    for fixed_param in allowed_fixed_params:
        if fixed_param in arg_lib:
            logger.debug("Applying fixed param " + fixed_param + ": " + str(arg_lib[fixed_param]))
            if arg_lib[fixed_param] is not None:
                survey.set_param_params(fixed_param, 'fixed', clean_quotes(arg_lib[fixed_param]))


    # Add allowed survey settings, with both level and setting possibilities
    for param_name in allowed_survey_settings:

        generation_level_name = param_name + "_level"
        if generation_level_name in arg_lib:
            logger.debug("Applying generation level " + generation_level_name + ": " + str(arg_lib[generation_level_name]))
            if arg_lib[generation_level_name] is not None:
                survey.set_generation_level(param_name,
                                            generation_levels[clean_quotes(arg_lib[generation_level_name])])


        settings_name = param_name + "_setting"
        if settings_name in arg_lib:
            logger.debug("Applying setting " + settings_name + ": " + str(arg_lib[settings_name]))
            if arg_lib[settings_name] is not None:

                split_params = clean_quotes(arg_lib[settings_name]).split()

                flt_args = []
                for str_arg in split_params[1:]:
                    flt_args.append(float(str_arg.strip()))

                survey.set_param_params(param_name, split_params[0].strip(), *flt_args)
        else:
            assert(False)
    
    logger.debug("# Exiting apply_args method.")

    return

def clean_quotes(s):
    if not isinstance(s, basestring): return s

    if s[0] == "'" and s[-1] == "'":
        s = s[1:-1]
    if s[0] == '"' and s[-1] == '"':
        s = s[1:-1]

    return s
