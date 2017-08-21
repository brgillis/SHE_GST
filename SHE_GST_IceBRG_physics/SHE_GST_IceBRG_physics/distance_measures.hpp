/**********************************************************************\
 @file distance_measures.hpp
 ------------------

 This file includes various functions to get cosmological distance
 measures, and functions directly related to them.

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

#ifndef ICEBRG_PHYSICS_DISTANCE_MEASURES_HPP_
#define ICEBRG_PHYSICS_DISTANCE_MEASURES_HPP_

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/math/misc_math.hpp"
#include "SHE_GST_IceBRG_main/units/units.hpp"

namespace IceBRG
{

// Functions to get transverse distance (in m) from angle (in rad) or vice-versa
custom_unit_type<1,0,0,-1,0> dfa( const flt_t & z ); // Just gives conversion factor
distance_type dfa( const angle_type & da, const flt_t & z ); // Performs conversion
distance_type dfa( const angle_type & a1, const angle_type & a2,
		const flt_t & z ); // Performs conversion of dist between two angles
distance_type dfa( const angle_type & a1x, const angle_type & a1y,
		const angle_type & a2x, const angle_type & a2y, const flt_t & z ); // Performs conversion of dist between two positions

custom_unit_type<-1,0,0,1,0> afd( const flt_t & z );
angle_type afd( const distance_type & dd, const flt_t & z );
angle_type afd( const distance_type & d1, const distance_type & d2,
		const flt_t & z );
angle_type afd( const distance_type & d1x, const distance_type & d1y,
		const distance_type & d2x, const distance_type & d2y, const flt_t & z );

distance_type ad_distance( flt_t z1, flt_t z2 = 0 );
distance_type comoving_distance( flt_t z );
distance_type luminosity_distance( flt_t z );

volume_type comoving_volume_element( flt_t z );

// Like dist2d, but using corrections for spherical geometry
template< typename Tr1, typename Td1, typename Tr2, typename Td2 >
inline const Tr1 skydist2d( const Tr1 & ra1, const Td1 & dec1,
		const Tr2 & ra2, const Td2 & dec2 )
{
	return quad_add( ( ra2 - ra1 ) * cos( ( dec2 + dec1 ) / 2. ), dec2 - dec1 );
}

} // namespace IceBRG


#endif // ICEBRG_PHYSICS_DISTANCE_MEASURES_HPP_
