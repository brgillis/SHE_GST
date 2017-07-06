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

from SHE_GST_GalaxyImageGeneration.gain import get_ADU_from_count, get_count_from_ADU
from SHE_GST_GalaxyImageGeneration.get_I_from_SN import get_I_from_SN
from SHE_GST_GalaxyImageGeneration.magic_values import mag_i_zeropoint, \
    mag_vis_zeropoint
import SHE_GST_GalaxyImageGeneration.magic_values as mv
import numpy as np


def get_count_from_mag_vis(m, exp_time):
    """ Gets the expected count from a magnitude using Euclid's magnitude zeropoint.

        @param m The input magnitude
        @param exp_time The exposure time

        @return The expected count
    """

    return exp_time * 10.0 ** (0.4 * (mv.mag_vis_zeropoint - m))

def get_mag_vis_from_count(c, exp_time):
    """ Gets the magnitude from the expected count using Euclid's magnitude zeropoint.

        @param c The input expected count
        @param exp_time The exposure time

        @return The magnitude
    """

    return mv.mag_vis_zeropoint - 2.5 * np.log10(c / exp_time)

def get_count_from_mag_i(m, exp_time):
    """ Gets the expected count from a magnitude using Euclid's magnitude zeropoint.

        @param m The input magnitude
        @param exp_time The exposure time

        @return The expected count
    """

    return exp_time * 10.0 ** (0.4 * (mv.mag_i_zeropoint - m))

def get_mag_i_from_count(c, exp_time):
    """ Gets the magnitude from the expected count using Euclid's magnitude zeropoint.

        @param c The input expected count
        @param exp_time The exposure time

        @return The magnitude
    """

    return mv.mag_i_zeropoint - 2.5 * np.log10(c / exp_time)

def get_I(I_parameter, parameter_type, gain, exp_time):
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
        return get_ADU_from_count(get_count_from_mag_vis(I_parameter, exp_time=exp_time), gain)
    elif(parameter_type == 'mag_i'):
        return get_ADU_from_count(get_count_from_mag_i(I_parameter, exp_time=exp_time), gain)
    else:
        raise Exception("get_I can't handle parameter type '" + str(parameter_type) + "'")
    return