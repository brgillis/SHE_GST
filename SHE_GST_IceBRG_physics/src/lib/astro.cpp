/**********************************************************************\
  @file astro.cpp

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

#include <SHE_GST_IceBRG_main/math/interpolator/interpolator.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/Eigen.hpp"
#include "SHE_GST_IceBRG_main/math/cache/cache.hpp"
#include "SHE_GST_IceBRG_main/math/cache/cache_2d.hpp"
#include "SHE_GST_IceBRG_main/math/calculus/differentiate.hpp"
#include "SHE_GST_IceBRG_main/math/calculus/integrate.hpp"
#include "SHE_GST_IceBRG_main/math/misc_math.hpp"
#include "SHE_GST_IceBRG_main/units/units.hpp"

#include "SHE_GST_IceBRG_physics/astro.hpp"

#include "SHE_GST_IceBRG_physics/detail/astro_caches.hpp"

namespace IceBRG {

/** Global Function Definitions **/
#if (1)

inverse_time_type H( const flt_t & z )
{
	// Friedmann equation, assuming omega = -1
	if(is_zero(z)) return H_0;
	flt_t zp1 = 1.+z;
	return H_0 * sqrt( Omega_r * quart( zp1 )
							+ Omega_m * cube( zp1 )
							+ Omega_k * square( zp1 ) + Omega_l );
}

// grid functions
#if (1)

#endif // end grid functions

// dfa and afd functions
#if (1)

custom_unit_type<1,0,0,-1,0> dfa( const flt_t & z )
{
	return dfa_cache().get( z );
}
distance_type dfa( const angle_type & da, const flt_t & z )
{
	return da * dfa(z);
}
distance_type dfa( const angle_type & a1, const angle_type & a2,
		const flt_t & z )
{
	return dfa( a2 - a1, z );
}
distance_type dfa( const angle_type & a1x, const angle_type & a1y,
		const angle_type & a2x, const angle_type & a2y, const flt_t & z )
{
	return dfa( skydist2d( a1x, a1y, a2x, a2y ), z );
}
custom_unit_type<-1,0,0,1,0> afd( const flt_t & z )
{
	return 1./dfa(z);
}
angle_type afd( const distance_type & dd, const flt_t & z )
{
	return dd / dfa(z);
}
angle_type afd( const distance_type & d1, const distance_type & d2,
		const flt_t & z )
{
	return afd( abs( d2 - d1 ), z );
}
angle_type afd( const distance_type & d1x,
		const distance_type & d1y, const distance_type & d2x,
		const distance_type & d2y, const flt_t & z )
{
	return afd( dist2d( d1x, d1y, d2x, d2y ), z );
}

flt_t zfa( const flt_t & a )
{
	return 1. / safe_d( a ) - 1.;
}
flt_t afz( const flt_t & z )
{
	return 1. / safe_d( 1 + z );
}

time_type tfz( const flt_t & z )
{
	return tfa( afz( z ) );
}
time_type tfa( const flt_t & a )
{
	return tfa_cache().get( a );
}
flt_t zft( const time_type & t )
{
	return zfa( aft( t ) );
}
flt_t aft( const time_type & t )
{
	tfa_cache cache;
	return cache.inverse_get( t );
}

time_type universe_age( const flt_t & z )
{
	time_type t = t_present_day - tfz(z);

	return t;
}

#endif

