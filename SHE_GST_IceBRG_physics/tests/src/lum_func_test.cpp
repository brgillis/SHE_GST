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

#include "SHE_GST_IceBRG_main/units/units.hpp"
#include "SHE_GST_IceBRG_main/units/unit_conversions.hpp"
#include "SHE_GST_IceBRG_physics/luminosity.hpp"

using namespace IceBRG;

BOOST_AUTO_TEST_SUITE (Lum_Func_Test)

BOOST_AUTO_TEST_CASE( lum_func_test )
{
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


