/**********************************************************************\
 @file cosmology.hpp
 ------------------

 This file defines various functions which I find useful for
 astrophysical calculations. All are declared in the namespace IceBRG.

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

#ifndef ICEBRG_PHYSICS_COSMOLOGY_HPP_
#define ICEBRG_PHYSICS_COSMOLOGY_HPP_

#include "IceBRG_main/common.hpp"

#include "IceBRG_main/units/units.hpp"

namespace IceBRG
{

inverse_time_type H( const flt_t & z );
density_type rho_bar( flt_t const & z = 0. );

// Functions to work between redshift, scale factor, and time (in s, with zero = present day)
flt_t zfa( const flt_t & a );
flt_t afz( const flt_t & z );

time_type tfz( const flt_t & z );
time_type tfa( const flt_t & z );
flt_t zft( const time_type & t );
flt_t aft( const time_type & t );

time_type universe_age( const flt_t & z );

// Functions to integrate out distances
distance_type integrate_add( const flt_t & z1, const flt_t & z2 );
distance_type integrate_cmd( const flt_t & z1, const flt_t & z2 );
distance_type integrate_Ld( const flt_t & z1, const flt_t & z2 );
distance_type integrate_ltd( const flt_t & z1, const flt_t & z2 );
distance_type integrate_add( const flt_t & z );
distance_type integrate_cmd( const flt_t & z );
distance_type integrate_Ld( const flt_t & z );
distance_type integrate_ltd( const flt_t & z );
distance_type integrate_distance( const flt_t & z1, const flt_t & z2,
		const int_t & mode, const int_t & resolution = 10000 );

// Lensing functions
surface_density_type sigma_crit( const flt_t & z_lens, const flt_t & z_source );

} // namespace IceBRG


#endif // ICEBRG_PHYSICS_COSMOLOGY_HPP_
