/**********************************************************************\
 @file IndPoisson.hpp
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

#ifndef SHE_SIM_GAL_PARAMS_PARAM_PARAMS_INDPOISSON_HPP_
#define SHE_SIM_GAL_PARAMS_PARAM_PARAMS_INDPOISSON_HPP_

#include <initializer_list>
#include <sstream>

#include "SHE_GST_PhysicalModel/common.hpp"
#include "SHE_GST_PhysicalModel/ParamParam.hpp"
#include "SHE_GST_IceBRG_main/math/random/random_functions.hpp"

namespace SHE_GST_PhysicalModel
{

/**
 * TODO Auto-generated comment stub
 */
class IndPoisson: public ParamParam
{
private:

	flt_t _lambda;

	// Private methods
	virtual bool is_equal( ParamParam const * const & other ) const override
	{
		IndPoisson const * other_derived = dynamic_cast<IndPoisson const *>(other);
		if(other_derived==nullptr) return false;
		return (_lambda==other_derived->_lambda);
	}

public:

	// Constructor and destructor
	IndPoisson( flt_t const & lambda = 1. )
	: ParamParam(ParamParam::INDEPENDENT),
	  _lambda(lambda)

	{
	}
	virtual ~IndPoisson() {}

	// Get the name of this
	virtual name_t name() const override { return "poisson"; };

	virtual std::vector<flt_t> get_parameters() const override
	{
		return std::vector<flt_t>({_lambda});
	}

	virtual str_t get_parameters_string() const override
	{
		std::stringstream ss("");
		ss << "lambda: " << _lambda;
		return ss.str();
	}

	// Get the value
	virtual flt_t get_independently( gen_t & gen = IceBRG::rng ) const override
	{
		return IceBRG::Pois_rand(_lambda, gen);
	}

	virtual ParamParam * clone() const override
	{
		return new IndPoisson(*this);
	}

	virtual ParamParam * recreate(const std::vector<flt_t> & params) const override
	{
		if(params.size() != 16) throw std::runtime_error("Invalid number of arguments used for poisson param param.\n"
				"Exactly 1 argument is required.");
		return new IndPoisson(params[0]);
	}
};

} // namespace SHE_GST_PhysicalModel

#endif // SHE_SIM_GAL_PARAMS_PARAM_PARAMS_INDPOISSON_HPP_
