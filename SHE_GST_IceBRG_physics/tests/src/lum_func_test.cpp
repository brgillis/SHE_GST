/**********************************************************************\
 @file lum_func_test.cpp
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
#include "SHE_GST_IceBRG_main/units/units.hpp"
#include "SHE_GST_IceBRG_main/units/unit_conversions.hpp"
#include "SHE_GST_IceBRG_physics/luminosity.hpp"

using namespace IceBRG;

BOOST_AUTO_TEST_SUITE (Lum_Func_Test)

BOOST_AUTO_TEST_CASE( lum_func_test )
{

  const std::string add_cache_filepath =
  Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/ang_di_d_cache.bin").string();
  const std::string crich_cache_filepath =
      Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/crich_cache.bin").string();
  const std::string crichz_cache_filepath =
      Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/crichz_cache.bin").string();
  const std::string dfa_cache_filepath = Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/dfa_cache.bin").string();
  const std::string lum_int_cache_filepath =
      Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/lum_int_cache.bin").string();
  const std::string massfunc_cache_filepath =
      Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/massfunc_cache.bin").string();
  const std::string mass_int_cache_filepath =
      Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/mass_int_cache.bin").string();
  const std::string sigma_r_cache_filepath =
      Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/sigma_r_cache.bin").string();
  const std::string tfa_cache_filepath = Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/tfa_cache.bin").string();
  const std::string viscdens_cache_filepath =
      Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/viscdens_cache.bin").string();
  const std::string vis_clus_cache_filepath =
      Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/vis_clus_cache.bin").string();
  const std::string vis_gal_cache_filepath =
      Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/vis_gal_cache.bin").string();
  const std::string visgdens_cache_filepath =
      Elements::getAuxiliaryPath("SHE_GST_IceBRG_physics/visgdens_cache.bin").string();

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
  .set_file_name(visgdens_cache_filepath);
  IceBRG::visible_galaxies_cache ()
  .set_file_name(vis_gal_cache_filepath);



	flt_t fb_ratio_1 = faint_bright_ratio(0.1);
	flt_t fb_ratio_2 = faint_bright_ratio(0.2);
	flt_t fb_ratio_9 = faint_bright_ratio(0.9);

	BOOST_CHECK_GE(fb_ratio_1,1);
	BOOST_CHECK_GE(fb_ratio_2,1);
	BOOST_CHECK_GE(fb_ratio_9,1);

	BOOST_CHECK_LE(fb_ratio_2,fb_ratio_1);
	BOOST_CHECK_LE(fb_ratio_9,fb_ratio_2);

}

BOOST_AUTO_TEST_SUITE_END()


