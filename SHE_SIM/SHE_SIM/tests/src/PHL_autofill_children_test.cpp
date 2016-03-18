/**********************************************************************\
 @file PHL_autofill_children_test.cpp
 ------------------

 TODO <Insert file description here>

 **********************************************************************

 Copyright (C) 2015 brg

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "IceBRG_main/math/misc_math.hpp"
#include "IceBRG_main/units/unit_conversions.hpp"
#include "IceBRG_physics/cluster_visibility.hpp"
#include "IceBRG_physics/galaxy_visibility.hpp"

#include "../SHE_SIM/common.hpp"
#include "../SHE_SIM/levels/Cluster.hpp"
#include "../SHE_SIM/levels/Field.hpp"
#include "../SHE_SIM/levels/Galaxy.hpp"
#include "../SHE_SIM/levels/Image.hpp"
#include "../SHE_SIM/levels/Survey.hpp"
#include "../SHE_SIM/default_values.hpp"

namespace SHE_SIM
{

using namespace IceBRG;

struct PHL_autofill_children_fixture {

	Survey survey1;

	flt_t ex_satellites = mean_cluster_richness(0.2,1.3) - 1;
	flt_t ex_fields = dv::num_fields;
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
};


BOOST_AUTO_TEST_SUITE (PHL_Autofill_Children_Test)

BOOST_FIXTURE_TEST_CASE(test_PHL_autofill_children, PHL_autofill_children_fixture) {

	BOOST_CHECK_NO_THROW(survey1.autofill_children());

	BOOST_CHECK_EQUAL(survey1.num_children(),ex_images);

	Image * p_image1;
	BOOST_CHECK_NO_THROW(p_image1 = survey1.get_children<Image>().at(0));

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

} // namespace SHE_SIM
