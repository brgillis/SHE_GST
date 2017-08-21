/**********************************************************************\
 @file galaxy_redshift.hpp
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

#ifndef SHE_SIM_GAL_PARAMS_DEPENDENCY_FUNCTIONS_GALAXY_REDSHIFT_HPP_
#define SHE_SIM_GAL_PARAMS_DEPENDENCY_FUNCTIONS_GALAXY_REDSHIFT_HPP_

#include "SHE_GST_PhysicalModel/common.hpp"
#include "SHE_GST_IceBRG_main/units/units.hpp"

namespace SHE_GST_PhysicalModel {

flt_t get_cluster_pz( flt_t const & z );

flt_t get_total_pz( flt_t const & z );

flt_t get_field_pz( flt_t const & z, flt_t const & total_enhancement,
		flt_t const & cluster_enhancement );

flt_t generate_cluster_z( flt_t const & z_min, flt_t const & z_max, gen_t & rng );

flt_t generate_field_z( flt_t const & total_enhancement,
		flt_t const & cluster_enhancement,
		flt_t const & z_min, flt_t const & z_max, gen_t & rng );

flt_t get_cluster_enhancement(flt_t const & cluster_density, flt_t const & z_min, flt_t const & z_max);

flt_t get_total_enhancement(flt_t const & total_density, flt_t const & z_min, flt_t const & z_max);

flt_t get_ex_num_cluster_galaxies(flt_t const & num_clusters, flt_t const & z_min, flt_t const & z_max);

} // namespace SHE_GST_PhysicalModel



#endif // SHE_SIM_GAL_PARAMS_DEPENDENCY_FUNCTIONS_GALAXY_REDSHIFT_HPP_
