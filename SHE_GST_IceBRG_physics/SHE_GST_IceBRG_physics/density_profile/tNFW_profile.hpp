/**********************************************************************\
  @file tNFW_profile.h

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

// body file: brg_physics/astro/density_profile/tNFW_profile.cpp

#ifndef _BRG_TNFW_PROFILE_H_
#define _BRG_TNFW_PROFILE_H_

#include "SHE_GST_IceBRG_main/units/units.hpp"
#include <cmath>
#include <cstdlib>
#include <vector>

#include "SHE_GST_IceBRG_physics/density_profile/detail/density_profile.hpp"
#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/utility.hpp"


namespace IceBRG {

class tNFW_profile: public density_profile
{
	/**********************************
	 tNFW_profile class
	 -------------

	 A virtual class for an object whose
	 mass_type fits a truncated NFW profile.
	 See ... .

	 Defined by four parameters:
	 mvir0, z, c, and tau

	 Parent class:
	 density_type profile

	 Derived classes:
	 tNFW_galaxy

	 **********************************/
private:
#if (1) // Private member variables specific to this class
	mass_type _mvir0_;
	mutable distance_type _rvir_cache_;
	mutable bool _rvir_cached_;
	flt_t _c_, _tau_;

#endif // end private member variables

protected:

	void _uncache_mass();

	flt_t _taufm( const flt_t & mratio, flt_t precision = 0.00001 ) const; // tau from Mtot/Mvir
	flt_t _delta_c() const // Simple function of concentration used as a step in calculating NFW densities
	{
		return ( 200. / 3. ) * cube( _c_ )
				/ ( std::log( 1 + _c_ ) - _c_ / ( 1 + _c_ ) );
	}

	// Functions relating to tNFW profiles

	// Concentration from mass relationship, from Neto
	flt_t _cfm( const mass_type mass, const flt_t & ) const
	{
		return 4.67 * std::pow( mass / ( 1e14 * unitconv::Msuntokg * kg ), -0.11 );
	}
	flt_t _cfm() const // Concentration from mass_type relationship, from Neto
	{
		return _cfm(_mvir0_,z());
	}
	flt_t _mftau( const flt_t & tau, const flt_t & conc ) const // Mtot/Mvir from tau
	{
		if(tau<=0) return 0;
		flt_t M0oM200 = 1 / ( std::log( 1 + conc ) - conc / ( 1 + conc ) );
		flt_t tautau = tau*tau;
		flt_t result =
				M0oM200 * tautau / square( tautau + 1 )
						* ( ( tautau - 1 ) * std::log( tau ) + tau * pi
								- ( tautau + 1 ) );
		return result;
	}
	flt_t _mftau( const flt_t & tau ) const // Mtot/Mvir from tau
	{
		return _mftau(tau,_c_);
	}
	flt_t _mftau() const // Mtot/Mvir from tau
	{
		return _mftau(_tau_,_c_);
	}
public:
#if (1) // public variables and functions

#if (1) // Constructors
	tNFW_profile();

	explicit tNFW_profile( const mass_type & init_mvir0, const flt_t & init_z = 0,
			const flt_t & init_c = -1, const flt_t & init_tau = -1 );

#endif // End constructors

	// Destructor
	virtual ~tNFW_profile()
	{
	}

#if (1) // Set functions

	virtual void set_mvir( const mass_type & new_halo_mass =
			false ) override;
	virtual void set_parameters( const std::vector< any_units_type > & new_parameters ) override;

	virtual void set_z( const flt_t & new_z ) override;
	virtual void set_tau( const flt_t & new_halo_tau ) override;
	virtual void set_c( const flt_t & new_halo_c ) override;

#endif // End set functions

#if (1) //Basic get functions

	virtual mass_type mvir() const override;
	const mass_type & mvir0() const;

	virtual mass_type mtot() const override;

	virtual distance_type rvir() const override;
	distance_type rvir0() const;
	virtual distance_type rt() const override;
	distance_type rs() const;

	virtual velocity_type vvir() const override;
	velocity_type vvir0() const;

	flt_t c() const;
	flt_t tau() const;
#endif // end basic get functions

#if (1) // advanced get functions

	virtual density_type dens( const distance_type & r ) const override;
	virtual mass_type enc_mass( const distance_type & r ) const override;
	virtual size_t num_parameters() const override
	{
		return 4; // Mass, redshift, c, and tau
	}
	virtual std::vector< any_units_type > get_parameters() const override;

	virtual std::vector< std::string > get_parameter_names() const override;
#endif // end advanced get functions

#if (1) // Other operations

	virtual void truncate_to_fraction( const flt_t & fraction ) override;
	virtual redshift_obj *redshift_obj_clone() const override
	{
		return new tNFW_profile( *this );
	}
	virtual density_profile *density_profile_clone() const override
	{
		return new tNFW_profile( *this );
	}
	virtual tNFW_profile *tNFW_profile_clone() const
	{
		return new tNFW_profile( *this );
	}

#endif

#endif // end public variables and functions
};

} // end namespace IceBRG

#endif /* _BRG_TNFW_PROFILE_H_ */
