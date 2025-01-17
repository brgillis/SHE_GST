/**********************************************************************\
  @file tNFW_profile.cpp

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

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <boost/math/tools/roots.hpp>

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/error_handling.hpp"
#include "SHE_GST_IceBRG_main/units/units.hpp"
#include "SHE_GST_IceBRG_main/utility.hpp"

#include "SHE_GST_IceBRG_physics/density_profile/tNFW_profile.hpp"
#include "SHE_GST_IceBRG_physics/density_profile/detail/tNFW_profile_functors.hpp"


constexpr IceBRG::flt_t min_x = 0.0001;

// IceBRG::tNFW_profile class methods
#if (1)

void IceBRG::tNFW_profile::_uncache_mass()
{
	_rvir_cached_ = false;
	hmvir_cached = false;
	hmtot_cached = false;
}

IceBRG::flt_t IceBRG::tNFW_profile::_taufm( const flt_t & m_ratio,
		flt_t precision ) const
{
	flt_t m_target = m_ratio * _mftau();
	flt_t taustepsize = _tau_ / 2;
	flt_t tautest[3];
	flt_t mtest, mbest;
	int_t i, ibest;

	tautest[0] = _tau_ / 2;
	mbest = 1e99;

	while ( taustepsize > precision * _c_ )
	{
		taustepsize /= 2;
		tautest[1] = tautest[0] - taustepsize;
		tautest[2] = tautest[0] + taustepsize;
		ibest = 0;
		for ( i = 0; i < 3; i++ )
		{
			mtest = _mftau( tautest[i] );
			if ( fabs( mtest - m_target ) <= fabs( mbest - m_target ) )
			{
				ibest = i;
				mbest = mtest;
			}
		}
		tautest[0] = tautest[ibest];
	}

	return tautest[0];

}

#if (1) // Constructors
IceBRG::tNFW_profile::tNFW_profile()
{
	_mvir0_ = 0;
	_rvir_cache_ = 0;
	_rvir_cached_ = false;
	_c_ = 0;
	_tau_ = 0;
}

IceBRG::tNFW_profile::tNFW_profile( const mass_type & init_mvir0,
		const flt_t & init_z, const flt_t & init_c, const flt_t & init_tau )
{
	_mvir0_ = init_mvir0;
	_rvir_cache_ = 0;
	_rvir_cached_ = false;
	if ( init_c <= 0 )
	{
		_c_ = _cfm();
	}
	else
	{
		_c_ = init_c;
	}
	if ( init_tau < 0 )
	{
		_tau_ = default_tau_factor * _c_;
	}
	else
	{
		_tau_ = init_tau;
	}
	set_z(init_z);
}

#endif // End constructors

#if (1) // Set functions

void IceBRG::tNFW_profile::set_mvir( const mass_type & new_halo_mass )
{
	_mvir0_ = new_halo_mass;
	_uncache_mass();
}
void IceBRG::tNFW_profile::set_tau( const flt_t & new_halo_tau )
{
	_tau_ = new_halo_tau;
	_uncache_mass();
}
void IceBRG::tNFW_profile::set_c( const flt_t & new_halo_c )
{
	_c_ = new_halo_c;
	_uncache_mass();
}
void IceBRG::tNFW_profile::set_z( const flt_t & new_z )
{
	redshift_obj::set_z( new_z );
	_uncache_mass();
}
void IceBRG::tNFW_profile::set_parameters(
		const std::vector< any_units_type > &parameters )
{
	assert(parameters.size()==num_parameters());

	set_mvir( any_cast<mass_type>(parameters[0]) );
	set_z( any_cast<flt_t>(parameters[1]) );
	flt_t new_c = any_cast<flt_t>(parameters[2]);
	if ( new_c <= 0 )
	{
		set_c( _cfm( mvir(), z() ) );
	}
	else
	{
		set_c( new_c );
	}
	flt_t new_tau = any_cast<flt_t>(parameters[3]);
	if ( new_tau <= 0 )
	{
		set_tau( default_tau_factor * _c_ );
	}
	else
	{
		set_tau( new_tau );
	}
}

#endif // end set functions

IceBRG::mass_type IceBRG::tNFW_profile::mvir() const
{
	return enc_mass(rvir());
}
const IceBRG::mass_type & IceBRG::tNFW_profile::mvir0() const
{
	return _mvir0_;
}

IceBRG::flt_t IceBRG::tNFW_profile::tau() const
{
	return _tau_;
}
IceBRG::flt_t IceBRG::tNFW_profile::c() const
{
	return _c_;
}

IceBRG::mass_type IceBRG::tNFW_profile::mtot() const
{
	return _mvir0_ * _mftau();
}

IceBRG::velocity_type IceBRG::tNFW_profile::vvir() const
{
	return ipow<1,3>( 10. * Gc * H() * mvir() );
}
IceBRG::velocity_type IceBRG::tNFW_profile::vvir0() const
{
	return ipow<1,3>( 10. * Gc * H() * _mvir0_ );
}
IceBRG::distance_type IceBRG::tNFW_profile::rvir() const
{
	if(!_rvir_cached_)
	{

		tNFW_solve_rvir_functor func(this);

		_rvir_cache_ = units_cast<distance_type>(0.);

		// First, we try solving iteratively
	  try
	  {
	    distance_type max_r = rvir0();
	    auto r_bracket = boost::math::tools::bisect( func, units_cast<distance_type>(0.), max_r,
	        boost::math::tools::eps_tolerance<distance_type>(4));
	    _rvir_cache_ = r_bracket.first + (r_bracket.second - r_bracket.first)/2;
	  }
	  catch(const std::exception &e)
	  {
	    handle_error("Could not solve virial radius. Assuming it's zero.");

	    _rvir_cache_ = units_cast<distance_type>(0.);
	    _rvir_cached_ = true;
	    return _rvir_cache_;
	  }
		_rvir_cached_ = true;
	}
	return _rvir_cache_;
}
IceBRG::distance_type IceBRG::tNFW_profile::rvir0() const
{
	return vvir0() / H() / 10.;
}
IceBRG::distance_type IceBRG::tNFW_profile::rs() const
{
	return rvir0() / _c_;
}
IceBRG::distance_type IceBRG::tNFW_profile::rt() const
{
	return rvir0() * _tau_/_c_;
}

IceBRG::density_type IceBRG::tNFW_profile::dens( const distance_type & r ) const
{
	density_type result, rho_c;

	flt_t d_c, x, tau_use;
	if ( _tau_ <= 0 )
		tau_use = default_tau_factor * _c_;
	else
		tau_use = _tau_;
	d_c = _delta_c();
	rho_c = 3. * square(H()) / ( 8. * pi * Gc );
	x = max(r / rs(),min_x);

	result = ( d_c * rho_c ) / ( x * square( 1 + x ) )
			* square( tau_use )
			/ ( square( tau_use ) + square( x ) );

	return result;
}
IceBRG::mass_type IceBRG::tNFW_profile::enc_mass( const distance_type & r ) const
{
	using IceBRG::square;
	using IceBRG::cube;
	using std::log;
	using std::atan;

	density_type rho_c;
	flt_t m0, mx;
	flt_t d_c, x, tau_use;
	if(value_of(r)<=0) return units_cast<mass_type>(0.);
	if ( _tau_ < 0 )
		tau_use = default_tau_factor * _c_;
	else if (_tau_ == 0)
		return 0;
	else
		tau_use = _tau_;

	flt_t tau_sq = square(tau_use);

	d_c = _delta_c();
	rho_c = 3. * square(H()) / ( 8. * pi * Gc );
	x = max(r / rs(),min_x);

	// Result here integrated with Wolfram Alpha
	m0 = (2 * (1 + tau_sq) - (-1 + tau_sq) * 2 * log(tau_use));
	mx = ((2 * (1 + tau_sq)) / (1 + x)
							+ 4 * tau_use * atan(x / tau_use)
							+ 2 * (-1 + tau_sq) * log(1 + x)
							- (-1 + tau_sq) * log(tau_sq + square(x))) - m0;
	return cube(rs()) * d_c * rho_c * 2. * pi * tau_sq * mx
			/ square(1 + tau_sq);
}

std::vector< IceBRG::any_units_type > IceBRG::tNFW_profile::get_parameters() const
{
	std::vector< any_units_type > parameters( num_parameters() );
	parameters[0] = _mvir0_;
	parameters[1] = z();
	parameters[2] = _c_;
	parameters[3] = _tau_;
	return parameters;
}

std::vector< std::string > IceBRG::tNFW_profile::get_parameter_names() const
{
	std::vector< std::string > parameter_names( num_parameters() );

	parameter_names[0] = "mvir0";
	parameter_names[1] = "z";
	parameter_names[2] = "c";
	parameter_names[3] = "tau";
	return parameter_names;
}

void IceBRG::tNFW_profile::truncate_to_fraction( const flt_t & f )
{
	if ( f <= 0 )
	{
		if ( f < 0 )
			handle_error("Cannot truncate to negative fraction. Truncating to zero instead.");
		_tau_ = 0;
		override_rhmvir( 0 );
		override_rhmtot( 0 );
		_rvir_cache_ = 0;
		_rvir_cached_ = true;
	}
	else
	{
		flt_t new_tau_val = _taufm( f, bound(SMALL_FACTOR, std::fabs(0.1*(1-f)),0.00001) );
		if ( new_tau_val < 0 )
		{
			handle_error(std::string("Could not solve for new tau value in truncate_to_fraction.\n")
						+ "tau will remain unchanged.");
		}
		else
		{
			_tau_ = new_tau_val;
		}
		_uncache_mass();
	}
}

#endif // end tNFW_profile functions
