/**********************************************************************\
 @file luminosity.hpp
 ------------------

 This file contains functions related to luminosit in various ways.

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

#ifndef ICEBRG_PHYSICS_LUMINOSITY_HPP_
#define ICEBRG_PHYSICS_LUMINOSITY_HPP_

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/units/units.hpp"

namespace IceBRG
{

// Luminosity function constants

constexpr flt_t bright_abs_mag_i_max = -19.5;
constexpr flt_t faint_app_mag_i_max = 29;

constexpr flt_t lum_func_min_abs_mag_B = -25;
constexpr flt_t lum_func_max_abs_mag_B = -11;

// Function declarations

flt_t get_abs_mag_from_app_mag( flt_t const & app_mag, flt_t const & z );

flt_t get_app_mag_from_abs_mag( flt_t const & abs_mag, flt_t const & z );

inverse_volume_type differential_luminosity_function( flt_t const & mag_B );

inverse_volume_type integrated_luminosity_function( flt_t const & mag_B_lo, flt_t const & mag_B_hi );

flt_t faint_bright_ratio( flt_t const & z, flt_t const & bright_abs_mag_i_lim = bright_abs_mag_i_max,
		flt_t const & faint_app_mag_i_lim = faint_app_mag_i_max);

// Taken from regression of CFHTLenS data. Scatter of 1.2972528920157342
flt_t estimate_abs_mag_g_from_stellar_mass( mass_type const & stellar_mass );

// Taken from regression of CFHTLenS data. Log10 scatter of 0.585461144291201
mass_type estimate_stellar_mass_from_abs_mag_g( flt_t const & abs_mag_g );

// Taken from regression of CFHTLenS data. Scatter of 0.93030163251098796
flt_t estimate_abs_mag_i_from_stellar_mass( mass_type const & stellar_mass );

// Taken from regression of CFHTLenS data. Log10 scatter of 0.41169238482277282
mass_type estimate_stellar_mass_from_abs_mag_i( flt_t const & abs_mag_g );

// Combining the above two regressions
flt_t estimate_abs_mag_g_from_abs_mag_i( flt_t const & abs_mag_i );
flt_t estimate_abs_mag_i_from_abs_mag_g( flt_t const & abs_mag_g );

// Taken from zeropoints in Lance's code
flt_t estimate_abs_mag_vis_from_abs_mag_i( flt_t const & abs_mag_i );
flt_t estimate_abs_mag_i_from_abs_mag_vis( flt_t const & abs_mag_vis );

// More ways to use mag_vis
flt_t estimate_abs_mag_vis_from_stellar_mass( mass_type const & stellar_mass );
mass_type estimate_stellar_mass_from_abs_mag_vis( flt_t const & abs_mag_vis );
flt_t estimate_abs_mag_g_from_abs_mag_vis( flt_t const & abs_mag_vis );
flt_t estimate_abs_mag_vis_from_abs_mag_g( flt_t const & abs_mag_g );


} // namespace IceBRG


#endif // ICEBRG_PHYSICS_LUMINOSITY_HPP_
