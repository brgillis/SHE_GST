""" @file utility/random.py

    Created 13 Oct 2015

    Functions related to random number generation

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

import numpy as np


def rand_from_cdf_arrays(xvals, cvals):

    # Normalize the cvals
    cvals -= cvals[0]
    cvals /= cvals[-1]

    # Generate a random value
    r = np.random.random()

    # Get the index on the cdf where this lies
    i = np.argmax(r < cvals)

    # Due to the way random generation works, we can safely ignore the pathological edge cases here
    # Interpolate to estimate the value
    clow = cvals[i - 1]
    chi = cvals[i]
    xlow = xvals[i - 1]
    xhi = xvals[i]

    res = xlow + (xhi - xlow) / (chi - clow) * (r - clow)

    return res

def rand_from_cdf(cdf_func, n=1000, xlow=-5., xhigh=5.):

    # Get an array of x points and cdf values at those points
    xvals = np.linspace(start=xlow, stop=xhigh, num=n)
    cvals = cdf_func(xvals)

    res = rand_from_cdf_arrays(xvals, cvals)

    return res

def rand_from_pdf(pdf_func, n=1000, xlow=-5., xhigh=5):

    # Get an array of x points and pdf values at those points
    xvals = np.linspace(start=xlow, stop=xhigh, num=n)
    pvals = pdf_func(xvals)

    # Get (unnormalized) cdf values
    cvals = np.cumsum(pvals)

    res = rand_from_cdf_arrays(xvals, cvals)

    return res

def Schechter_pdf(mag, m_star, alpha):

    # Define the Shechter pdf
    x = 10.**(0.4 * (m_star - mag))
    p = x ** (alpha + 1) * np.exp(-x)

    return p

def exp_quad_pdf(mag, N_scale=9.69722, hinge_mag=23., beta_0=0.789647, d_beta=-0.0397336):

    # Define the pdf
    m = mag - hinge_mag
    p = N_scale * np.exp(beta_0 * m + d_beta * np.square(m))

    return p

def softened_Rayleigh_pdf(g, sigma, shear_soften, shear_max, soften_param):
    p = (g / sigma ** 2) * np.exp(-g ** 2 / (2 * sigma ** 2))

    if g > shear_soften:
        if g > shear_max:
            return 0.
        else:
            p *= np.cos(np.pi * (g - shear_soften) / soften_param)

    return p
