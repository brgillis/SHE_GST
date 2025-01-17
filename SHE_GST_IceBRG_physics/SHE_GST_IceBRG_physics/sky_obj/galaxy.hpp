/**********************************************************************\
  @file galaxy.h

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

// body file: galaxy.cpp

#ifndef _BRG_GALAXY_H_
#define _BRG_GALAXY_H_

#include "SHE_GST_IceBRG_main/units/units.hpp"
#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_physics/sky_obj/detail/sky_obj.hpp"


namespace IceBRG {

// Forward declare galaxy_group class so we can point to it
class galaxy_group;

class galaxy: public virtual sky_obj
{
	/**********************************
	 galaxy class
	 -------------

	 A class for galaxies.

	 Parent class: sky_obj

	 **********************************/

private:
	// private member variables (none needed in current implementation. Should make all private for consistency in future though)

public:

	// Public member variables

	mass_type stellar_mass;
	flt_t umag, umag_err;
	flt_t gmag, gmag_err;
	flt_t rmag, rmag_err;
	flt_t imag, imag_err;
	flt_t zmag, zmag_err;

	flt_t z_phot, z_phot_err;
	flt_t odds;
	flt_t phot_template;

	galaxy_group *host_group; // Pointer to group this galaxy resides within
	int_t host_group_index;

	// Public member functions

	// Constructors
	galaxy( const angle_type & init_ra = 0, const angle_type & init_dec = 0, flt_t init_z = 0,
			const angle_type & init_ra_err = 0, const angle_type & init_dec_err = 0, flt_t init_z_err =
			0, const mass_type & init_stellar_mass=0,
			flt_t init_mag=0, flt_t init_mag_err=0 );

	// Copy constructor
	//galaxy(const galaxy other_galaxy); // Implicit is fine for us

	// Virtual destructor
	virtual ~galaxy()
	{
	}

	// Implementations of pure virtual functions from sky_obj
#if (1)
	mass_type m() const
	{
		return stellar_mass;
	}
	flt_t mag() const
	{
		return imag;
	}
#endif

	// Clear function
	virtual void clear();

	// Clone functions
	virtual redshift_obj *redshift_obj_clone() const
	{
		return new galaxy( *this );
	}
	virtual sky_obj *sky_obj_clone() const
	{
		return new galaxy( *this );
	}
	virtual galaxy *galaxy_clone() const
	{
		return new galaxy( *this );
	}

};
// class galaxy

} // end namespace IceBRG

#endif /* _BRG_GALAXY_H_ */
