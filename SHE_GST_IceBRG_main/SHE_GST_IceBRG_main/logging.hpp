/**********************************************************************\
 @file logging.hpp
 ------------------

 Handling for logging in multiple ways.

 **********************************************************************

 Copyright (C) 2012-2020 Euclid Science Ground Segment

 This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General
 Public License as published by the Free Software Foundation; either version 3.0 of the License, or (at your option)
 any later version.

 This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

\**********************************************************************/


#ifndef SHE_GST_ICEBRG_MAIN_SHE_GST_ICEBRG_MAIN_LOGGING_HPP_
#define SHE_GST_ICEBRG_MAIN_SHE_GST_ICEBRG_MAIN_LOGGING_HPP_

#define LOGGER_NAME "IceBRG"

#if defined(ELEMENTS_LINKER_LIBRARY) or defined(_FORTIFY_SOURCE) or defined(_GNU_SOURCE)

#include "ElementsKernel/Logging.h"

#define ICEBRG_GET_LOGGER(program) \
    Elements::Logging::getLogger(#program)

#define ICEBRG_LOG_TRIVIAL(mode) \
  Elements::Logging::getLogger(LOGGER_NAME).mode()
#define ICEBRG_LOG(mode,program) \
  Elements::Logging::getLogger(#program).mode()

#define ICEBRG_WARN_TRIVIAL() \
  Elements::Logging::getLogger(LOGGER_NAME).warn()
#define ICEBRG_WARN(program) \
  Elements::Logging::getLogger(#program).warn()

#else

#define BOOST_LOG_DYN_LINK 1

#include <boost/log/trivial.hpp>

#define ICEBRG_GET_LOGGER(program) \
    std::cout

#define ICEBRG_LOG_TRIVIAL(mode) \
  BOOST_LOG_TRIVIAL(mode)
#define ICEBRG_LOG(mode,program) \
  BOOST_LOG_TRIVIAL(mode)

#define ICEBRG_WARN_TRIVIAL() \
  BOOST_LOG_TRIVIAL(warning)
#define ICEBRG_WARN(program) \
  BOOST_LOG_TRIVIAL(warning)

#endif

#endif /* ifndef SHE_GST_ICEBRG_MAIN_SHE_GST_ICEBRG_MAIN_LOGGING_HPP_ */