// Functions to integrate out distances
#if (1)
distance_type integrate_add( const flt_t & z1, const flt_t & z2 )
{
	return integrate_distance( z1, z2, 0, 100000 );
}
distance_type integrate_cmd( const flt_t & z1, const flt_t & z2 )
{
	return integrate_distance( z1, z2, 1, 10000000 );
}
distance_type integrate_Ld( const flt_t & z1, const flt_t & z2 )
{
	return integrate_distance( z1, z2, 2, 10000000 );
}
distance_type integrate_ltd( const flt_t & z1, const flt_t & z2 )
{
	return integrate_distance( z1, z2, 3, 10000000 );
}
distance_type integrate_add( const flt_t & z )
{
	return integrate_distance( 0, z, 0, 100000 );
}
distance_type integrate_cmd( const flt_t & z )
{
	return integrate_distance( 0, z, 1, 10000000 );
}
distance_type integrate_Ld( const flt_t & z )
{
	return integrate_distance( 0, z, 2, 10000000 );
}
distance_type integrate_ltd( const flt_t & z )
{
	return integrate_distance( 0, z, 3, 10000000 );
}
distance_type integrate_distance( const flt_t & z1_init,
		const flt_t & z2_init, const int_t & mode, const int_t & n )
{
	// Function that will help calculate cosmic distances, thanks to Richard Powell - http://www.atlasoftheuniverse.com/
	// NOTE: Will return a negative value if z2 < z1. This is by design.

	flt_t OK0;
	flt_t OM0 = Omega_m, OR0 = Omega_r, OL0 = Omega_l;
	flt_t OM, OR, OL, OK;
	flt_t z1 = z1_init, z2 = z2_init;
	flt_t HD; //Hubble distance_type in billions of lightyears
	flt_t z, a, a1, a2, adot, h1;
	flt_t DC = std::numeric_limits<flt_t>::max(), DCC = 0, DT = std::numeric_limits<flt_t>::max(), DTT = 0, DM;
	int_t i;
	short_int_t sign = 1;

	if(is_zero(z1-z2)) return 0;

	OK0 = 1 - OM0 - OL0 - OR0;

	if ( z2 < z1 )
	{
		std::swap(z1,z2);
		sign = -1;
	}

	if ( is_zero(z1) )
	{
		z = z2;
		h1 = value_of(H_0);
		OM = OM0;
		OR = OR0;
		OL = OL0;
		OK = OK0;
	}
	else
	{
		a1 = 1 / ( 1 + z1 );
		a2 = 1 / ( 1 + z2 );
		z = ( a1 / a2 ) - 1;
		h1 = value_of(H_0)
				* std::sqrt( OR0 * inv_quart( a1 ) + OM0 * inv_cube( a1 )
								+ OK0 * inv_square( a1 ) + OL0 );
		OM = OM0 * square( value_of(H_0) / h1 ) * inv_cube( a1 );
		OR = OR0 * square( value_of(H_0) / h1 ) * inv_quart( a1 );
		OL = OL0 * square( value_of(H_0) / h1 );
		OK = 1 - OM - OR - OL;
	}

	HD = value_of(c) / h1;

	for ( i = n; i >= 1; i-- )        // This loop is the numerical integration
	{
		a = ( i - 0.5 ) / n;              // Steadily decrease the scale factor
		// Comoving formula (See section 4 of Hogg, but I've added a radiation term too):
		adot = a * std::sqrt( OM * inv_cube(a) + OK * inv_square(a)
								+ OL + OR * inv_quart(a) ); // Note that "a" is equivalent to 1/(1+z)
		 // Collect DC and DT until the correct scale is reached
		DCC = DCC + 1 / ( a * adot ) / n; // Running total of the comoving distance_type
		DTT = DTT + 1 / adot / n; // Running total of the light travel time_type (see section 10 of Hogg)
		 // Collect DC and DT until the correct scale is reached
		DC = DCC;                 // Comoving distance_type DC
		DT = DTT;                 // Light travel time_type DT
		if ( a <= 1 / ( 1 + z ) ) // Collect DC and DT until the correct scale is reached
		{
			break;
		}
	}

	// Transverse comoving distance_type DM from section 5 of Hogg:
	if ( OK > 0.0001 )
		DM = ( 1 / std::sqrt( OK ) ) * sinh( std::sqrt( OK ) * DC );
	else if ( OK < -0.0001 )
		DM = ( 1 / std::sqrt( fabs( OK ) ) ) * std::sin( std::sqrt( fabs( OK ) ) * DC );
	else
		DM = DC;

	//age = HD*DTT;                 // Age of the universe (billions of years)
	//size = HD*DCC;                // Comoving radius of the observable universe

	flt_t res;

	switch ( mode )
	{
	case 0:
		res = sign * HD * DM / ( 1 + z ); // Angular diameter distance_type (section 6 of Hogg)
		break;
	case 1:
		res = sign * HD * DC;             // Comoving distance_type
		break;
	case 2:
		res = sign * HD * DM * ( 1 + z );  // Luminosity distance_type (section 7 of Hogg)
		break;
	case 3:
		res = sign * HD * DT;             // Light travel distance_type
		break;
	default:
		res = sign * HD * DT;             // Light travel distance_type
	}

	return units_cast<distance_type>(res);
}

