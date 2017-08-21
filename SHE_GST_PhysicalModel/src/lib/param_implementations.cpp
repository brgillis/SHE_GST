/**********************************************************************\
 @file random_functions.cpp
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

#include <utility>

#include "SHE_GST_PhysicalModel/param_params/Calculated.hpp"
#include "SHE_GST_PhysicalModel/param_params/DepFieldRedshift.hpp"
#include "SHE_GST_PhysicalModel/param_params/IndArcCos.hpp"
#include "SHE_GST_PhysicalModel/param_params/IndClusterRedshift.hpp"
#include "SHE_GST_PhysicalModel/param_params/IndContRayleigh.hpp"
#include "SHE_GST_PhysicalModel/param_params/IndFixed.hpp"
#include "SHE_GST_PhysicalModel/param_params/IndLogNormalMean.hpp"
#include "SHE_GST_PhysicalModel/param_params/IndTruncLogNormalMean.hpp"
#include "SHE_GST_PhysicalModel/param_params/IndUniform.hpp"
#include "SHE_GST_PhysicalModel/ParamGenerator.hpp"
#include "SHE_GST_PhysicalModel/ParamHierarchyLevel.hpp"
#include "SHE_GST_PhysicalModel/ParamParam.hpp"
#include "SHE_GST_PhysicalModel/default_param_params.hpp"
#include "SHE_GST_PhysicalModel/default_values.hpp"
#include "SHE_GST_PhysicalModel/dependency_functions/cosmology.hpp"
#include "SHE_GST_PhysicalModel/dependency_functions/galaxy_redshift.hpp"
#include "SHE_GST_PhysicalModel/dependency_functions/galaxy_type.hpp"
#include "SHE_GST_PhysicalModel/dependency_functions/halos.hpp"
#include "SHE_GST_PhysicalModel/dependency_functions/misc_dependencies.hpp"
#include "SHE_GST_PhysicalModel/dependency_functions/morphology.hpp"
#include "SHE_GST_PhysicalModel/param_declarations.hpp"
#include "SHE_GST_IceBRG_main/math/misc_math.hpp"
#include "SHE_GST_IceBRG_main/units/unit_conversions.hpp"
#include "SHE_GST_IceBRG_physics/cluster_visibility.hpp"
#include "SHE_GST_IceBRG_physics/galaxy_visibility.hpp"


namespace SHE_GST_PhysicalModel {

// Implement default maps

params_t default_params_map;
param_params_t default_param_params_map;
generation_level_map_t default_generation_levels_map;

// Functions to help insert objects into maps

template<typename T_in, typename... Args>
void insert_default_param_param(const name_t & param_name,
		level_t const & gen_level,
		Args... args)
{
	typename param_params_t::mapped_type new_ptr(new T_in(args...));

	default_param_params_map.insert(std::make_pair(std::move(param_name),std::move(new_ptr)));
	default_generation_levels_map.insert(std::make_pair(param_name,
			level_ptr_t(new level_t(gen_level))));
}

// Macros to simplify adding param params to the default maps, using the attach by initialization
// idiom.
#define IMPLEMENT_PARAM(param, \
			            level, \
                        param_params,\
                        dependent_generation, \
						alt_dependent_generation) \
 \
const name_t param##_name = #param; \
 \
void param##_obj::_generate() \
{ \
	if(_p_params->get_mode()==ParamParam::DEPENDENT) \
	{ \
		dependent_generation; \
	} \
	else if(_p_params->get_mode()==ParamParam::ALT_DEPENDENT) \
	{ \
		alt_dependent_generation; \
	} \
	else if(_p_params->get_mode()==ParamParam::INDEPENDENT) \
	{ \
		_cached_value = _p_params->get_independently(get_rng()); \
	} \
	else \
	{ \
		throw bad_mode_error(_p_params->get_mode_name()); \
	} \
} \
param##_obj::param##_obj( owner_t * const & p_owner) \
: ParamGenerator(p_owner) \
{ \
	/* See if we can get generation level and params from the parent */ \
	auto p_parent_version = _p_parent_version(); \
	if(p_parent_version) \
	{ \
		_p_generation_level = p_parent_version->get_p_generation_level(); \
		_p_params = p_parent_version->get_p_params(); \
	} \
	else \
	{ \
		_p_params = default_param_params_map.at(name()).get(); \
		_p_generation_level = default_generation_levels_map.at(name()).get(); \
	} \
} \
 \
struct param##_initializer \
{ \
	param##_initializer() \
	{ \
		default_generation_levels_map.insert(std::make_pair(param##_name, \
				level_ptr_t(new level_t(level)))); \
	     \
		typename param_params_t::mapped_type new_ptr(new param_params); \
		default_param_params_map.insert(std::make_pair(param##_name, \
				std::move(new_ptr))); \
		 \
		default_params_map.insert(std::make_pair(param##_name, \
			param_ptr_t(new param##_obj(nullptr)))); \
		 \
	} \
}; \
\
param##_initializer param##_initializer_instance;

#define REQUEST(param) _request_param_value(param##_name)

using namespace IceBRG;

#include "param_implementation_detail/high_level_param_implementations.hh"

#include "param_implementation_detail/mid_level_param_implementations.hh"

#include "param_implementation_detail/galaxy_level_param_implementations.hh"

} // namespace SHE_GST_PhysicalModel
