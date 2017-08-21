/**********************************************************************\
 @file galaxy_visibility.hpp
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

#ifndef ICEBRG_PHYSICS_GALAXY_VISIBILITY_HPP_
#define ICEBRG_PHYSICS_GALAXY_VISIBILITY_HPP_

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/units/units.hpp"
#include "SHE_GST_IceBRG_main/math/misc_math.hpp"

namespace IceBRG
{

flt_t max_galaxy_abs_mag_B( flt_t const & z,
		flt_t const & faint_app_mag_i_lim = faint_app_mag_i_max );

/**
 * Get the number density of galaxies at a given redshift in units of
 * number per square radian per unit redshift.
 *
 * @param z
 * @return
 */
inverse_square_angle_type galaxy_angular_density_at_z(flt_t const & z);
flt_t visible_galaxies( square_angle_type const & area, flt_t const & z1 = 0.1, flt_t const & z2 = 2.0 );

} // namespace IceBRG





#endif // ICEBRG_PHYSICS_GALAXY_VISIBILITY_HPP_
