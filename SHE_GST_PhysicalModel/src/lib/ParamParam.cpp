/**********************************************************************\
 @file ParamParam.cpp
 ------------------

 TODO <Insert file description here>

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

#include "SHE_GST_PhysicalModel/ParamParam.hpp"

#include <stdexcept>

#include <boost/bimap.hpp>

#include "SHE_GST_PhysicalModel/common.hpp"

namespace SHE_GST_PhysicalModel
{

// Initialization
template <typename L, typename R>
boost::bimap<L, R>
make_bimap(std::initializer_list<typename boost::bimap<L, R>::value_type> list)
{
    return boost::bimap<L, R>(list.begin(), list.end());
}

const boost::bimap<name_t,ParamParam::Mode> ParamParam::_mode_names =
		make_bimap<name_t,ParamParam::Mode>({
			{"independent", ParamParam::Mode::INDEPENDENT},
			{"dependent", ParamParam::Mode::DEPENDENT},
			{"alt_dependent", ParamParam::Mode::ALT_DEPENDENT},
			{"other", ParamParam::Mode::OTHER},
			{"unspecified", ParamParam::Mode::UNSPECIFIED},
		});

// Protected methods
ParamParam::Mode ParamParam::_get_mode_from_name( name_t name ) const
{
	boost::algorithm::to_lower(name);

	try
	{
		return _mode_names.left.at(name);
	}
	catch( const std::out_of_range & e)
	{
		throw bad_mode_error(name);
	}
}

name_t ParamParam::_get_name_from_mode( Mode const & mode ) const
{

	try
	{
		return _mode_names.right.at(mode);
	}
	catch( const std::out_of_range & e)
	{
		throw bad_mode_error(mode);
	}
}

} // namespace SHE_GST_PhysicalModel