distance_type ad_distance( flt_t z1, flt_t z2 )
{
	if ( z2 < z1 )
		std::swap( z1, z2 );
	return add_cache().get( z1, z2 );
}
distance_type comoving_distance( flt_t z )
{
	return dfa_cache().get(z)*(1+z)*rad;
}
distance_type luminosity_distance( flt_t z )
{
	return dfa_cache().get(z)*square(1+z)*rad;
} //

volume_type comoving_volume_element( flt_t z )
{
	return square(comoving_distance(z))*c/H(z);
}

#endif

// Lensing functions
#if (1)

surface_density_type sigma_crit( const flt_t & z_lens,
		const flt_t & z_source )
{
	return square( c ) / ( 4. * pi * Gc )
			* ad_distance( 0, z_source )
			/ ( ad_distance( 0, z_lens )
					* ad_distance( z_lens, z_source ) );

}
#endif // end lensing functions

// Luminosity-related functions
#if (1)

flt_t get_abs_mag_from_app_mag( flt_t const & app_mag, flt_t const & z )
{
	distance_type lum_distance_at_z = luminosity_distance(z);

	flt_t res = app_mag - 5*((std::log10(lum_distance_at_z/(1*unitconv::pctom*m)))-1);

	return res;
}

flt_t get_app_mag_from_abs_mag( flt_t const & abs_mag, flt_t const & z )
{
	distance_type lum_distance_at_z = luminosity_distance(z);

	flt_t res = abs_mag + 5*((std::log10(lum_distance_at_z/(1*unitconv::pctom*m)))-1);

	return res;
}

// Using results from Willmer et al. 2006 for the "All" sample with minimal weighting at
// z = 0.7 for the luminosity function

const inverse_volume_type phi_star = 24.43e-4 / cube( unitconv::Mpctom * m);
constexpr flt_t mag_B_star = -21.53;
constexpr flt_t alpha = -1.3;
constexpr flt_t abs_mag_B_min = -25;

inverse_volume_type differential_luminosity_function( flt_t const & mag_B )
{
	inverse_volume_type res = 0.4 * std::log(10.) * phi_star *
			std::pow(10.,0.4*(mag_B_star-mag_B)*(alpha+1))*std::exp(-std::pow(10,0.4*(mag_B_star-mag_B)));

	return res;
}
inverse_volume_type integrated_luminosity_function( flt_t const & mag_B_lo, flt_t const & mag_B_hi )
{
	inverse_volume_type res = lum_func_integral_cache().get(mag_B_lo,mag_B_hi);
	return res;
}

flt_t faint_bright_ratio( flt_t const & z, flt_t const & bright_abs_mag_i_lim,
		flt_t const & faint_app_mag_i_lim)
{

	flt_t faint_abs_mag_i_lim = get_abs_mag_from_app_mag( faint_app_mag_i_lim, z );

	// Estimate mag_B limits, assuming mag_B ~ mag_g
	flt_t faint_abs_mag_B_lim = estimate_abs_mag_g_from_abs_mag_i(faint_abs_mag_i_lim);
	flt_t bright_abs_mag_B_lim = estimate_abs_mag_g_from_abs_mag_i(bright_abs_mag_i_lim);

	flt_t res = integrated_luminosity_function( abs_mag_B_min, faint_abs_mag_B_lim ) /
			integrated_luminosity_function( abs_mag_B_min, bright_abs_mag_B_lim );

	return res;
}

#endif // end Luminosity-related functions

// Mass-related functions
#if(1)

