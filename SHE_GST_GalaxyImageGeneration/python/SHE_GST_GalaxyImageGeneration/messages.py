"""
    @file messages.py

    Created 23 Jul 2015

    Various boilerplate messages to be reported to the user.
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

from SHE_GST_GalaxyImageGeneration import magic_values as mv
from SHE_PPT.logging import getLogger


def bad_config_format():
    """
        @brief A function to output a boilerplate error for a general problem with the
            configuration file.
    """

    raise Exception( "ERROR: Improperly formatted configuration file. Please check"
                    + "sample files and ensure it has one of the proper formats." )


def old_config_version( ver ):
    """
        @brief A boilerplate alert that the config file version being used is old, and some default values
            will have to be assumed.
    """

    logger = getLogger( mv.logger_name )

    logger.info( "Using configuration file for version " + ver +
                " of the script. This is an old version. Default values will be used for " +
                "new parameters added in more recent versions." )
    return


def new_config_version( ver ):
    """
        @brief A boilerplate note that the config file version being used is up to date.
    """

    logger = getLogger( mv.logger_name )

    logger.info( "Using configuration file for version " + ver + " of the script." +
                "This is the most recent version - no default values need to be assumed." )
    return
