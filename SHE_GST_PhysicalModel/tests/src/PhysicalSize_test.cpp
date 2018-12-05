/**********************************************************************\
 @file PhysicalSize_test.cpp
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

#include "SHE_GST_IceBRG_physics/detail/astro_caches.hpp"

#include "SHE_GST_IceBRG_main/math/misc_math.hpp"

#include "SHE_GST_PhysicalModel/common.hpp"
#include "SHE_GST_PhysicalModel/default_values.hpp"
#include "SHE_GST_PhysicalModel/param_declarations.hpp"
#include "SHE_GST_PhysicalModel/dependency_functions/galaxy_size_detail.hpp"
#include "SHE_GST_PhysicalModel/levels/Survey.hpp"
#include "SHE_GST_PhysicalModel/levels/Galaxy.hpp"

namespace SHE_GST_PhysicalModel
{

using namespace IceBRG;

struct physical_size_fixture {

	Survey survey;

  const std::string add_cache_filepath = Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/ang_di_d_cache.bin").string();
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

	int_t const num_gals = 1000;

	int_t const test_seed = 12344;

	flt_t const z_1 = 0.25;
	flt_t const z_2 = 1.25;

	flt_t const l10_ms_1 = 9.75;
	flt_t const l10_ms_2 = 11.25;

	flt_t const ex_mean_l10_r_bulge_11 = std::log10(1.9);
	flt_t const ex_mean_l10_r_bulge_12 = std::log10(8.7);
	flt_t const ex_mean_l10_r_bulge_21 = std::log10(1.7);
	flt_t const ex_mean_l10_r_bulge_22 = std::log10(4.0);

	flt_t const ex_scatter_l10_r_bulge_11 = 0.10;
	flt_t const ex_scatter_l10_r_bulge_12 = 0.10;
	flt_t const ex_scatter_l10_r_bulge_21 = 0.12;
	flt_t const ex_scatter_l10_r_bulge_22 = 0.12;

	flt_t const ex_mean_l10_r_disk_11 = std::log10(4.1);
	flt_t const ex_mean_l10_r_disk_12 = std::log10(9.8);
	flt_t const ex_mean_l10_r_disk_21 = std::log10(3.2);
	flt_t const ex_mean_l10_r_disk_22 = std::log10(6.3);

	flt_t const ex_scatter_l10_r_disk_11 = 0.16;
	flt_t const ex_scatter_l10_r_disk_12 = 0.16;
	flt_t const ex_scatter_l10_r_disk_21 = 0.17;
	flt_t const ex_scatter_l10_r_disk_22 = 0.17;

	flt_t const tol_mean = 10; // Since I'm guessing from a graph here
	flt_t const tol_scatter = 5; // More reliable, but more noisy

};


BOOST_AUTO_TEST_SUITE (PhysicalSize_Test)

BOOST_FIXTURE_TEST_CASE(test_physical_size, physical_size_fixture) {

  IceBRG::add_cache().set_file_name(add_cache_filepath);
  IceBRG::cluster_richness_cache().set_file_name(crich_cache_filepath);
  IceBRG::cluster_richness_at_z_cache().set_file_name(crichz_cache_filepath);
  IceBRG::dfa_cache().set_file_name(dfa_cache_filepath);
  IceBRG::lum_func_integral_cache().set_file_name(lum_int_cache_filepath);
  IceBRG::l10_mass_function_cache().set_file_name(massfunc_cache_filepath);
  IceBRG::l10_mass_function_integral_cache().set_file_name(mass_int_cache_filepath);
  IceBRG::sigma_r_cache().set_file_name(sigma_r_cache_filepath);
  IceBRG::tfa_cache().set_file_name(tfa_cache_filepath);
  IceBRG::visible_cluster_density_cache().set_file_name(viscdens_cache_filepath);
  IceBRG::visible_clusters_cache().set_file_name(vis_clus_cache_filepath);
  IceBRG::visible_galaxy_density_cache ().set_file_name(visgdens_cache_filepath);
  IceBRG::visible_galaxies_cache ().set_file_name(vis_gal_cache_filepath);

	// Now see if we can inherit all the way down to the galaxy level
	survey.set_param_params(num_clusters_name,"fixed",0.);
	survey.set_param_params(num_field_galaxies_name,"fixed",flt_t(num_gals));

	survey.set_generation_level(redshift_name,dv::survey_level);
	survey.set_generation_level(stellar_mass_name,dv::survey_level);

	survey.set_seed(test_seed);

	survey.autofill_children();

	auto galaxies = survey.get_galaxy_descendants();

	gal_size_array_t l10_r_bulge_array = gal_size_array_t::Zero(num_gals);
	gal_size_array_t l10_r_disk_array = gal_size_array_t::Zero(num_gals);

	// Test for each set of redshift and stellar mass

	// 1-1

	survey.set_param_params(redshift_name,"fixed",z_1);
	survey.set_param_params(stellar_mass_name,"fixed",std::pow(10.,l10_ms_1));

	for( int_t i=0; i<num_gals; ++i )
	{
		l10_r_bulge_array[i] = std::log10(galaxies.at(i)->get_param_value(physical_size_bulge_name));
		l10_r_disk_array[i] = std::log10(galaxies.at(i)->get_param_value(physical_size_disk_name));
	}

	flt_t mean_r_bulge = l10_r_bulge_array.mean();
	flt_t mean_r_disk = l10_r_disk_array.mean();

	flt_t scatter_r_bulge = std::sqrt(square(l10_r_bulge_array).mean() - square(l10_r_bulge_array.mean()));
	flt_t scatter_r_disk = std::sqrt(square(l10_r_disk_array).mean() - square(l10_r_disk_array.mean()));

	BOOST_CHECK_CLOSE(mean_r_bulge,ex_mean_l10_r_bulge_11,tol_mean);
	BOOST_CHECK_CLOSE(mean_r_disk,ex_mean_l10_r_disk_11,tol_mean);

	BOOST_CHECK_CLOSE(scatter_r_bulge,ex_scatter_l10_r_bulge_11,tol_scatter);
	BOOST_CHECK_CLOSE(scatter_r_disk,ex_scatter_l10_r_disk_11,tol_scatter);

	// 1-2

	survey.set_param_params(redshift_name,"fixed",z_1);
	survey.set_param_params(stellar_mass_name,"fixed",std::pow(10.,l10_ms_2));

	for( int_t i=0; i<num_gals; ++i )
	{
		l10_r_bulge_array[i] = std::log10(galaxies.at(i)->get_param_value(physical_size_bulge_name));
		l10_r_disk_array[i] = std::log10(galaxies.at(i)->get_param_value(physical_size_disk_name));
	}

	mean_r_bulge = l10_r_bulge_array.mean();
	mean_r_disk = l10_r_disk_array.mean();

	scatter_r_bulge = std::sqrt(square(l10_r_bulge_array).mean() - square(l10_r_bulge_array.mean()));
	scatter_r_disk = std::sqrt(square(l10_r_disk_array).mean() - square(l10_r_disk_array.mean()));

	BOOST_CHECK_CLOSE(mean_r_bulge,ex_mean_l10_r_bulge_12,tol_mean);
	BOOST_CHECK_CLOSE(mean_r_disk,ex_mean_l10_r_disk_12,tol_mean);

	BOOST_CHECK_CLOSE(scatter_r_bulge,ex_scatter_l10_r_bulge_12,tol_scatter);
	BOOST_CHECK_CLOSE(scatter_r_disk,ex_scatter_l10_r_disk_12,tol_scatter);

	// 2-1

	survey.set_param_params(redshift_name,"fixed",z_2);
	survey.set_param_params(stellar_mass_name,"fixed",std::pow(10.,l10_ms_1));

	for( int_t i=0; i<num_gals; ++i )
	{
		l10_r_bulge_array[i] = std::log10(galaxies.at(i)->get_param_value(physical_size_bulge_name));
		l10_r_disk_array[i] = std::log10(galaxies.at(i)->get_param_value(physical_size_disk_name));
	}

	mean_r_bulge = l10_r_bulge_array.mean();
	mean_r_disk = l10_r_disk_array.mean();

	scatter_r_bulge = std::sqrt(square(l10_r_bulge_array).mean() - square(l10_r_bulge_array.mean()));
	scatter_r_disk = std::sqrt(square(l10_r_disk_array).mean() - square(l10_r_disk_array.mean()));

	BOOST_CHECK_CLOSE(mean_r_bulge,ex_mean_l10_r_bulge_21,tol_mean);
	BOOST_CHECK_CLOSE(mean_r_disk,ex_mean_l10_r_disk_21,tol_mean);

	BOOST_CHECK_CLOSE(scatter_r_bulge,ex_scatter_l10_r_bulge_21,tol_scatter);
	BOOST_CHECK_CLOSE(scatter_r_disk,ex_scatter_l10_r_disk_21,tol_scatter);

	// 2-2

	survey.set_param_params(redshift_name,"fixed",z_2);
	survey.set_param_params(stellar_mass_name,"fixed",std::pow(10.,l10_ms_2));

	for( int_t i=0; i<num_gals; ++i )
	{
		l10_r_bulge_array[i] = std::log10(galaxies.at(i)->get_param_value(physical_size_bulge_name));
		l10_r_disk_array[i] = std::log10(galaxies.at(i)->get_param_value(physical_size_disk_name));
	}

	mean_r_bulge = l10_r_bulge_array.mean();
	mean_r_disk = l10_r_disk_array.mean();

	scatter_r_bulge = std::sqrt(square(l10_r_bulge_array).mean() - square(l10_r_bulge_array.mean()));
	scatter_r_disk = std::sqrt(square(l10_r_disk_array).mean() - square(l10_r_disk_array.mean()));

	BOOST_CHECK_CLOSE(mean_r_bulge,ex_mean_l10_r_bulge_22,tol_mean);
	BOOST_CHECK_CLOSE(mean_r_disk,ex_mean_l10_r_disk_22,tol_mean);

	BOOST_CHECK_CLOSE(scatter_r_bulge,ex_scatter_l10_r_bulge_22,tol_scatter);
	BOOST_CHECK_CLOSE(scatter_r_disk,ex_scatter_l10_r_disk_22,tol_scatter);

}

BOOST_AUTO_TEST_SUITE_END ()

} // namespace SHE_GST_PhysicalModel
