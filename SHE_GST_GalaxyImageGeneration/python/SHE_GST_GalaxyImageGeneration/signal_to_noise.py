""" @file SHE_GST_GalaxyImageGeneration/signal_to_noise.py

    Created 24 Oct 2018

    @TODO: File docstring
"""
from Program_Files.GalSim.galsim.errors import GalSimHSMError

from SHE_PPT.logging import getLogger
import galsim

from SHE_PPT.signal_to_noise import get_SN_of_image


__updated__ = "2018-10-24"

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


logger = getLogger(__name__)


def get_signal_to_noise_estimate(ra, dec, image, background, rms, gain, stamp_size):
    """Gets a signal to noise estimate for the galaxy at a given location in an image. This is used as a wrapper
    for SHE_PPT.signal_to_noise.get_SN_of_image in order to extract the galaxy's stamp first.

    Parameters
    ----------
    ra : float
        The galaxy's Right Ascension
    dec : float
        The galaxy's Declination
    image : galsim.Image
        Image containing the galaxies
    background : galsim.Image
        Background map
    rms : float
        Sky noise in ADU
    gain : float
        Gain of the detector in e-/ADU
    stamp_size : int
        Size of the postage stamp to extract around each galaxy

    Returns
    -------
    signal_to_noise : float
        The galaxy's signal to noise

    """

    # Get the galaxy's pixel position on the image
    xy = image.wcs.toImage(galsim.PositionD(ra, dec))
    xp = xy.x
    yp = xy.y

    # Determine the galaxy's bounds
    xp_i = int(xp)
    yp_i = int(yp)

    # Determine boundaries
    xl = xp_i - stamp_size // 2
    xh = xl + stamp_size - 1
    yl = yp_i - stamp_size // 2
    yh = yl + stamp_size - 1

    # Check if the stamp crosses an edge and adjust as necessary
    x_shift = 0
    if xl < 1:
        x_shift = 1 - xl
    elif xh > image.array.shape[1]:
        x_shift = image.array.shape[1] - xh
    xh += x_shift
    xl += x_shift

    y_shift = 0
    if yl < 1:
        y_shift = 1 - yl
    elif yh > image.array.shape[0]:
        y_shift = image.array.shape[0] - yh
    yh += y_shift
    yl += y_shift

    gal_bounds = galsim.BoundsI(xl, xh, yl, yh)

    gal_stamp = image[gal_bounds] - background[gal_bounds]

    try:
        signal_to_noise = get_SN_of_image(gal_stamp, gain, sigma_sky=rms)
    except RuntimeError as e:
        if not "HSM Error" in str(e):
            raise
        logger.debug("Cannot calculated S/N for galaxy: " + str(e))
        return 0

    return signal_to_noise