flt_t unnormed_power_spectrum( inverse_distance_type const & k)
{
	using std::log;

	constexpr flt_t nsp = 0.958;
	const distance_type Mpc = unitconv::Mpctom * m;
	const distance_type Mpc_o_h = Mpc/h_0;
	const flt_t xi = Omega_m * h_0;

	flt_t res = pow(k*Mpc_o_h,2. + nsp) * square(log(1. + 2.34*Mpc_o_h * k / xi) /
			(2.34*Mpc_o_h * k / xi) /
			pow(1. + 3.89*Mpc_o_h * k / xi + square(16.2*Mpc_o_h * k / xi) + cube(5.47*Mpc_o_h * k / xi) +
					quart(6.71*Mpc_o_h * k / xi),0.25));

	return res;
}

// Press-Schechter formalism functions
flt_t delta_c( flt_t const & z )
{
	flt_t res = 1.686 * universe_age(z)/universe_age(0.);
	return res;
}
density_type rho_bar( flt_t const & z)
{
	density_type rho_crit_0 = 3.*square(H_0)/(8.*pi*Gc);
	density_type rho_bar_0 = Omega_m * rho_crit_0;

	density_type res = rho_bar_0*cube(1.+z);

	return res;
}
distance_type r_of_m( mass_type const & mass )
{
	distance_type r = ipow<1,3>(3.*mass/(4*pi*rho_bar(0.)));

	return r;
}
flt_t sigma_of_r( distance_type const & r)
{
	return 0.8*sqrt(sigma_r_cache().get(r)/sigma_r_cache().get(8.*unitconv::Mpctom*m/h_0));
}
flt_t sigma_of_m( mass_type const & mass )
{
	return sigma_of_r(r_of_m(mass));
}
flt_t nu_of_m( mass_type const & mass, flt_t const & z )
{
	flt_t nu = delta_c(z)/sigma_of_m(mass);
	return nu;
}
flt_t fps_of_nu(flt_t const & nu)
{
	return sqrt(2./pi)*nu*std::exp(-square(nu)/2.);
}
flt_t fec_of_nu(flt_t const & nu)
{
	flt_t nu_tilde = 0.84*nu;
	return 0.322*(1+std::pow(nu_tilde,-0.6))*fps_of_nu(nu_tilde);
}
inverse_volume_inverse_mass_type mass_function( mass_type const & mass, flt_t const & z )
{
	// Using Sheth, Mo, and Tormen (2001) formalism

	flt_t nu = nu_of_m(mass,z);

	flt_t f_of_nu = fec_of_nu(nu);

	auto ln_nu_of_m = [&z] (mass_type const & mass)
	{
		return std::log(nu_of_m(mass,z));
	};

	custom_unit_type<0,0,-1,0,0> d_ln_nu_d_m = differentiate(&ln_nu_of_m,mass);

	inverse_volume_inverse_mass_type res = rho_bar(0.)/mass * f_of_nu * d_ln_nu_d_m;

	return res;
}
inverse_volume_type log10_mass_function( flt_t const & log10msun_mass, flt_t const & z )
{
	mass_type mass = std::pow(10.,log10msun_mass)*unitconv::Msuntokg*kg;

	inverse_volume_type res = mass_function(mass,z)*mass*std::log(10.);

	return res;
}
inverse_volume_type integrated_log10_mass_function( flt_t const & l10_m_lo, flt_t const & l10_m_hi,
		flt_t const & z )
{
	inverse_volume_type res = l10_mass_function_integral_cache().get(l10_m_lo,z) -
			l10_mass_function_integral_cache().get(l10_m_hi,z);
	return res;
}

#endif // end Mass-related functions

// Cluster visibility functions
#if(1)

const mass_type richness_mstar = 4.07e12*unitconv::Msuntokg*kg;
constexpr flt_t richness_beta = 1.4;

