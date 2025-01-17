/**********************************************************************\
  @file sky_obj.cpp

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


#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/units/units.hpp"

#include "SHE_GST_IceBRG_physics/distance_measures.hpp"
#include "SHE_GST_IceBRG_physics/sky_obj/detail/sky_obj.hpp"

// IceBRG::sky_obj class methods
#if (1)

IceBRG::sky_obj::sky_obj( const angle_type & init_ra, const angle_type & init_dec,
		const flt_t & init_z, const angle_type & init_ra_err,
		const angle_type & init_dec_err, const flt_t & init_z_err )
{
	partial_clear();
	set_ra_dec_z_err( init_ra, init_dec, init_z, init_ra_err, init_dec_err,
			init_z_err );
}

void IceBRG::sky_obj::clear()
{
	set_ra_dec_z_err( 0, 0, 0, 0, 0, 0 );
	return partial_clear();
}

void IceBRG::sky_obj::partial_clear()
{
	_index_ = 0;
	_ID_ = "0";
	_weight_ = 1;
}

void IceBRG::sky_obj::set_ra( const angle_type & new_ra )
{
	_ra_ = new_ra;
}
void IceBRG::sky_obj::set_ra_err( const angle_type & new_ra_err )
{
	_ra_err_ = new_ra_err;
}
void IceBRG::sky_obj::set_dec( const angle_type & new_dec )
{
	_dec_ = new_dec;
}
void IceBRG::sky_obj::set_dec_err( const angle_type & new_dec_err )
{
	_dec_err_ = new_dec_err;
}
void IceBRG::sky_obj::set_ra_dec( const angle_type & new_ra,
		const angle_type & new_dec )
{
	set_ra( new_ra );
	set_dec( new_dec );
}
void IceBRG::sky_obj::set_ra_dec_z( const angle_type & new_ra,
		const angle_type & new_dec, const flt_t & new_z )
{
	set_ra( new_ra );
	set_dec( new_dec );
	set_z( new_z );
}
void IceBRG::sky_obj::set_ra_dec_z_err( const angle_type & new_ra,
		const angle_type & new_dec, const flt_t & new_z,
		const angle_type & new_ra_err, const angle_type & new_dec_err,
		const flt_t & new_z_err )
{
	set_ra( new_ra );
	set_dec( new_dec );
	set_z( new_z );
	set_ra_err( new_ra_err );
	set_dec_err( new_dec_err );
	set_z_err( new_z_err );
}
void IceBRG::sky_obj::set_ra_dec_err( const angle_type & new_ra,
		const angle_type & new_dec, const angle_type & new_ra_err,
		const angle_type & new_dec_err )
{
	set_ra( new_ra );
	set_dec( new_dec );
	set_ra_err( new_ra_err );
	set_dec_err( new_dec_err );
}
void IceBRG::sky_obj::set_weight( const flt_t & new_weight )
{
	_weight_ = new_weight;
}
void IceBRG::sky_obj::set_index( const int_t new_index )
{
	_index_ = new_index;
}
void IceBRG::sky_obj::set_ID( const std::string &new_ID )
{
	_ID_ = new_ID;
}

#endif // end IceBRG::sky_obj class methods

IceBRG::distance_type IceBRG::dfa( const IceBRG::sky_obj *obj1,
		const IceBRG::sky_obj *obj2, const flt_t & z )
{
	flt_t z_to_use;
	if ( z == -1 )
		z_to_use = obj1->z();
	else
		z_to_use = z;
	return IceBRG::dfa(
			skydist2d( obj1->ra(), obj1->dec(), obj2->ra(), obj2->dec() ),
			z_to_use );
}

IceBRG::angle_type IceBRG::skydist2d( const IceBRG::sky_obj *obj1, const IceBRG::sky_obj *obj2 )
{
	return skydist2d(obj1->ra(),obj1->dec(),obj2->ra(),obj2->dec());
}
