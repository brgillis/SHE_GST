/**********************************************************************\
 @file cluster_visibility.hpp
 ------------------

 TODO <Insert file description here>

 **********************************************************************

 Copyright (C) 2016 brg

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

#ifndef ICEBRG_PHYSICS_CLUSTER_VISIBILITY_HPP_
#define ICEBRG_PHYSICS_CLUSTER_VISIBILITY_HPP_

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/units/units.hpp"
#include "SHE_GST_IceBRG_main/math/misc_math.hpp"

#include "SHE_GST_IceBRG_physics/luminosity.hpp"

namespace IceBRG
{

flt_t cluster_richness( mass_type const & mass, flt_t const & z,
		flt_t const & bright_abs_mag_i_lim = bright_abs_mag_i_max,
		flt_t const & faint_app_mag_i_lim = faint_app_mag_i_max );
mass_type min_cluster_mass( flt_t const & z,
		flt_t const & bright_abs_mag_i_lim = bright_abs_mag_i_max,
		flt_t const & faint_app_mag_i_lim = faint_app_mag_i_max );

/**
 * Get the number density of clusters at a given redshift in units of
 * number per square radian per unit redshift.
 *
 * @param z
 * @return
 */
inverse_square_angle_type cluster_angular_density_at_z(flt_t const & z);
flt_t visible_clusters( square_angle_type const & area, flt_t const & z1 = 0.1, flt_t const & z2 = 1.3 );

flt_t integrate_mean_cluster_richness_at_redshift( flt_t const & z );
flt_t integrate_mean_cluster_richness( flt_t const & z_min, flt_t const & z_max );
flt_t mean_cluster_richness_at_redshift( flt_t const & z );
flt_t mean_cluster_richness( flt_t const & z_min, flt_t const & z_max );

} // namespace IceBRG


#endif // ICEBRG_PHYSICS_CLUSTER_VISIBILITY_HPP_