flt_t cluster_richness( mass_type const & mass, flt_t const & z,
		flt_t const & bright_abs_mag_i_lim, flt_t const & faint_app_mag_i_lim )
{
	flt_t fb_ratio = faint_bright_ratio(z,bright_abs_mag_i_lim,faint_app_mag_i_lim);

	flt_t res = fb_ratio * std::pow(mass/richness_mstar,1./richness_beta);

	return res;
}
mass_type min_cluster_mass( flt_t const & z, flt_t const & bright_abs_mag_lim,
		flt_t const & faint_app_mag_lim )
{
	flt_t fb_ratio = faint_bright_ratio(z,bright_abs_mag_lim,faint_app_mag_lim);

	mass_type res = richness_mstar * pow(2/fb_ratio,richness_beta);

	return res;
}

inverse_square_angle_type cluster_angular_density_at_z(flt_t const & z)
{
	inverse_volume_type cluster_volume_density = visible_cluster_density_cache().get(z);

	custom_unit_type<3,0,0,-2,0> vol_per_area = square(comoving_distance(z)/rad)*c/H(z);

	inverse_square_angle_type density_per_area = cluster_volume_density*vol_per_area;

	return density_per_area;
}

flt_t visible_clusters( square_angle_type const & area, flt_t const & z1, flt_t const & z2 )
{
	inverse_square_angle_type area_density = visible_clusters_cache().get(z2) -
			visible_clusters_cache().get(z1);

	flt_t res = area*area_density;

	return res;
}

flt_t integrate_mean_cluster_richness_at_redshift( flt_t const & z )
{
	auto typical_richness = [&] (flt_t const & l10_m)
	{
		mass_type m = std::pow(10.,l10_m)*unitconv::Msuntokg*kg;
		return cluster_richness(m,z);
	};
	auto weight = [&] (flt_t const & l10_m)
	{
		return log10_mass_function(l10_m,z);
	};
	auto weighted_typical_richness = [&] (flt_t const & l10_m)
	{
		return typical_richness(l10_m)*weight(l10_m);
	};

	flt_t l10_min_mass = std::log10(min_cluster_mass(z)/(unitconv::Msuntokg*kg));

	auto total_weighted_richness = integrate_Romberg(weighted_typical_richness,l10_min_mass,mass_func_l10_max);
	auto total_weight = integrate_Romberg(weight,l10_min_mass,mass_func_l10_max);

	flt_t mean_richness = total_weighted_richness/total_weight;

	return mean_richness;
}
flt_t mean_cluster_richness_at_redshift( flt_t const & z )
{
	return cluster_richness_at_z_cache().get(z);
}

flt_t integrate_mean_cluster_richness( flt_t const & z_min, flt_t const & z_max )
{
	if(is_zero(z_max-z_min))
	{
		return mean_cluster_richness_at_redshift(z_min);
	}
	auto richness_at_z = [&] (flt_t const & z)
	{
		return mean_cluster_richness_at_redshift(z);
	};
	auto weight = [&] (flt_t const & z)
	{
		return cluster_angular_density_at_z(z);
	};
	auto weighted_richness_at_z = [&] (flt_t const & z)
	{
		return richness_at_z(z)*weight(z);
	};

	auto total_weighted_richness = integrate_Romberg(weighted_richness_at_z,z_min,z_max);
	auto total_weight = integrate_Romberg(weight,z_min,z_max);

	flt_t mean_richness = total_weighted_richness/total_weight;

	return mean_richness;
}
flt_t mean_cluster_richness( flt_t const & z_min, flt_t const & z_max )
{
	return cluster_richness_cache().get(z_min,z_max);
}

#endif // end cluster visibility functions

// Functions connecting mass and luminosity through abundance matching
#if(1)

interpolator lum_cdf_interpolator;
interpolator lum_cdf_inv_interpolator;

