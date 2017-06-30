/**********************************************************************\
 @file param_params_list.hpp
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

#ifndef SHE_SIM_GAL_PARAMS_MAKE_PARAM_PARAMS_MAP_HPP_
#define SHE_SIM_GAL_PARAMS_MAKE_PARAM_PARAMS_MAP_HPP_

#include <memory>
#include <unordered_map>
#include <utility>

#include "../SHE_GST_PhysicalModel/param_params/AltCalculated.hpp"
#include "../SHE_GST_PhysicalModel/param_params/Calculated.hpp"
#include "../SHE_GST_PhysicalModel/param_params/DepFieldRedshift.hpp"
#include "../SHE_GST_PhysicalModel/param_params/DepUniform.hpp"
#include "../SHE_GST_PhysicalModel/param_params/IndArcCos.hpp"
#include "../SHE_GST_PhysicalModel/param_params/IndContRayleigh.hpp"
#include "../SHE_GST_PhysicalModel/param_params/IndExpQuadratic.hpp"
#include "../SHE_GST_PhysicalModel/param_params/IndFixed.hpp"
#include "../SHE_GST_PhysicalModel/param_params/IndGaussian.hpp"
#include "../SHE_GST_PhysicalModel/param_params/IndLogNormalMean.hpp"
#include "../SHE_GST_PhysicalModel/param_params/IndLogNormalPeak.hpp"
#include "../SHE_GST_PhysicalModel/param_params/IndPoisson.hpp"
#include "../SHE_GST_PhysicalModel/param_params/IndRayleigh.hpp"
#include "../SHE_GST_PhysicalModel/param_params/IndTruncGaussian.hpp"
#include "../SHE_GST_PhysicalModel/param_params/IndTruncLogNormalMean.hpp"
#include "../SHE_GST_PhysicalModel/param_params/IndTruncLogNormalPeak.hpp"
#include "../SHE_GST_PhysicalModel/param_params/IndTruncRayleigh.hpp"
#include "../SHE_GST_PhysicalModel/param_params/IndUniform.hpp"

#include "../../SHE_GST_PhysicalModel/SHE_SIM/common.hpp"
#include "../../SHE_GST_PhysicalModel/SHE_SIM/ParamParam.hpp"
#include "../../SHE_GST_PhysicalModel/SHE_SIM/ParamParam.hpp"

namespace SHE_SIM {

extern const param_params_t param_params_map;

template<typename T_in>
void insert_param_param(param_params_t & res)
{
	typename param_params_t::mapped_type new_ptr(new T_in);
	auto name(new_ptr->name());

	res.insert(std::make_pair(std::move(name),std::move(new_ptr)));
}

// Function to get a list of all params
inline param_params_t make_full_param_params_map()
{
	param_params_t res;

	// Insert all param_params here
	insert_param_param<AltCalculated>(res);
	insert_param_param<Calculated>(res);
	insert_param_param<DepFieldRedshift>(res);
	insert_param_param<DepUniform>(res);
  insert_param_param<IndArcCos>(res);
	insert_param_param<IndExpQuadratic>(res);
	insert_param_param<IndFixed>(res);
	insert_param_param<IndContRayleigh>(res);
	insert_param_param<IndGaussian>(res);
	insert_param_param<IndLogNormalPeak>(res);
	insert_param_param<IndLogNormalMean>(res);
	insert_param_param<IndPoisson>(res);
	insert_param_param<IndRayleigh>(res);
	insert_param_param<IndTruncGaussian>(res);
	insert_param_param<IndTruncLogNormalPeak>(res);
	insert_param_param<IndTruncLogNormalMean>(res);
	insert_param_param<IndTruncRayleigh>(res);
	insert_param_param<IndUniform>(res);

	return res;

} // param_params_t get_full_param_params_map()

} // namespace SHE_SIM

#endif // SHE_SIM_GAL_PARAMS_MAKE_PARAM_PARAMS_MAP_HPP_
