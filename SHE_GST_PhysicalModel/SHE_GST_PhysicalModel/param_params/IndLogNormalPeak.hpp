/**********************************************************************\
 @file IndLogNormalPeak.hpp
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

#ifndef SHE_SIM_GAL_PARAMS_PARAM_PARAMS_INDLOGNORMALPEAK_HPP_
#define SHE_SIM_GAL_PARAMS_PARAM_PARAMS_INDLOGNORMALPEAK_HPP_

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
class IndLogNormalPeak: public ParamParam
{
private:

	flt_t _l10_peak, _l10_stddev;

	// Private methods
	virtual bool is_equal( ParamParam const * const & other ) const override
	{
		IndLogNormalPeak const * other_derived = dynamic_cast<IndLogNormalPeak const *>(other);
		if(other_derived==nullptr) return false;
		return (_l10_peak==other_derived->_l10_peak) and (_l10_stddev==other_derived->_l10_stddev);
	}

public:

	// Constructor and destructor
	IndLogNormalPeak( flt_t const & l10_peak = 0., flt_t const & l10_stddev = 1. )
	: ParamParam(ParamParam::INDEPENDENT),
	  _l10_peak(l10_peak),
	  _l10_stddev(l10_stddev)
	{
	}
	virtual ~IndLogNormalPeak() {}

	// Get the name of this
	virtual name_t name() const override { return "lognormal_peak"; };

	virtual std::vector<flt_t> get_parameters() const override
	{
		return std::vector<flt_t>({_l10_peak,_l10_stddev});
	}

	virtual str_t get_parameters_string() const override
	{
		std::stringstream ss("");
		ss << "log_10(Peak): " << _l10_peak << ", "
				<< "sigma (log_10): " << _l10_stddev;
		return ss.str();
	}

	// Get the value
	virtual flt_t get_independently( gen_t & gen = IceBRG::rng ) const override
	{
		return std::pow(10.,IceBRG::Gaus_rand(_l10_peak,_l10_stddev,gen));
	}

	virtual ParamParam * clone() const override
	{
		return new IndLogNormalPeak(*this);
	}

	virtual ParamParam * recreate(const std::vector<flt_t> & params) const override
	{
		if(params.size() != 2) throw std::runtime_error("Invalid number of arguments used for lognormal_peak param param.\n"
				"Exactly 2 arguments are required.");
		return new IndLogNormalPeak(params[0],params[1]);
	}
};

} // namespace SHE_GST_PhysicalModel

#endif // SHE_SIM_GAL_PARAMS_PARAM_PARAMS_INDLOGNORMALPEAK_HPP_
