/**********************************************************************\
 @file ParamParam.hpp
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

#ifndef SHE_SIM_GAL_PARAMS_PARAMPARAM_HPP_
#define SHE_SIM_GAL_PARAMS_PARAMPARAM_HPP_

#include <string>
#include <stdexcept>
#include <initializer_list>

#include <boost/algorithm/string.hpp>
#include <boost/bimap.hpp>

#include "SHE_GST_PhysicalModel/common.hpp"
#include "SHE_GST_IceBRG_main/math/random/random_functions.hpp"

namespace SHE_GST_PhysicalModel
{

/**
 * TODO Auto-generated comment stub
 */
class ParamParam
{
public:

	/// Enum for mode. Deliberately not making it a scoped enum so that ints can be converted to it
	enum Mode
	{
		UNSPECIFIED = -1,
		INDEPENDENT = 0,
		DEPENDENT = 1,
		ALT_DEPENDENT = 2,
		OTHER = 3
	};

private:

	// Private members
	Mode _mode;
	static const boost::bimap<name_t,Mode> _mode_names;

protected:

	Mode _get_mode_from_name( name_t mode_str ) const;
	name_t _get_name_from_mode( Mode const & Mode ) const;

public:

	// Constructor and destructor

	ParamParam( Mode const & mode = UNSPECIFIED )
	: _mode(mode)
	{
	};

	ParamParam( name_t const & mode_name )
	: _mode(_get_mode_from_name( mode_name ))
	{
	};

	virtual ~ParamParam() {};

	// Comparisons
	virtual bool is_equal( ParamParam const * const & other ) const = 0;
	bool not_equal( ParamParam const * const & other ) const
	{
		return !is_equal(other);
	}

	// Public methods

	Mode const & get_mode() const noexcept
	{
		return _mode;
	}

	name_t get_mode_name() const
	{
		return _get_name_from_mode(_mode);
	}

	virtual name_t name() const = 0;

	virtual std::vector<flt_t> get_parameters() const = 0;

	virtual str_t get_parameters_string() const = 0;

	virtual flt_t get_independently( gen_t & gen=IceBRG::rng ) const = 0;

	virtual ParamParam * clone() const = 0;

	virtual ParamParam * recreate(const std::vector<flt_t> & params) const = 0;

};

class bad_mode_error : std::runtime_error
{
public:
	bad_mode_error()
	: std::runtime_error("Unrecognized ParamParam generation mode.")
	{
	}
	bad_mode_error(name_t const & name)
	: std::runtime_error("Unrecognized ParamParam generation mode: " + name)
	{
	}
	bad_mode_error(ParamParam::Mode const & mode)
	: std::runtime_error("Unrecognized ParamParam generation mode: " + mode)
	{
	}
	virtual ~bad_mode_error() noexcept {}

};

} // namespace SHE_GST_PhysicalModel

#endif // SHE_SIM_GAL_PARAMS_PARAMPARAM_HPP_
