/**********************************************************************\
 @file PHL_autofill_children_test.cpp
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
#include "SHE_GST_IceBRG_main/units/unit_conversions.hpp"
#include "SHE_GST_IceBRG_physics/cluster_visibility.hpp"
#include "SHE_GST_IceBRG_physics/galaxy_visibility.hpp"

#include "SHE_GST_PhysicalModel/common.hpp"
#include "SHE_GST_PhysicalModel/levels/Cluster.hpp"
#include "SHE_GST_PhysicalModel/levels/Field.hpp"
#include "SHE_GST_PhysicalModel/levels/Galaxy.hpp"
#include "SHE_GST_PhysicalModel/levels/Image.hpp"
#include "SHE_GST_PhysicalModel/levels/ImageGroup.hpp"
#include "SHE_GST_PhysicalModel/levels/Survey.hpp"
#include "SHE_GST_PhysicalModel/default_values.hpp"

namespace SHE_GST_PhysicalModel
{

using namespace IceBRG;

struct PHL_autofill_children_fixture {

	Survey survey1;

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

	flt_t ex_satellites = mean_cluster_richness(0.2,1.3) - 1;
	flt_t ex_fields = dv::num_fields;
    flt_t ex_image_groups = dv::num_image_groups;
	flt_t ex_images = dv::num_images;

	flt_t accepted_sigma = 3.;

	flt_t ex_clusters = dv::image_size_xp * dv::image_size_yp * square(dv::pixel_scale/60.) *
			visible_clusters(1.*square(unitconv::amintorad*rad));
	flt_t ex_clusters_min = ex_clusters - accepted_sigma * std::sqrt(ex_clusters);
	flt_t ex_clusters_max = ex_clusters + accepted_sigma * std::sqrt(ex_clusters);

	flt_t ex_cgs = IceBRG::mean_cluster_richness(0.2,1.3);
	flt_t ex_cgs_min = ex_cgs - accepted_sigma * std::sqrt(ex_cgs-1); // - 1 to exclude central from variation
	flt_t ex_cgs_max = ex_cgs + accepted_sigma * std::sqrt(ex_cgs-1); // - 1 to exclude central from variation

	flt_t ex_fgs = dv::image_size_xp * dv::image_size_yp * square(dv::pixel_scale/60.) *
			visible_galaxies(1.*square(unitconv::amintorad*rad)) -
			ex_clusters*ex_cgs;
	flt_t ex_fgs_min = ex_fgs - accepted_sigma * std::sqrt(ex_fgs);
	flt_t ex_fgs_max = ex_fgs + accepted_sigma * std::sqrt(ex_fgs);

  flt_t ex_wcs_g1 = 0;
  flt_t ex_wcs_g2 = 0;
  flt_t ex_wcs_theta = 0;
};


BOOST_AUTO_TEST_SUITE (PHL_Autofill_Children_Test)

BOOST_FIXTURE_TEST_CASE(test_PHL_autofill_children, PHL_autofill_children_fixture) {

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

	BOOST_CHECK_NO_THROW(survey1.autofill_children());

	BOOST_CHECK_EQUAL(survey1.num_children(),ex_image_groups);

  BOOST_CHECK_EQUAL(survey1.get_param_value("wcs_g1"),ex_wcs_g1);
  BOOST_CHECK_EQUAL(survey1.get_param_value("wcs_g2"),ex_wcs_g2);
  BOOST_CHECK_EQUAL(survey1.get_param_value("wcs_theta"),ex_wcs_theta);

	ImageGroup * p_image_group1;
	BOOST_CHECK_NO_THROW(p_image_group1 = survey1.get_children<ImageGroup>().at(0));

    BOOST_CHECK_EQUAL(p_image_group1->num_children(),ex_image_groups);

    Image * p_image1;
    BOOST_CHECK_NO_THROW(p_image1 = p_image_group1->get_children<Image>().at(0));

	auto fields = p_image1->get_children<Field>();
	BOOST_CHECK_EQUAL(static_cast<int_t>(fields.size()),ex_fields);

	auto fgs = fields.at(0)->get_children<Galaxy>();
	BOOST_CHECK_GE(static_cast<int_t>(fgs.size()),ex_fgs_min);
	BOOST_CHECK_LE(static_cast<int_t>(fgs.size()),ex_fgs_max);

	auto clusters = p_image1->get_children<Cluster>();
	BOOST_CHECK_GE(static_cast<int_t>(clusters.size()),ex_clusters_min);
	BOOST_CHECK_LE(static_cast<int_t>(clusters.size()),ex_clusters_max);

	auto cgs = clusters.at(0)->get_children<Galaxy>();
	BOOST_CHECK_GE(static_cast<int_t>(cgs.size()),ex_cgs_min);
	BOOST_CHECK_LE(static_cast<int_t>(cgs.size()),ex_cgs_max);

}

BOOST_AUTO_TEST_SUITE_END ()

} // namespace SHE_GST_PhysicalModel
