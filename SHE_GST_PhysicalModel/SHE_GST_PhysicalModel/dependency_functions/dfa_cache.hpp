/**********************************************************************\
 @file dfa_cache.hpp
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

#ifndef SHE_SIM_GAL_PARAMS_DEPENDENCY_FUNCTIONS_DFA_CACHE_HPP_
#define SHE_SIM_GAL_PARAMS_DEPENDENCY_FUNCTIONS_DFA_CACHE_HPP_

#include <utility>

#include <eigen3/Eigen/Core>

#include "SHE_GST_PhysicalModel/common.hpp"

namespace SHE_GST_PhysicalModel {

typedef Eigen::Array<flt_t,Eigen::Dynamic,1> dfa_array_t;
typedef std::pair< dfa_array_t, dfa_array_t > dfa_cache_t;

extern const dfa_cache_t dfa_cache;

dfa_cache_t load_dfa_cache();

} // namespace SHE_GST_PhysicalModel



#endif // SHE_SIM_GAL_PARAMS_DEPENDENCY_FUNCTIONS_DFA_CACHE_HPP_
