/**********************************************************************\
  @file astro_caches.cpp

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


#include <cstdlib>
#include <iostream>
#include <exception>
#include <string>

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/math/cache/cache.hpp"
#include "SHE_GST_IceBRG_main/math/cache/cache_2d.hpp"
#include "SHE_GST_IceBRG_main/math/calculus/integrate.hpp"
#include "SHE_GST_IceBRG_main/units/units.hpp"
#include "SHE_GST_IceBRG_main/units/unit_conversions.hpp"

#include "SHE_GST_IceBRG_physics/abundance_matching.hpp"
#include "SHE_GST_IceBRG_physics/detail/astro_caches.hpp"
#include "SHE_GST_IceBRG_physics/cluster_visibility.hpp"
#include "SHE_GST_IceBRG_physics/constants.hpp"
#include "SHE_GST_IceBRG_physics/cosmology.hpp"
#include "SHE_GST_IceBRG_physics/galaxy_visibility.hpp"
#include "SHE_GST_IceBRG_physics/luminosity.hpp"
#include "SHE_GST_IceBRG_physics/mass_function.hpp"

/** Static Class Initialisation **/
#if (1)

namespace IceBRG {

// Initialisation for IceBRG::dfa_cache
DEFINE_BRG_CACHE( dfa_cache, flt_t, distance_over_angle_type,
		0, 5, 0.001
		,
			return integrate_add( 0, in_param )/radian;
		,

		,

)

// Initialisation for IceBRG::add_cache
DEFINE_BRG_CACHE_2D( add_cache, flt_t, flt_t, distance_type,
		0, 5, 0.01, 0, 5, 0.01,
			return IceBRG::integrate_add(in_param_1,in_param_2);
		,

		,

)

// Initialisation for IceBRG::tfa_cache
DEFINE_BRG_CACHE( tfa_cache, flt_t, time_type, 0.001, 1.02, 0.001
		,
			return -integrate_ltd( 0, zfa( in_param ) ) / c;
		,

		,

)

// Initialisation for IceBRG::lum_func_integral_cache
DEFINE_BRG_CACHE_2D( lum_func_integral_cache, flt_t, flt_t, inverse_volume_type,
		lum_func_min_abs_mag_B, lum_func_max_abs_mag_B, 0.1,
		lum_func_min_abs_mag_B, lum_func_max_abs_mag_B, 0.1
		,
			inverse_volume_type res = integrate_Romberg(differential_luminosity_function,in_param_1,in_param_2);
			return res;
		,

		,

)

// Initialisation for IceBRG::sigma_r_cache
DEFINE_BRG_CACHE( sigma_r_cache, distance_type, flt_t,
		0.01*unitconv::Mpctom*m, 100.*unitconv::Mpctom*m, 0.01*unitconv::Mpctom*m
		,
			const distance_type Mpc_o_h = unitconv::Mpctom * m/h_0;

			distance_type aR = in_param/h_0;

			auto power_spectrum = [&] (inverse_distance_type const & k)
			{
				using std::log;
				using std::sin;
				using std::cos;

				flt_t res = unnormed_power_spectrum(k) *
								square(3. * (sin(k * aR) - k * aR * cos(k * aR)) / cube(k * aR));

				return res;
			};

			flt_t res = Mpc_o_h*integrate_Romberg(power_spectrum,0.001/Mpc_o_h,1000./Mpc_o_h)/(2.*square(pi));
			return res;
		,

		,

)

// Initialisation for IceBRG::l10_mass_function_cache
DEFINE_BRG_CACHE_2D( l10_mass_function_cache, flt_t, flt_t, inverse_volume_type,
		mass_func_l10_min, mass_func_l10_max, 0.01,
		0.1, 2.0, 0.01
		,
			return log10_mass_function( in_param_1, in_param_2 );
		,
			sigma_r_cache().load();
			tfa_cache().load();
		,
			sigma_r_cache().unload();
			tfa_cache().unload();
)

// Initialisation for IceBRG::l10_mass_function_integral_cache
DEFINE_BRG_CACHE_2D( l10_mass_function_integral_cache, flt_t, flt_t, inverse_volume_type,
		mass_func_l10_min, mass_func_l10_max, 0.01,
		0.1, 2.0, 0.01
		,
			auto mass_function_integrand = [&] (flt_t const & l10_m)
			{
				return l10_mass_function_cache().get(l10_m,in_param_2);
			};

			return integrate_Romberg(mass_function_integrand,in_param_1,mass_func_l10_max);
		,
			l10_mass_function_cache().load();
		,
			l10_mass_function_cache().unload();

)

// Initialisation for IceBRG::visible_cluster_density_cache
DEFINE_BRG_CACHE( visible_cluster_density_cache, flt_t, inverse_volume_type,
		0.1, 1.3, 0.01
		,
			mass_type min_mass = min_cluster_mass(in_param);
			flt_t max_abs_mag_B_central = get_abs_mag_B_from_mass(min_mass,in_param);

			lum_func_integral_cache lf_cache;

			inverse_volume_type res = lum_func_integral_cache().get(lum_func_min_abs_mag_B,max_abs_mag_B_central);

			return res;
		,
			dfa_cache().load();
			l10_mass_function_integral_cache().load();
			lum_func_integral_cache().load();
		,
			l10_mass_function_integral_cache().unload();

)

// Initialisation for IceBRG::visible_clusters_cache
DEFINE_BRG_CACHE( visible_clusters_cache, flt_t, inverse_square_angle_type,
		0.1, 1.3, 0.02
		,

			inverse_square_angle_type res = integrate_Romberg(cluster_angular_density_at_z,
					0.1,in_param);

			return res;
		,
			dfa_cache().load();
			visible_cluster_density_cache().load();
		,

)

// Initialisation for IceBRG::visible_galaxy_density_cache
DEFINE_BRG_CACHE( visible_galaxy_density_cache, flt_t, inverse_volume_type,
		0.1, 2.0, 0.02
		,
			flt_t max_abs_mag_B = max_galaxy_abs_mag_B(in_param);

			lum_func_integral_cache lf_cache;

			inverse_volume_type res = lum_func_integral_cache().get(lum_func_min_abs_mag_B,max_abs_mag_B);

			return res;
		,
			lum_func_integral_cache().load();
		,

)

// Initialisation for IceBRG::visible_galaxies_cache
DEFINE_BRG_CACHE( visible_galaxies_cache, flt_t, inverse_square_angle_type,
		0.1, 2.0, 0.02
		,

			inverse_square_angle_type res = integrate_Romberg(galaxy_angular_density_at_z,
					0.1,in_param);

			return res;
		,
			dfa_cache().load();
			visible_galaxy_density_cache().load();
		,

)

// Initialisation for IceBRG::visible_galaxy_density_cache
DEFINE_BRG_CACHE( cluster_richness_at_z_cache, flt_t, flt_t,
		0.1, 1.3, 0.02
		,
			return integrate_mean_cluster_richness_at_redshift(in_param);
		,
			dfa_cache().load();
			lum_func_integral_cache().load();
			l10_mass_function_cache().load();
		,

)

// Initialisation for IceBRG::visible_galaxies_cache
DEFINE_BRG_CACHE_2D( cluster_richness_cache, flt_t, flt_t, flt_t,
		0.1, 1.3, 0.02,
		0.1, 1.3, 0.02
		,
			return integrate_mean_cluster_richness(in_param_1,in_param_2);
		,
			dfa_cache().load();
			cluster_richness_at_z_cache().load();
			visible_cluster_density_cache().load();
		,

)

} // namespace IceBRG

#endif // end Static Class Initialisation
