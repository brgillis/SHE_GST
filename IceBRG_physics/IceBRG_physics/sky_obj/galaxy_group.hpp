/**********************************************************************\
  @file galaxy_group.h

 **********************************************************************

 Copyright (C) 2014  Bryan R. Gillis

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

// body file: galaxy_group.cpp

#ifndef _BRG_GALAXY_GROUP_H_
#define _BRG_GALAXY_GROUP_H_

#include <vector>

#include "IceBRG_main/common.hpp"

#include "IceBRG_physics/sky_obj/detail/sky_obj.hpp"
#include "IceBRG_physics/sky_obj/galaxy.hpp"

namespace IceBRG {


class galaxy_group: public sky_obj
{
	/**********************************
	 group class
	 -------------

	 A class for groups and clusters of galaxies.

	 Parent class: sky_obj

	 Derived classes:
	 (none)

	 **********************************/

private:

public:
	// Public member variables

	flt_t z_phot, z_phot_err;
	flt_t odds;

	size_t BCG_index;

	size_t num_members;
	std::vector< size_t > member_indices;
	std::vector< galaxy * > members;

	// Public member functions

	// Constructor
	galaxy_group( size_t init_num_members = 0 );
	galaxy_group( flt_t init_mass, flt_t init_z, flt_t init_c = -1,
			flt_t init_tau = -1 );

	// Copy constructor
	//group( const group &other ); // Implicit is fine

	// Destructor
	virtual ~galaxy_group();

	// Functions to set parameters
	virtual void clear();
	virtual void resize( size_t new_num_members );
	virtual void set_member( size_t index, galaxy * new_member );
	virtual void set_member_index( size_t index, size_t new_member_index );
	virtual void add_member( galaxy * new_member =
			nullptr );
	virtual void remove_member( galaxy * rem_member =
			nullptr );

	// Implementations of pure virtual functions from sky_obj
#if (1)
	mass_type m() const
	{
		return 0; // TODO: Should be combine magnitude of member galaxies
	}
	flt_t mag() const
	{
		return 0; // TODO: Should be combine magnitude of member galaxies
	}
#endif

	// Clone functions
	virtual redshift_obj *redshift_obj_clone() const
	{
		return new galaxy_group( *this );
	}
	virtual sky_obj *sky_obj_clone() const
	{
		return new galaxy_group( *this );
	}
	virtual galaxy_group *galaxy_group_clone() const
	{
		return new galaxy_group( *this );
	}

};
// class galaxy_group

} // end namespace IceBRG

#endif /* _BRG_GALAXYGROUP_H_ */