void set_up_lum_interpolators()
{
	// Clear in case they need it
	lum_cdf_interpolator.clear();
	lum_cdf_inv_interpolator.clear();

	int_t num_points = 150;

	flt_array_t lums = flt_array_t::LinSpaced(num_points,lum_func_min_abs_mag_B,lum_func_max_abs_mag_B);
	flt_array_t lum_cdfs(num_points);

	for( int_t i=0; i<num_points; ++i )
	{
		lum_cdfs[i] = value_of(integrated_luminosity_function(lum_func_min_abs_mag_B,lums[i]));
	}

	// Normalize the cdfs
	lum_cdfs -= lum_cdfs[0];

	for( int_t i=0; i<num_points; ++i )
	{
		lum_cdf_interpolator.add_point(lum_cdfs[i],lums[i]);
		lum_cdf_inv_interpolator.add_point(lums[i],lum_cdfs[i]);
	}

	return;
}

std::tuple<interpolator,interpolator> get_mass_interpolators(flt_t const & z)
{
	interpolator mass_interpolator;
	interpolator mass_inv_interpolator;

	int_t num_points = 150;

	flt_array_t masses = flt_array_t::LinSpaced(num_points,mass_func_l10_min,mass_func_l10_max);
	flt_array_t mass_cdfs(num_points);

	for( int_t i=0; i<num_points; ++i )
	{
		mass_cdfs[i] = value_of(integrated_log10_mass_function(masses[i],mass_func_l10_max,z));
	}

	// Normalize the cdfs
	mass_cdfs -= mass_cdfs[num_points-1];

	for( int_t i=0; i<num_points; ++i )
	{
		mass_interpolator.add_point(mass_cdfs[i],masses[i]);
		mass_inv_interpolator.add_point(masses[i],mass_cdfs[i]);
	}

	return std::make_tuple(mass_interpolator,mass_inv_interpolator);
}

flt_t get_abs_mag_B_from_mass( mass_type const & m, flt_t const & z )
{
	interpolator mass_cdf_inv_interpolator = std::get<1>(get_mass_interpolators(z));
	if(lum_cdf_interpolator.empty()) set_up_lum_interpolators();

	flt_t l10_m = std::log10(m/(unitconv::Msuntokg*kg));

	flt_t rank = mass_cdf_inv_interpolator(l10_m);

	flt_t abs_mag_B = lum_cdf_interpolator(rank);

	return abs_mag_B;
}

mass_type get_mass_from_abs_mag_B( flt_t const & abs_mag_B, flt_t const & z )
{
	interpolator mass_cdf_interpolator = std::get<0>(get_mass_interpolators(z));
	if(lum_cdf_inv_interpolator.empty())
		set_up_lum_interpolators();

	flt_t l10_m = mass_cdf_interpolator(lum_cdf_inv_interpolator(abs_mag_B));

	return std::pow(10.,l10_m)*unitconv::Msuntokg*kg;
}

flt_t get_app_mag_B_from_mass( mass_type const & m, flt_t const & z )
{
	flt_t abs_mag_B = get_abs_mag_B_from_mass(m,z);
	flt_t app_mag_B = get_app_mag_from_abs_mag(abs_mag_B,z);

	return app_mag_B;
}

mass_type get_mass_from_app_mag_B( flt_t const & app_mag_B, flt_t const & z )
{
	flt_t abs_mag_B = get_abs_mag_from_app_mag(app_mag_B,z);
	mass_type mass = get_mass_from_abs_mag_B(abs_mag_B,z);

	return mass;
}

#endif // end abundance matching functions


// Galaxy visibility functions
#if(1)

flt_t max_galaxy_abs_mag_B( flt_t const & z, flt_t const & faint_app_mag_i_lim )
{
	flt_t abs_mag_i = get_abs_mag_from_app_mag(faint_app_mag_i_lim,z);

	flt_t abs_mag_B = estimate_abs_mag_g_from_abs_mag_i(abs_mag_i);

	return abs_mag_B;
}

inverse_square_angle_type galaxy_angular_density_at_z(flt_t const & z)
{
	inverse_volume_type galaxy_volume_density = visible_galaxy_density_cache().get(z);

	custom_unit_type<3,0,0,-2,0> vol_per_area = comoving_volume_element(z)/square(rad);

	inverse_square_angle_type density_per_area = galaxy_volume_density*vol_per_area;

	return density_per_area;
}

