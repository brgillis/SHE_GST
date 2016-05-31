/**********************************************************************\
 @file IndContRayleigh.hpp
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

#ifndef SHE_SIM_GAL_PARAMS_PARAM_PARAMS_INDCONTRAYLEIGH_HPP_
#define SHE_SIM_GAL_PARAMS_PARAM_PARAMS_INDCONTRAYLEIGH_HPP_

#include <stdexcept>
#include <initializer_list>
#include <sstream>

#include "SHE_SIM/common.hpp"
#include "SHE_SIM/ParamParam.hpp"
#include "IceBRG_main/math/random/random_functions.hpp"

namespace SHE_SIM
{

/**
 * TODO Auto-generated comment stub
 */
class IndContRayleigh: public ParamParam
{
private:

	// Private members
	flt_t _sigma, _max, _p;

	// Private methods
	virtual bool is_equal( ParamParam const * const & other ) const override
	{
		IndContRayleigh const * other_derived = dynamic_cast<IndContRayleigh const *>(other);
		if(other_derived==nullptr) return false;
		return (_sigma==other_derived->_sigma) and (_max==other_derived->_max) and
				(_p==other_derived->_p);
	}

public:

	// Constructor and destructor
	IndContRayleigh( flt_t const & sigma = 0., flt_t const & max = 1., flt_t const & p = 4. )
	: ParamParam(ParamParam::INDEPENDENT),
	  _sigma(sigma),
	  _max(max),
	  _p(p)
	{
	}
	virtual ~IndContRayleigh() {}

	// Get the name of this
	virtual name_t name() const override { return "contracted_rayleigh"; };

	virtual std::vector<flt_t> get_parameters() const override
	{
		return std::vector<flt_t>({_sigma,_max,_p});
	}

	virtual str_t get_parameters_string() const override
	{
		std::stringstream ss("");
		ss << "sigma: " << _sigma << ", "
				<< "Max: " << _max  << ", "
				<< "p: " << _p;
		return ss.str();
	}

	// Get the value
	virtual flt_t get_independently( gen_t & gen = IceBRG::rng ) const override
	{
		return IceBRG::contracted_Rayleigh_rand(_sigma,_max,_p,gen);
	}

	virtual ParamParam * clone() const override
	{
		return new IndContRayleigh(*this);
	}

	virtual ParamParam * recreate(const std::vector<flt_t> & params) const override
	{
		if(params.size() != 3) throw std::runtime_error("Invalid number of arguments used for contracted_rayleigh param param.\n"
				"Exactly 3 arguments are required.");
		return new IndContRayleigh(params[0],params[1],params[2]);
	}
};

} // namespace SHE_SIM

#endif // SHE_SIM_GAL_PARAMS_PARAM_PARAMS_INDCONTRAYLEIGH_HPP_
