""" @file mag_size_model/skew_norm.py

    Created 13 Oct 2015

    Functions related to skewed normal distributions

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

from scipy.stats import norm
import numpy as np


def sn_pdf(x, a=0., loc=0., scale=1.):
    """ Skewed normal pdf.

        @param x The position at which to determine the pdf
        @param a The "alpha" parameter for the distribution, which determines the skewness. Default 0.
        @param loc The location of the distribution. Default 0.
        @param scale The scale of the distribution. Default 1.
    """

    z = (x - loc) / scale
    return 2. * norm.pdf(z) * norm.cdf(a * z) / scale

def sn_mean(a=0., loc=0., scale=1.):
    """ Get the mean of a skewed normal distribution

        @param a The "alpha" parameter for the distribution, which determines the skewness. Default 0.
        @param loc The location of the distribution. Default 0.
        @param scale The scale of the distribution. Default 1.
    """

    return loc + scale * np.sqrt(2. / np.pi) * a / np.sqrt(1. + np.square(a))

def sn_variance(a=0., scale=1.):
    """ Get the variance of a skewed normal distribution

        @param a The "alpha" parameter for the distribution, which determines the skewness. Default 0.
        @param scale The scale of the distribution. Default 1.
    """

    return np.square(scale) * (1. - 2 * a * a / (np.pi * (1 + a * a)))

def sn_stddev(a=0., scale=1.):
    """ Get the standard deviation of a skewed normal distribution

        @param a The "alpha" parameter for the distribution, which determines the skewness. Default 0.
        @param scale The scale of the distribution. Default 1.
    """

    return np.sqrt(sn_variance(a=a, scale=scale))

def sn_scale(a=0., stddev=1.):
    """ Get the scale of a skewed normal distribution from the stddev

        @param a The "alpha" parameter for the distribution, which determines the skewness. Default 0.
        @param std The standard deviation of the distribution. Default 1.
    """

    return stddev / np.sqrt(1. - 2 * a * a / (np.pi * (1 + a * a)))

def sn_skewness(a=0.):
    """ Get the skewness of a skewed normal distribution

        @param a The "alpha" parameter for the distribution, which determines the skewness. Default 0.
        @param scale The scale of the distribution. Default 1.
    """

    d_square = a * a / (1. + a * a)
    return (2. - np.pi / 2.) * np.power((2.*d_square / np.pi) / (1. - 2.*d_square / np.pi), 3. / 2.) * np.sign(a)

def sn_kurtosis(a=0.):
    """ Get the kurtosis of a skewed normal distribution

        @param a The "alpha" parameter for the distribution, which determines the skewness. Default 0.
        @param scale The scale of the distribution. Default 1.
    """

    d_square = a * a / (1. + a * a)
    return 2.*(np.pi - 3.) * np.square((2.*d_square / np.pi) / (1. - 2.*d_square / np.pi))