flt_t visible_galaxies( square_angle_type const & area, flt_t const & z1, flt_t const & z2 )
{
	inverse_square_angle_type area_density = visible_galaxies_cache().get(z2) -
			visible_galaxies_cache().get(z1);

	flt_t res = area*area_density;

	return res;
}

#endif // end galaxy visibility functions

// Stellar mass functions
#if(1)

// Taken from regression of CFHTLenS data. Scatter of 1.2972528920157342
flt_t estimate_abs_mag_g_from_stellar_mass( mass_type const & stellar_mass )
{
	flt_t l10_m = std::log10(stellar_mass/(unitconv::Msuntokg*kg));

	flt_t abs_mag_g = (l10_m - 2.2480200524693386)/-0.37325475722757273;

	return abs_mag_g;
}

// Taken from regression of CFHTLenS data. Log10 scatter of 0.585461144291201
mass_type estimate_stellar_mass_from_abs_mag_g( flt_t const & abs_mag_g )
{
	flt_t l10_m = 2.2480200524693386 + -0.37325475722757273*abs_mag_g;

	mass_type stellar_mass = std::pow(10.,l10_m)*unitconv::Msuntokg*kg;

	return stellar_mass;
}

// Taken from regression of CFHTLenS data. Scatter of 0.93030163251098796
flt_t estimate_abs_mag_i_from_stellar_mass( mass_type const & stellar_mass )
{
	flt_t l10_m = std::log10(stellar_mass/(unitconv::Msuntokg*kg));

	flt_t abs_mag_i = (l10_m - 1.3190844893636555)/-0.40474903941295159;

	return abs_mag_i;
}

// Taken from regression of CFHTLenS data. Log10 scatter of 0.41169238482277282
mass_type estimate_stellar_mass_from_abs_mag_i( flt_t const & abs_mag_i )
{
	flt_t l10_m = 1.3190844893636555 + -0.40474903941295159*abs_mag_i;

	mass_type stellar_mass = std::pow(10.,l10_m)*unitconv::Msuntokg*kg;

	return stellar_mass;
}

// Combining regressions
flt_t estimate_abs_mag_g_from_abs_mag_i( flt_t const & abs_mag_i )
{
	return estimate_abs_mag_g_from_stellar_mass( estimate_stellar_mass_from_abs_mag_i(abs_mag_i) );
}
flt_t estimate_abs_mag_i_from_abs_mag_g( flt_t const & abs_mag_g )
{
	return estimate_abs_mag_i_from_stellar_mass( estimate_stellar_mass_from_abs_mag_g(abs_mag_g) );
}

// Using Lance's zeropoints to convert between i and vis
flt_t estimate_abs_mag_vis_from_abs_mag_i( flt_t const & abs_mag_i )
{
	return abs_mag_i + 0.2643;
}
flt_t estimate_abs_mag_i_from_abs_mag_vis( flt_t const & abs_mag_vis )
{
	return abs_mag_vis - 0.2643;
}

// More ways to use mag_vis
flt_t estimate_abs_mag_vis_from_stellar_mass( mass_type const & stellar_mass )
{
	return estimate_abs_mag_vis_from_abs_mag_i(
			estimate_abs_mag_i_from_stellar_mass(stellar_mass) );
}
mass_type estimate_stellar_mass_from_abs_mag_vis( flt_t const & abs_mag_vis )
{
	return estimate_stellar_mass_from_abs_mag_i(
			estimate_abs_mag_i_from_abs_mag_vis(abs_mag_vis) );
}

// Connecting magnitude conversions
flt_t estimate_abs_mag_g_from_abs_mag_vis( flt_t const & abs_mag_vis )
{
	return estimate_abs_mag_g_from_abs_mag_i( estimate_abs_mag_i_from_abs_mag_vis(abs_mag_vis) );
}
flt_t estimate_abs_mag_vis_from_abs_mag_g( flt_t const & abs_mag_g )
{
	return estimate_abs_mag_vis_from_abs_mag_i( estimate_abs_mag_i_from_abs_mag_g(abs_mag_g) );
}

#endif // stellar mass functions

#endif // end Global function definitions

} // namespace IceBRG
