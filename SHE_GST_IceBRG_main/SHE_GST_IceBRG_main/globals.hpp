/**********************************************************************\
 @file globals.hpp
 ------------------

 A class to contain global information for a program's run.

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

#ifndef ICEBRG_MAIN_GLOBALS_HPP_
#define ICEBRG_MAIN_GLOBALS_HPP_

#include <unordered_map>

#include "SHE_GST_IceBRG_main/common.hpp"

namespace IceBRG {

/// Error behavior enum
enum class error_behavior_type
{
	THROW,
	WARN,
	LOG,
	NOTHING
};

struct globals
{
	// General globals, to be present in any program

	/// Working directory
	static str_t workdir;

	/// Program name
	static str_t program_name;

	/// Error handling behaviour
	static error_behavior_type error_behavior;

};

} // namespace IceBRG



#endif // ICEBRG_MAIN_GLOBALS_HPP_
