/**********************************************************************\
 @file cosmology.hpp
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

#ifndef SHE_SIM_GAL_PARAMS_DEPENDENCY_FUNCTIONS_COSMOLOGY_HPP_
#define SHE_SIM_GAL_PARAMS_DEPENDENCY_FUNCTIONS_COSMOLOGY_HPP_

#include "SHE_GST_PhysicalModel/common.hpp"

namespace SHE_GST_PhysicalModel {

constexpr flt_t const Omega_m = 0.288; // WMAP9 + priors
constexpr flt_t const Omega_r = 0.000086; // WMAP9 + priors
constexpr flt_t const Omega_k = 0; // Assuming flat space
constexpr flt_t const Omega_l = 1 - Omega_k - Omega_m - Omega_r;
constexpr flt_t const Omega_b = 0.0472; // WMAP9 + priors

/**
 * Get a transverse distance in kpc from an angle in arcsec
 *
 * @param theta_arcsec angle in arcsec
 * @param z
 * @return distance in kpc
 */
flt_t get_distance_from_angle( const flt_t & theta_arcsec, const flt_t & z );

/**
 * Get an angle in arcsec from a transverse distance in kpc
 *
 * @param d_kpc transverse distance in kpc
 * @param z
 * @return angle in arcsec
 */
flt_t get_angle_from_distance( const flt_t & d_kpc, const flt_t & z );

/**
 * Get the ratio of the luminosity distance at redshift 1 to that at redshift 2.
 *
 * @param z1
 * @param z2
 * @return The ratio of Dl(z1) to Dl(z2)
 */
flt_t get_relative_luminosity_distance( const flt_t & z1, const flt_t & z2 );

flt_t get_apparent_magnitude_at_other_redshift( const flt_t & mag1, const flt_t & z1, const flt_t & z2);

} // namespace SHE_GST_PhysicalModel

#endif // SHE_SIM_GAL_PARAMS_DEPENDENCY_FUNCTIONS_COSMOLOGY_HPP_
