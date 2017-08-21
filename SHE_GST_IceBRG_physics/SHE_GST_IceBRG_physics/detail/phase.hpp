/**********************************************************************\
  @file phase.hpp

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

#ifndef _BRG_PHASE_HPP_INCLUDED_
#define _BRG_PHASE_HPP_INCLUDED_

#include "SHE_GST_IceBRG_main/common.hpp"

#ifdef _BRG_USE_UNITS_
#include "SHE_GST_IceBRG_main/physics/units/unit_obj.h"
#endif

namespace IceBRG
{

/** Class definitions **/
#if (1)
struct phase
/**********************************************
 phase
 -----

 A structure representing the full phase of an
 object (position and velocity_type) and also time_type,
 to help limit the number of variables that need
 to be passed to certain functions.

 All member variables are public, and may be
 accessed directly. Units should be in the
 default set: m for position, m/s for velocity_type,
 s for time_type.

 **********************************************/
{
	distance_type x, y, z;

	velocity_type vx, vy, vz;

	time_type t;
	phase( const distance_type & init_x = 0, const distance_type & init_y = 0,
			const distance_type & init_z = 0, const velocity_type & init_vx = 0,
			const velocity_type & init_vy = 0, const velocity_type & init_vz = 0,
	time_type init_t = 0 )
	{
		x = init_x;
		y = init_y;
		z = init_z;
		vx = init_vx;
		vy = init_vy;
		vz = init_vz;
		t = init_t;
	}
	const int_t set_phase( const distance_type & init_x = 0, const distance_type & init_y = 0,
			const distance_type & init_z = 0, const velocity_type & init_vx = 0,
			const velocity_type & init_vy = 0, const velocity_type & init_vz = 0,
	time_type init_t = 0 )
	{
		x = init_x;
		y = init_y;
		z = init_z;
		vx = init_vx;
		vy = init_vy;
		vz = init_vz;
		t = init_t;

		return 0;
	}
};

#endif // end class declarations

}

#endif // __BRG_PHASE_HPP_INCLUDED__
