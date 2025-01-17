/**********************************************************************\
 @file galaxy_physical_size_interpolation.hpp
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

#ifndef SHE_SIM_GAL_PARAMS_DEPENDENCY_FUNCTIONS_GALAXY_SIZE_DETAIL_HPP_
#define SHE_SIM_GAL_PARAMS_DEPENDENCY_FUNCTIONS_GALAXY_SIZE_DETAIL_HPP_

#include <utility>

#include <eigen3/Eigen/Core>

#include "SHE_GST_PhysicalModel/common.hpp"

namespace SHE_GST_PhysicalModel {

typedef Eigen::Array<flt_t,Eigen::Dynamic,1> gal_size_array_t;
typedef std::tuple< gal_size_array_t, gal_size_array_t, gal_size_array_t > med_gal_size_cache_t;
typedef std::tuple< gal_size_array_t, gal_size_array_t > gal_size_scatter_cache_t;

extern const med_gal_size_cache_t med_gal_size_bulge_cache;
extern const med_gal_size_cache_t med_gal_size_disk_cache;
extern const gal_size_scatter_cache_t gal_size_scatter_bulge_cache;
extern const gal_size_scatter_cache_t gal_size_scatter_disk_cache;

med_gal_size_cache_t load_med_gal_size_bulge_cache();
med_gal_size_cache_t load_med_gal_size_disk_cache();
gal_size_scatter_cache_t load_gal_size_scatter_bulge_cache();
gal_size_scatter_cache_t load_gal_size_scatter_disk_cache();

flt_t generate_physical_size( flt_t const & redshift,
		flt_t const & stellar_mass,
		med_gal_size_cache_t const & med_gal_size_cache,
		gal_size_scatter_cache_t const & gal_size_scatter_cache,
		gen_t & rng );

} // namespace SHE_GST_PhysicalModel



#endif // SHE_SIM_GAL_PARAMS_DEPENDENCY_FUNCTIONS_GALAXY_SIZE_DETAIL_HPP_
