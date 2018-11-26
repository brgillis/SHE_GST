/**********************************************************************\
 @file mass_func_test.cpp
 ------------------

 TODO <Insert file description here>

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <ElementsKernel/Auxiliary.h>


#include "SHE_GST_IceBRG_main/math/calculus/integrate.hpp"
#include "SHE_GST_IceBRG_main/units/units.hpp"
#include "SHE_GST_IceBRG_main/units/unit_conversions.hpp"

#include "SHE_GST_IceBRG_physics/detail/astro_caches.hpp"
#include "SHE_GST_IceBRG_physics/cluster_visibility.hpp"
#include "SHE_GST_IceBRG_physics/cosmology.hpp"
#include "SHE_GST_IceBRG_physics/galaxy_visibility.hpp"
#include "SHE_GST_IceBRG_physics/mass_function.hpp"

using namespace IceBRG;

struct mass_func_fixture {

  const std::string add_cache_filepath =
      Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/ang_di_d_cache.bin").string();
  const std::string crich_cache_filepath = Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/crich_cache.bin").string();
  const std::string crichz_cache_filepath = Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/crichz_cache.bin").string();
  const std::string dfa_cache_filepath = Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/dfa_cache.bin").string();
  const std::string lum_int_cache_filepath = Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/lum_int_cache.bin").string();
  const std::string massfunc_cache_filepath = Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/massfunc_cache.bin").string();
  const std::string mass_int_cache_filepath = Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/mass_int_cache.bin").string();
  const std::string sigma_r_cache_filepath = Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/sigma_r_cache.bin").string();
  const std::string tfa_cache_filepath = Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/tfa_cache.bin").string();
  const std::string viscdens_cache_filepath = Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/viscdens_cache.bin").string();
  const std::string vis_clus_cache_filepath = Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/vis_clus_cache.bin").string();
  const std::string vis_gal_cache_filepath = Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/vis_gal_cache.bin").string();
  const std::string visgdens_cache_filepath = Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/visgdens_cache.bin").string();

};

BOOST_AUTO_TEST_SUITE (Mass_Func_Test)

BOOST_FIXTURE_TEST_CASE( vis_galaxies_test, mass_func_fixture )
{

  IceBRG::add_cache ()
  .set_file_name(add_cache_filepath);
  IceBRG::cluster_richness_cache ()
  .set_file_name(crich_cache_filepath);
  IceBRG::cluster_richness_at_z_cache ()
  .set_file_name(crichz_cache_filepath);
  IceBRG::dfa_cache ()
  .set_file_name(dfa_cache_filepath);
  IceBRG::lum_func_integral_cache ()
  .set_file_name(lum_int_cache_filepath);
  IceBRG::l10_mass_function_cache ()
  .set_file_name(massfunc_cache_filepath);
  IceBRG::l10_mass_function_integral_cache ()
  .set_file_name(mass_int_cache_filepath);
  IceBRG::sigma_r_cache ()
  .set_file_name(sigma_r_cache_filepath);
  IceBRG::tfa_cache ()
  .set_file_name(tfa_cache_filepath);
  IceBRG::visible_cluster_density_cache ()
  .set_file_name(viscdens_cache_filepath);
  IceBRG::visible_clusters_cache ()
  .set_file_name(vis_clus_cache_filepath);
  IceBRG::visible_galaxy_density_cache ()
  .set_file_name(vis_gal_cache_filepath);
  IceBRG::visible_galaxies_cache ()
  .set_file_name(visgdens_cache_filepath);

	square_angle_type area = 1.*square(unitconv::amintorad*rad);

	galaxy_angular_density_at_z(0.2);

	flt_t n = visible_galaxies(area);

	BOOST_CHECK_GT(n,50);
	BOOST_CHECK_LT(n,500);
}

BOOST_FIXTURE_TEST_CASE( mass_func_test, mass_func_fixture )
{

  IceBRG::add_cache ()
  .set_file_name(add_cache_filepath);
  IceBRG::cluster_richness_cache ()
  .set_file_name(crich_cache_filepath);
  IceBRG::cluster_richness_at_z_cache ()
  .set_file_name(crichz_cache_filepath);
  IceBRG::dfa_cache ()
  .set_file_name(dfa_cache_filepath);
  IceBRG::lum_func_integral_cache ()
  .set_file_name(lum_int_cache_filepath);
  IceBRG::l10_mass_function_cache ()
  .set_file_name(massfunc_cache_filepath);
  IceBRG::l10_mass_function_integral_cache ()
  .set_file_name(mass_int_cache_filepath);
  IceBRG::sigma_r_cache ()
  .set_file_name(sigma_r_cache_filepath);
  IceBRG::tfa_cache ()
  .set_file_name(tfa_cache_filepath);
  IceBRG::visible_cluster_density_cache ()
  .set_file_name(viscdens_cache_filepath);
  IceBRG::visible_clusters_cache ()
  .set_file_name(vis_clus_cache_filepath);
  IceBRG::visible_galaxy_density_cache ()
  .set_file_name(vis_gal_cache_filepath);
  IceBRG::visible_galaxies_cache ()
  .set_file_name(visgdens_cache_filepath);

	flt_t z = 0;

	volume_type Gpc_cubed = cube(1e9*unitconv::pctom*m);

	flt_t log10msun_m1 = 8;
	flt_t log10msun_m2 = 10;
	flt_t log10msun_m3 = 12;
	flt_t log10msun_m4 = 14;
	flt_t log10msun_m5 = 16;

	// Try for z=0

	flt_t n1 = log10_mass_function(log10msun_m1,z)*Gpc_cubed;
	flt_t n2 = log10_mass_function(log10msun_m2,z)*Gpc_cubed;
	flt_t n3 = log10_mass_function(log10msun_m3,z)*Gpc_cubed;
	flt_t n4 = log10_mass_function(log10msun_m4,z)*Gpc_cubed;
	flt_t n5 = log10_mass_function(log10msun_m5,z)*Gpc_cubed;

	BOOST_CHECK_LT(value_of(n2),value_of(n1));
	BOOST_CHECK_LT(value_of(n3),value_of(n2));
	BOOST_CHECK_LT(value_of(n4),value_of(n3));
	BOOST_CHECK_LT(value_of(n5),value_of(n4));

	auto density_at_scale_0 = [&] (flt_t const & log10_mass)
	{
		return log10_mass_function(log10_mass,z)*std::pow(10.,log10_mass)*unitconv::Msuntokg*kg;
	};

	density_type dens_0 = integrate_Romberg(density_at_scale_0,0.,16.);

	density_type matter_dens = rho_bar(0.);

	BOOST_CHECK_GE(value_of(dens_0),0.5*value_of(matter_dens));
	BOOST_CHECK_LE(value_of(dens_0),value_of(matter_dens));

	// Try for z=0.9

	z = 0.9;

	n1 = log10_mass_function(log10msun_m1,z)*Gpc_cubed;
	n2 = log10_mass_function(log10msun_m2,z)*Gpc_cubed;
	n3 = log10_mass_function(log10msun_m3,z)*Gpc_cubed;
	n4 = log10_mass_function(log10msun_m4,z)*Gpc_cubed;
	n5 = log10_mass_function(log10msun_m5,z)*Gpc_cubed;

	BOOST_CHECK_LT(value_of(n2),value_of(n1));
	BOOST_CHECK_LT(value_of(n3),value_of(n2));
	BOOST_CHECK_LT(value_of(n4),value_of(n3));
	BOOST_CHECK_LT(value_of(n5),value_of(n4));

	auto density_at_scale_0_9 = [&] (flt_t const & log10_mass)
	{
		return log10_mass_function(log10_mass,z)*std::pow(10.,log10_mass)*unitconv::Msuntokg*kg;
	};

	density_type dens_0_9 = integrate_Romberg(density_at_scale_0_9,0.,16.);

	matter_dens = rho_bar(0.);

	// Between half and 100% of matter should be in collapsed haloes
	BOOST_CHECK_GE(value_of(dens_0_9),0.5*value_of(matter_dens));
	BOOST_CHECK_LE(value_of(dens_0_9),value_of(matter_dens));

	// Less collapsed now than at z=0
	BOOST_CHECK_LE(value_of(dens_0_9),value_of(dens_0));

	// Try for z=2.0

	z = 2.0;

	n1 = log10_mass_function(log10msun_m1,z)*Gpc_cubed;
	n2 = log10_mass_function(log10msun_m2,z)*Gpc_cubed;
	n3 = log10_mass_function(log10msun_m3,z)*Gpc_cubed;
	n4 = log10_mass_function(log10msun_m4,z)*Gpc_cubed;
	n5 = log10_mass_function(log10msun_m5,z)*Gpc_cubed;

	BOOST_CHECK_LT(value_of(n2),value_of(n1));
	BOOST_CHECK_LT(value_of(n3),value_of(n2));
	BOOST_CHECK_LT(value_of(n4),value_of(n3));
	BOOST_CHECK_LT(value_of(n5),value_of(n4));

	auto density_at_scale_2_0 = [&] (flt_t const & log10_mass)
	{
		return log10_mass_function(log10_mass,z)*std::pow(10.,log10_mass)*unitconv::Msuntokg*kg;
	};

	density_type dens_2_0 = integrate_Romberg(density_at_scale_2_0,0.,16.);

	matter_dens = rho_bar(0.);

	// Between half and 100% of matter should be in collapsed haloes
	BOOST_CHECK_GE(value_of(dens_2_0),0.5*value_of(matter_dens));
	BOOST_CHECK_LE(value_of(dens_2_0),value_of(matter_dens));

	// Less collapsed now than at z=0
	BOOST_CHECK_LE(value_of(dens_2_0),value_of(dens_0_9));
}

BOOST_FIXTURE_TEST_CASE( vis_clusters_test, mass_func_fixture )
{

  IceBRG::add_cache ()
  .set_file_name(add_cache_filepath);
  IceBRG::cluster_richness_cache ()
  .set_file_name(crich_cache_filepath);
  IceBRG::cluster_richness_at_z_cache ()
  .set_file_name(crichz_cache_filepath);
  IceBRG::dfa_cache ()
  .set_file_name(dfa_cache_filepath);
  IceBRG::lum_func_integral_cache ()
  .set_file_name(lum_int_cache_filepath);
  IceBRG::l10_mass_function_cache ()
  .set_file_name(massfunc_cache_filepath);
  IceBRG::l10_mass_function_integral_cache ()
  .set_file_name(mass_int_cache_filepath);
  IceBRG::sigma_r_cache ()
  .set_file_name(sigma_r_cache_filepath);
  IceBRG::tfa_cache ()
  .set_file_name(tfa_cache_filepath);
  IceBRG::visible_cluster_density_cache ()
  .set_file_name(viscdens_cache_filepath);
  IceBRG::visible_clusters_cache ()
  .set_file_name(vis_clus_cache_filepath);
  IceBRG::visible_galaxy_density_cache ()
  .set_file_name(vis_gal_cache_filepath);
  IceBRG::visible_galaxies_cache ()
  .set_file_name(visgdens_cache_filepath);

	square_angle_type area = 1.*square(unitconv::amintorad*rad);

	flt_t n = visible_clusters(area);

	BOOST_CHECK_GT(n,1);
	BOOST_CHECK_LT(n,10);
}

BOOST_FIXTURE_TEST_CASE( cluster_richness_test, mass_func_fixture )
{

  IceBRG::add_cache ()
  .set_file_name(add_cache_filepath);
  IceBRG::cluster_richness_cache ()
  .set_file_name(crich_cache_filepath);
  IceBRG::cluster_richness_at_z_cache ()
  .set_file_name(crichz_cache_filepath);
  IceBRG::dfa_cache ()
  .set_file_name(dfa_cache_filepath);
  IceBRG::lum_func_integral_cache ()
  .set_file_name(lum_int_cache_filepath);
  IceBRG::l10_mass_function_cache ()
  .set_file_name(massfunc_cache_filepath);
  IceBRG::l10_mass_function_integral_cache ()
  .set_file_name(mass_int_cache_filepath);
  IceBRG::sigma_r_cache ()
  .set_file_name(sigma_r_cache_filepath);
  IceBRG::tfa_cache ()
  .set_file_name(tfa_cache_filepath);
  IceBRG::visible_cluster_density_cache ()
  .set_file_name(viscdens_cache_filepath);
  IceBRG::visible_clusters_cache ()
  .set_file_name(vis_clus_cache_filepath);
  IceBRG::visible_galaxy_density_cache ()
  .set_file_name(vis_gal_cache_filepath);
  IceBRG::visible_galaxies_cache ()
  .set_file_name(visgdens_cache_filepath);

	flt_t z1 = 0.2;
	flt_t z2 = 0.23;

	flt_t mean_richness = mean_cluster_richness(z1,z2);

	BOOST_CHECK_GT(mean_richness,2);
	BOOST_CHECK_LT(mean_richness,10);
}

BOOST_AUTO_TEST_SUITE_END()


