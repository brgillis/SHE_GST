/**********************************************************************\
 @file mass_function.hpp
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

#ifndef ICEBRG_PHYSICS_MASS_FUNCTION_HPP_
#define ICEBRG_PHYSICS_MASS_FUNCTION_HPP_

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/units/units.hpp"

namespace IceBRG
{

// Mass function constants

constexpr flt_t mass_func_l10_min = 8;
constexpr flt_t mass_func_l10_max = 16;

// Functions

flt_t unnormed_power_spectrum( inverse_distance_type const & k);

flt_t delta_c( flt_t const & z = 0. );

distance_type r_of_m( mass_type const & mass );

flt_t sigma_of_r( distance_type const & r );
flt_t sigma_of_m( mass_type const & mass );

flt_t nu_of_m( mass_type const & mass, flt_t const & z = 0. );

inverse_volume_inverse_mass_type mass_function( mass_type const & mass, flt_t const & z = 0. );
inverse_volume_type log10_mass_function( flt_t const & log10msun_mass, flt_t const & z = 0. );
inverse_volume_type integrated_log10_mass_function( flt_t const & l10_m_lo, flt_t const & l10_m_hi,
		flt_t const & z = 0. );

} // namespace IceBRG

#endif // ICEBRG_PHYSICS_MASS_FUNCTION_HPP_
