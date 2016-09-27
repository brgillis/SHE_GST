""" @file /disk2/brg/Program_Files/workspace/Generate_GalSim_Images/SHE_SIM_galaxy_image_generation/config/check_config.py

    Created 15 Mar 2016

    @TODO: File docstring

    ---------------------------------------------------------------------

    Copyright (C) 2016 brg

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

from SHE_SIM_galaxy_image_generation import magic_values as mv
from SHE_SIM_galaxy_image_generation.config.config_default import (allowed_option_values,
                                                            generation_levels)


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

    if(survey.get_param_value("num_images") < mv.min_num_images):
        survey.set_param_param("num_images", "fixed", mv.min_num_images)
        print "WARNING: Adjusted number of images to minimum of " + str(mv.min_num_images) + "."

    if(survey.get_param_value("pixel_scale") < mv.min_pixel_scale):
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
