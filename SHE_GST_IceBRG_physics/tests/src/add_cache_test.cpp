/**********************************************************************\
 @file add_cache_test.cpp
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

#include "SHE_GST_IceBRG_main/units/units.hpp"
#include "SHE_GST_IceBRG_main/units/unit_conversions.hpp"
#include "SHE_GST_IceBRG_physics/detail/astro_caches.hpp"

using namespace IceBRG;

BOOST_AUTO_TEST_SUITE (ADD_Cache_Test)

BOOST_AUTO_TEST_CASE( add_cache_test )
{
  add_cache cache;

  cache.set_file_name("tmp_add_cache.bin");

  cache.set_range(0,0.4,0.2,0,0.8,0.2);

  auto add_00 = cache.get(0,0);
  auto add_03 = cache.get(0,0.3);
  auto add_30 = cache.get(0.3,0);
  auto add_33 = cache.get(0.3,0.3);

  auto add_06 = cache.get(0,0.6);
  auto add_36 = cache.get(0,0.6);

  // Sanity checks

  BOOST_CHECK_SMALL(value_of(add_00),0.001*unitconv::Mpctom);
  BOOST_CHECK_SMALL(value_of(add_33),0.001*unitconv::Mpctom);
  BOOST_CHECK_EQUAL(value_of(add_03),-value_of(add_30));

  BOOST_CHECK_GT(value_of(add_06),value_of(add_03));
  BOOST_CHECK_LT(value_of(add_06),2*value_of(add_03));

  BOOST_CHECK_LT(value_of(add_06),value_of(add_03)+value_of(add_36));

  // Allow 10% closeness here, due to coarseness of cache in this test
  BOOST_CHECK_CLOSE(value_of(add_03),921.2*unitconv::Mpctom,10);
  BOOST_CHECK_CLOSE(value_of(add_06),1385.7*unitconv::Mpctom,10);

}

BOOST_AUTO_TEST_SUITE_END()


