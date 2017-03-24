""" @file magnitude_conversions.py

    Created 6 Oct 2015

    Functions to convert between Euclid magnitude and electron count

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

from SHE_SIM_galaxy_image_generation.gain import get_ADU_from_count, get_count_from_ADU
from SHE_SIM_galaxy_image_generation.get_I_from_SN import get_I_from_SN
from SHE_SIM_galaxy_image_generation.magic_values import mag_i_zeropoint, \
    mag_vis_zeropoint
import SHE_SIM_galaxy_image_generation.magic_values as mv
import numpy as np


def get_count_from_mag_vis(m, exp_time=mv.default_exp_time):
    """ Gets the expected count from a magnitude using Euclid's magnitude zeropoint.

        @param m The input magnitude
        @param exp_time The exposure time

        @return The expected count
    """

    return exp_time * 10.0 ** (0.4 * (mv.mag_vis_zeropoint - m))

def get_mag_vis_from_count(c, exp_time=mv.default_exp_time):
    """ Gets the magnitude from the expected count using Euclid's magnitude zeropoint.

        @param c The input expected count
        @param exp_time The exposure time

        @return The magnitude
    """

    return mv.mag_vis_zeropoint - 2.5 * np.log10(c / exp_time)

def get_count_from_mag_i(m, exp_time=mv.default_exp_time):
    """ Gets the expected count from a magnitude using Euclid's magnitude zeropoint.

        @param m The input magnitude
        @param exp_time The exposure time

        @return The expected count
    """

    return exp_time * 10.0 ** (0.4 * (mv.mag_i_zeropoint - m))

def get_mag_i_from_count(c, exp_time=mv.default_exp_time):
    """ Gets the magnitude from the expected count using Euclid's magnitude zeropoint.

        @param c The input expected count
        @param exp_time The exposure time

        @return The magnitude
    """

    return mv.mag_i_zeropoint - 2.5 * np.log10(c / exp_time)

def get_I(I_parameter, parameter_type, gain=mv.default_gain, exp_time=mv.default_exp_time):
    """ Gets the measured intensity from the provided parameters

        @param c The input expected count
        @param exp_time The exposure time

        @return The measured intensity
    """

    if(parameter_type == 'intensity'):
        return I_parameter
    elif(parameter_type == 'count'):
        return get_ADU_from_count(I_parameter, gain)
    elif(parameter_type == 'flux'):
        return get_ADU_from_count(I_parameter * exp_time, gain)
    elif(parameter_type == 'mag_vis'):
        return get_ADU_from_count(get_count_from_mag_vis(I_parameter, exp_time=exp_time))
    elif(parameter_type == 'mag_i'):
        return get_ADU_from_count(get_count_from_mag_i(I_parameter, exp_time=exp_time))
    else:
        raise Exception("get_I can't handle parameter type '" + str(parameter_type) + "'")
    return

def get_mag_i(config_dict):
    """ Gets the i-band magnitude from the values stored in the config dictionary

        @param config_dict The configuration dictionary

        @return The magnitude
    """

    parameter_type = config_dict['intensity_type']
    I_parameter = config_dict['galaxy_I_parameter']

    if(parameter_type == 'intensity'):
        return get_mag_i_from_count(get_count_from_ADU(I_parameter, gain=config_dict['gain']),
                                    exp_time=config_dict['exp_time'])
    elif(parameter_type == 'count'):
        return get_mag_i_from_count(I_parameter, exp_time=config_dict['exp_time'])
    elif(parameter_type == 'flux'):
        return get_mag_i_from_count(I_parameter, exp_time=1)
    elif(parameter_type == 'mag_vis'):
        return I_parameter - mag_i_zeropoint + mag_vis_zeropoint
    elif(parameter_type == 'mag_i'):
        return I_parameter
    elif(parameter_type == 's/n'):
        I = get_I_from_SN(config_dict['galaxy_I_parameter']     ,
                                    config_dict['galaxy_stddev_arcsec']  ,
                                    config_dict['psf_stddev_arcsec']     ,
                                    config_dict['sky_level_subtracted']  ,
                                    config_dict['sky_level_unsubtracted'],
                                    config_dict['read_noise']            ,
                                    config_dict['sample_scale']          ,
                                    config_dict['gain'])
        return get_mag_i_from_count(get_count_from_ADU(I, gain=config_dict['gain']),
                                    exp_time=config_dict['exp_time'])
    else:
        raise Exception("get_mag_i can't handle parameter type '" + str(parameter_type) + "'")
    return
