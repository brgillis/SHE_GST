/**********************************************************************\
 @file tfa_cache_test.cpp
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
#include "SHE_GST_IceBRG_physics/detail/astro_caches.hpp"

using namespace IceBRG;

BOOST_AUTO_TEST_SUITE (TFA_Cache_Test)

BOOST_AUTO_TEST_CASE( tfa_cache_test )
{
    tfa_cache cache;

  cache.set_file_name("tmp_tfa_cache.bin");

    cache.set_range(0.1,1,0.1);

    auto time_1 = cache.get(0.95);
    auto time_2 = cache.get(0.85);

    BOOST_CHECK_LT(value_of(time_2),value_of(time_1));

    // Allow 10% closeness here, due to coarseness of cache in this test
    BOOST_CHECK_CLOSE(value_of(time_1),value_of(-0.708*unitconv::Gyrtos*s),10);
    BOOST_CHECK_CLOSE(value_of(time_2),value_of(-2.183*unitconv::Gyrtos*s),10);

    auto a_1 = cache.inverse_get(time_1);
    auto a_2 = cache.inverse_get(time_2);

    BOOST_CHECK_CLOSE(value_of(a_1),0.95,0.1);
    BOOST_CHECK_CLOSE(value_of(a_2),0.85,0.1);
}

BOOST_AUTO_TEST_SUITE_END()


