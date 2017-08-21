/**********************************************************************\
  @file density_profile_functors.h

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

// body file: brg_physics/astro/density_profile/density_profile_functors.cpp

#ifndef _BRG_DENSITY_PROFILE_FUNCTORS_H_
#define _BRG_DENSITY_PROFILE_FUNCTORS_H_

#include "SHE_GST_IceBRG_main/units/units.hpp"
#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_physics/density_profile/detail/density_profile.hpp"

namespace IceBRG {

class accel_functor
{
	/**********************************
	 accel_functor class
	 -----------------------------

	 Function class for acceleration_type within a density_type profile

	 Parent class: function_class (from brg_functors)

	 **********************************/
private:
	const density_profile *_host_ptr_;
public:

	void set_host_ptr( const density_profile *new_host_ptr );
	const density_profile * host_ptr()
	{
		return _host_ptr_;
	}

	acceleration_type operator()( const distance_type &  in_param ) const;

	accel_functor();
	accel_functor( const density_profile *init_host_ptr );
	virtual ~accel_functor()
	{
	}
};
// class accel_functor

class solve_rhm_functor
{
	/**********************************
	 solve_rhm_functor class
	 -----------------------------

	 Function class for solving the half-mass_type
	 radius of a halo.

	 Parent class: None

	 **********************************/
private:
	const density_profile * _host_ptr_;
	mass_type _target_mass_;

public:

	void set_host_ptr( const density_profile * new_host_ptr );
	const density_profile * host_ptr()
	{
		return _host_ptr_;
	}
	void set_target_mass( mass_type const & new_target_mass );
	mass_type const & target_mass()
	{
		return _target_mass_;
	}

	mass_type operator ()( distance_type const & in_param ) const;

	solve_rhm_functor();
	solve_rhm_functor( const density_profile *init_host,
			mass_type const & init_target_mass );

};
// end class unitless_solve_rhm_functor

class spherical_density_functor
{
	/**********************************
	 spherical_density_functor class
	 -----------------------------

	 Function class integrating density_type in a sphere

	 Parent class: function_class (from brg_functors)

	 **********************************/
private:
	const density_profile *_host_ptr_;

public:

	void set_host_ptr( const density_profile *new_host_ptr );
	const density_profile * host_ptr()
	{
		return _host_ptr_;
	}

	custom_unit_type<-1,0,1,0,0> operator()( const distance_type & in_param ) const;

	spherical_density_functor();
	spherical_density_functor( const density_profile *init_host );
	virtual ~spherical_density_functor()
	{
	}
};

} // namespace IceBRG

#endif /* _BRG_DENSITY_PROFILE_FUNCTORS_H_ */
