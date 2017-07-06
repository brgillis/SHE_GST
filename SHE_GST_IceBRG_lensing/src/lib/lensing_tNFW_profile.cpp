/**********************************************************************\
  @file lensing_tNFW_profile.cpp

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


#include <vector>

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/units/units.hpp"

#include "SHE_GST_IceBRG_lensing/lensing_tNFW_profile.hpp"
#include "SHE_GST_IceBRG_lensing/detail/lensing_tNFW_caches.hpp"


namespace IceBRG {

const flt_t & min_x = 0.000001;

surface_density_type IceBRG::lensing_tNFW_profile::_quick_Delta_Sigma( const distance_type & r ) const
{
	surface_density_type result = IceBRG::tNFW_sig_cache().get(
			std::log(value_of(mvir0())), z(), std::log(value_of(r)) );
	return result;
}
surface_density_type IceBRG::lensing_tNFW_profile::_quick_offset_Delta_Sigma(
		const distance_type & r, const distance_type & offset_r ) const
{
	if(value_of(offset_r)<=0) return Delta_Sigma(r);
	surface_density_type result = IceBRG::tNFW_offset_sig_cache().get(
			std::log(value_of(mvir0())), z(), std::log(value_of(r)),
			std::log(value_of(offset_r)) );
	return result;
}
surface_density_type IceBRG::lensing_tNFW_profile::_quick_group_Delta_Sigma(
		const distance_type & r, const flt_t & group_c ) const
{
	surface_density_type result = IceBRG::tNFW_group_sig_cache().get(
			std::log(value_of(mvir0())), z(), std::log(value_of(r)),
			group_c );
	return result;
}
surface_density_type IceBRG::lensing_tNFW_profile::_quick_Sigma( const distance_type & r ) const
{
	surface_density_type result = IceBRG::tNFW_Sigma_cache().get(
			std::log(value_of(mvir0())), z(), std::log(value_of(r)));
	return result;
}
surface_density_type IceBRG::lensing_tNFW_profile::_quick_offset_Sigma(
		const distance_type & r, const distance_type & offset_r ) const
{
	if(value_of(offset_r)<=0) return Delta_Sigma(r);
	surface_density_type result = IceBRG::tNFW_offset_Sigma_cache().get(
			std::log(value_of(mvir0())), z(), std::log(value_of(r)),
			std::log(value_of(offset_r)));
	return result;
}
surface_density_type IceBRG::lensing_tNFW_profile::_quick_group_Sigma(
		const distance_type & r, const flt_t & group_c ) const
{
	surface_density_type result = IceBRG::tNFW_group_Sigma_cache().get(
			std::log(value_of(mvir0())), z(), std::log(value_of(r)),
			group_c);
	return result;
}
surface_density_type IceBRG::lensing_tNFW_profile::_proj_dens( const distance_type & r ) const
{
	surface_density_type res;
	density_type rho_c_t_4pi;
	long_flt_t d_c, x, tau_use, fx, lx;

	if ( tau() <= 0 )
		tau_use = default_tau_factor * c();
	else
		tau_use = tau();

	d_c = _delta_c();
	rho_c_t_4pi = 3. * square(H()) / ( 2. * Gc );
	x = max(r / rs(),min_x);
	long_flt_t xx = x*x;
	long_flt_t tautau = tau_use*tau_use;
	long_flt_t tautaup1 = tautau + 1.;
	long_flt_t sqrt_tautaupxx = std::sqrt(tautau + xx);

	if ( x == 1. )
		fx = 1.;
	else if ( x > 1. )
		fx = acos( 1. / x ) / std::sqrt( xx - 1. );
	else
		fx = -std::log( 1. / x - std::sqrt( 1. / ( xx ) - 1. ) ) / std::sqrt( 1. - xx );
	lx = std::log( x / ( sqrt_tautaupxx + tau_use ) );
	if ( x == 1 )
		res = rs() * rho_c_t_4pi * static_cast<flt_t>(
				d_c * tautau
						/ ( 2. * pi * square( tautaup1 ) )
						* ( tautaup1/3. + 2. * fx
								- pi / sqrt_tautaupxx
								+ ( tautau - 1. ) * lx
										/ ( tau_use
												* sqrt_tautaupxx ) ));
	else
		res = rs() * rho_c_t_4pi * static_cast<flt_t>(
				d_c * tautau
						/ ( 2. * pi * square( tautaup1 ) )
						* ( tautaup1 / ( xx - 1. )
								* ( 1. - fx ) + 2. * fx
								- pi / sqrt_tautaupxx
								+ ( tautau - 1. ) * lx
										/ ( tau_use
												* sqrt_tautaupxx ) ));
	return res;
}
surface_density_type IceBRG::lensing_tNFW_profile::_proj_enc_dens( const distance_type & r ) const
{
	surface_density_type res;
	density_type rho_c_t_4pi;
	//Takes M in kg, r in kpc
	long_flt_t d_c = _delta_c();
	long_flt_t tau_use;
	long_flt_t fx;
	if ( tau() <= 0 )
		tau_use = default_tau_factor * c();
	else
		tau_use = tau();

	d_c = _delta_c();
	rho_c_t_4pi = 3. * square(H()) / ( 2. * Gc );
	long_flt_t x = max(r / rs(),min_x);
	long_flt_t xx = x*x;
	long_flt_t tautau = tau_use*tau_use;
	long_flt_t tautaup1 = tautau + 1.;
	long_flt_t sqrt_tautaupxx = std::sqrt(tautau + xx);

	if ( x == 1. )
		fx = 1.;
	else if ( x > 1. )
		fx = acos( 1. / x ) / std::sqrt( xx - 1. );
	else
		fx = -std::log( 1 / x - std::sqrt( 1 / ( xx ) - 1 ) ) / std::sqrt( 1 - xx );
	flt_t lx = std::log( x / ( sqrt_tautaupxx + tau_use ) );
	flt_t log_tau = std::log(tau_use);

	res = rs()*rho_c_t_4pi * static_cast<flt_t>( d_c * tautau
					/ ( pi * xx * tautaup1 * tautaup1 )
					* ( ( tautaup1 + 2 * ( xx - 1 ) ) * fx
							+ pi * tau_use
							+ ( tautau - 1 ) * log_tau
							+ sqrt_tautaupxx
									* ( lx * ( tautau - 1 )
											/ tau_use - pi ) ));
	return res;
}
mass_type IceBRG::lensing_tNFW_profile::_proj_enc_mass( const distance_type & R ) const
{
	return proj_enc_dens( R ) * pi * R * R;
}

} // namespace IceBRG
