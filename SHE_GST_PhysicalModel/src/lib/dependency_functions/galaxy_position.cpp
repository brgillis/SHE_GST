/**********************************************************************\
 @file galaxy_position.cpp
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

#include <cmath>

#include "SHE_GST_PhysicalModel/dependency_functions/galaxy_type.hpp"
#include "SHE_GST_PhysicalModel/dependency_functions/misc_dependencies.hpp"
#include "SHE_GST_PhysicalModel/common.hpp"

#include "SHE_GST_IceBRG_main/math/random/random_functions.hpp"
#include <SHE_GST_IceBRG_physics/lensing_tNFW_profile.hpp>

namespace SHE_GST_PhysicalModel {

using namespace IceBRG;

flt_t generate_rp( flt_t const & galaxy_type, flt_t const & cluster_mass, flt_t const & cluster_redshift,
		flt_t const & pixel_scale, gen_t & rng  )
{
	if(!is_satellite_galaxy(galaxy_type)) return 0.;

	lensing_tNFW_profile cluster_profile(cluster_mass*unitconv::Msuntokg*kg,cluster_redshift);

	distance_type R_min = cluster_profile.rvir()/10.;
	distance_type R_max = cluster_profile.rt();

	auto R_pdf = [&] (distance_type const & R)
	{
		return value_of(2*pi*R*cluster_profile.quick_Sigma(R));
	};

	distance_type R = rand_from_pdf(R_pdf,40,R_min,R_max,rng);

	angle_type theta = afd(R,cluster_redshift);

  flt_t rp = value_of(theta) * unitconv::degtorad / pixel_scale;

	return rp;
}

flt_t get_xp( flt_t const & rp, flt_t const & theta_sat, flt_t const & cluster_xp )
{
	return cluster_xp + rp * std::cos(theta_sat*M_PI/180);
}

flt_t get_yp( flt_t const & rp, flt_t const & theta_sat, flt_t const & cluster_yp )
{
	return get_xp( rp, 45-theta_sat, cluster_yp);
}

}


