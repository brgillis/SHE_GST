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

#include "SHE_GST_IceBRG_main/math/calculus/integrate.hpp"
#include "SHE_GST_IceBRG_main/units/units.hpp"
#include "SHE_GST_IceBRG_main/units/unit_conversions.hpp"

#include "SHE_GST_IceBRG_physics/cluster_visibility.hpp"
#include "SHE_GST_IceBRG_physics/cosmology.hpp"
#include "SHE_GST_IceBRG_physics/galaxy_visibility.hpp"
#include "SHE_GST_IceBRG_physics/mass_function.hpp"

using namespace IceBRG;

BOOST_AUTO_TEST_SUITE (Mass_Func_Test)

BOOST_AUTO_TEST_CASE( vis_galaxies_test )
{
	square_angle_type area = 1.*square(unitconv::amintorad*rad);

	galaxy_angular_density_at_z(0.2);

	flt_t n = visible_galaxies(area);

	BOOST_CHECK_GT(n,50);
	BOOST_CHECK_LT(n,500);
}

BOOST_AUTO_TEST_CASE( mass_func_test )
{
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

BOOST_AUTO_TEST_CASE( vis_clusters_test )
{
	square_angle_type area = 1.*square(unitconv::amintorad*rad);

	flt_t n = visible_clusters(area);

	BOOST_CHECK_GT(n,1);
	BOOST_CHECK_LT(n,10);
}

BOOST_AUTO_TEST_CASE( cluster_richness_test )
{
	flt_t z1 = 0.2;
	flt_t z2 = 0.23;

	flt_t mean_richness = mean_cluster_richness(z1,z2);

	BOOST_CHECK_GT(mean_richness,2);
	BOOST_CHECK_LT(mean_richness,10);
}

BOOST_AUTO_TEST_SUITE_END()


