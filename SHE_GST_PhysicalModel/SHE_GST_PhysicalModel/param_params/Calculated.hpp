/**********************************************************************\
 @file Calculated.hpp
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

#ifndef SHE_SIM_GAL_PARAMS_PARAM_PARAMS_CALCULATED_HPP_
#define SHE_SIM_GAL_PARAMS_PARAM_PARAMS_CALCULATED_HPP_

#include <initializer_list>

#include "../../../SHE_GST_PhysicalModel/SHE_SIM/common.hpp"
#include "../../../SHE_GST_PhysicalModel/SHE_SIM/ParamParam.hpp"
#include "SHE_GST_IceBRG_main/math/random/random_functions.hpp"

namespace SHE_SIM
{

/**
 * TODO Auto-generated comment stub
 */
class Calculated: public ParamParam
{
public:

	// Private methods
	virtual bool is_equal( ParamParam const * const & other ) const override
	{
		Calculated const * other_derived = dynamic_cast<Calculated const *>(other);
		if(other_derived==nullptr) return false;
		return true;
	}

public:

	// Constructor and destructor
	Calculated()
	: ParamParam(ParamParam::DEPENDENT)
	{
	}
	virtual ~Calculated() {}

	// Get the name of this
	virtual name_t name() const override { return "calculated"; };

	virtual std::vector<flt_t> get_parameters() const override
	{
		return std::vector<flt_t>();
	}

	virtual str_t get_parameters_string() const override
	{
		return str_t("<None>");
	}

	// Get the value
	virtual flt_t get_independently( gen_t & = IceBRG::rng ) const override
	{
		throw std::logic_error("Calculated parameters cannot use the 'get_independently' method.");
	}

	virtual ParamParam * clone() const override
	{
		return new Calculated(*this);
	}

	virtual ParamParam * recreate(const std::vector<flt_t> & params) const override
	{
		if(params.size() != 0) throw std::runtime_error("Invalid number of arguments used for alt_calculated param param.\n"
				"Exactly 0 arguments are required.");
		return new Calculated();
	}
};

} // namespace SHE_SIM

#endif // SHE_SIM_GAL_PARAMS_PARAM_PARAMS_CALCULATED_HPP_
