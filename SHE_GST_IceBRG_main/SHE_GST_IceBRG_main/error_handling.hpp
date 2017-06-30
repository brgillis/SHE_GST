/**********************************************************************\
 @file error_handling.h
 ------------------

 TODO <Insert file description here>

 **********************************************************************

 Copyright (C) 2015 brg

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

\**********************************************************************/

#ifndef BRG_ERROR_HANDLING_H_
#define BRG_ERROR_HANDLING_H_

#include <iostream>
#include <stdexcept>
#include <string>

#include "SHE_GST_IceBRG_main/common.hpp"
#include "SHE_GST_IceBRG_main/globals.hpp"
#include "SHE_GST_IceBRG_main/logging.hpp"

namespace IceBRG {

/// Handle an error message
inline void handle_error(str_t const & str)
{
	switch (globals::error_behavior) {
		case error_behavior_type::THROW:
			throw std::runtime_error(str);
			break;
		case error_behavior_type::WARN:
			std::cerr << "WARNING: " << str << "\n";
			break;
		case error_behavior_type::LOG:
			ICEBRG_LOG_TRIVIAL(error) << str;
			break;
		case error_behavior_type::NOTHING:
			break;
		default:
			break;
	}
}

/// Handle a notification
inline void handle_notification(str_t const & str)
{
	switch (globals::error_behavior) {
		case error_behavior_type::THROW:
			std::cout << str << "\n";
			break;
		case error_behavior_type::WARN:
			std::cout << str << "\n";
			break;
		case error_behavior_type::LOG:
			ICEBRG_LOG_TRIVIAL(info) << str;
			break;
		case error_behavior_type::NOTHING:
			break;
		default:
			break;
	}
}

/// Handle an error message when throwing isn't an option
inline void handle_error_message(str_t const & str)
{
	switch (globals::error_behavior) {
		case error_behavior_type::THROW:
			std::cerr << "WARNING: " << str << "\n";
			break;
		case error_behavior_type::WARN:
			std::cerr << "WARNING: " << str << "\n";
			break;
		case error_behavior_type::LOG:
			ICEBRG_WARN_TRIVIAL() << str;
			break;
		case error_behavior_type::NOTHING:
			break;
		default:
			break;
	}
}

}

#endif // BRG_ERROR_HANDLING_H_
