/**********************************************************************\
 @file redshift_obj.h
 ------------------

 TODO <Insert file description here>

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

#ifndef ICEBRG_PHYSICS_REDSHIFT_OBJ_HPP_
#define ICEBRG_PHYSICS_REDSHIFT_OBJ_HPP_


#include "SHE_GST_IceBRG_main/common.hpp"
#include "SHE_GST_IceBRG_main/math/misc_math.hpp"
#include "SHE_GST_IceBRG_main/units/units.hpp"

#include "SHE_GST_IceBRG_physics/constants.hpp"
#include "SHE_GST_IceBRG_physics/cosmology.hpp"

namespace IceBRG {

class redshift_obj
{
	/**********************************
	 redshift_obj class
	 ------------------

	 A base class for anything with a redshift.

	 Derived classes:

	 sky_obj
	 galaxy
	 group
	 density_profile
	 tNFW_profile
	 point_mass_profile

	 **********************************/
private:
	flt_t _z_, _z_err_;
	mutable inverse_time_type _H_cache_;
	mutable bool _H_cached_;
public:

	// Constructor
	redshift_obj( const flt_t & init_z = 0, const flt_t & init_z_err = 0 )
	: _z_(init_z),
	  _z_err_(init_z_err),
	  _H_cache_(0),
	  _H_cached_(false)
	{
	}

	// Copy constructor
	// (implicit is fine for us)

	// Virtual destructor
	virtual ~redshift_obj()
	{
	}

	// Set functions
#if (1)
	virtual void set_z( const flt_t & new_z ) // Sets z
	{
		_z_ = new_z;
		_H_cached_ = false;
	}
	virtual void set_z_err( const flt_t & new_z_err ) // Sets z error
	{
		_z_err_ = new_z_err;
	}
#endif

	// Get functions
#if (1)
	flt_t z() const
	{
		return _z_;
	}
	flt_t z_err() const
	{
		return _z_err_;
	}
#endif

	// Astro calculations

#if (1)
	// H at the redshift of the object, given in units of m/s^2
	inverse_time_type H() const
	{
		// If not cached, calculate and cache it
		if(!_H_cached_)
		{
			if(_z_==0)
			{
				_H_cache_ = H_0;
			}
			else
			{
				_H_cache_ = IceBRG::H(_z_);
			}
			_H_cached_ = true;
		}
		return _H_cache_;
	}

	// Critical density_type at this redshift
	density_type rho_crit() const
	{
		return 3.*square(H())/(8.*pi*Gc);
	}

#endif

	// Clone function
	virtual redshift_obj *redshift_obj_clone() const
	{
		return new redshift_obj(*this);
	}
};

} // namespace IceBRG



#endif // ICEBRG_PHYSICS_REDSHIFT_OBJ_HPP_
