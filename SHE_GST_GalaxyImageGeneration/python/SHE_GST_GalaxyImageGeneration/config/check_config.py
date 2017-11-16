""" @file /disk2/brg/Program_Files/workspace/Generate_GalSim_Images/SHE_GST_GalaxyImageGeneration/config/check_config.py

    Created 15 Mar 2016

    @TODO: File docstring
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

from copy import deepcopy

from SHE_GST_GalaxyImageGeneration import magic_values as mv
from SHE_GST_GalaxyImageGeneration.config.config_default import (allowed_option_values,
                                                                 allowed_survey_settings,
                                                                 generation_levels)
from SHE_PPT.logging import getLogger

def get_full_options(options,image):
    """
        @brief Get a dictionary containing a full description of the model for this image.
        
        @param options <dict> Options dictionary
        
        @param image <SSImage> image object containing realization of the physical model
        
        @param full_options <dict>
    """

    logger = getLogger(mv.logger_name)
    
    logger.debug("# Entering get_full_options method.")
    
    # Start with a copy of the options dictionary
    full_options = deepcopy(options)
    
    # Delete options which don't affect images
    del (full_options['details_only'],
         full_options['details_output_format'],
         full_options['dithering_scheme'],
         full_options['num_parallel_threads'],
         full_options['workdir'],
         full_options['output_file_name_base'],
         full_options['psf_file_name_base'],
         full_options['seed'], # stored separately
         )
    
    # Add allowed survey settings, with both level and setting possibilities
    for allowed_survey_setting in allowed_survey_settings:

        logger.debug("Getting generation level: " + allowed_survey_setting + "_level")
        full_options[allowed_survey_setting + "_level"] = image.get_generation_level(allowed_survey_setting)
        
        logger.debug("Getting survey setting: " + allowed_survey_setting + "_level")
        param_settings = image.get_param(allowed_survey_setting).get_params()
        full_options[allowed_survey_setting + "_setting"] = param_settings.name() + " " + param_settings.get_parameters_string()
    
    return full_options
    
    logger.debug("# Exiting get_full_options method.")
    
def check_options(options):
    for name in options:
        # Check if it's an allowed value
        if name in allowed_option_values:
            if options[name] not in allowed_option_values[name]:
                raise Exception("Invalid setting '" + str(options[name]) + "' for option '" +
                                name + "'. Allowed settings are: " + str(allowed_option_values[name])
                                + ".")

    return

def check_survey_settings(survey):

    # Check the values we read in against hard bounds and adjust if necessary,
    # printing a warning.

    if survey.get_param_value("num_images") < mv.min_num_images:
        survey.set_param_param("num_images", "fixed", mv.min_num_images)
        print "WARNING: Adjusted number of images to minimum of " + str(mv.min_num_images) + "."

    if survey.get_param_value("pixel_scale") < mv.min_pixel_scale:
        survey.set_param_param("pixel_scale", "fixed", mv.min_pixel_scale)
        print "WARNING: Adjusted pixel_scale to minimum of " + str(mv.min_pixel_scale) + "."
        print "Check you're using units of arcsec/pixel!"

    return

def handle_special_settings(options, survey):
    """ This function applies special handling for certain options and survey settings.
    """

    # For stamps mode, we want zero clustering, so xp and yp will be uniform for all galaxies
    # and we can use them for other values without affecting the seed
    if options['mode'] == 'stamps':
        survey.set_generation_level('cluster_density', generation_levels['survey'])
        survey.set_param_params('cluster_density', 'fixed', 0.)

    return
