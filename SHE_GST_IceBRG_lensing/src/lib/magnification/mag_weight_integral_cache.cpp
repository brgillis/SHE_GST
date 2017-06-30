/**********************************************************************\
 @file mag_weight_integral_cache.cpp
 ------------------

 TODO <Insert file description here>

 **********************************************************************

 Copyright (C) 2014  Bryan R. Gillis

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


#include "SHE_GST_IceBRG_lensing/magnification/detail/mag_weight_integral_cache.hpp"

#include <cstdlib>

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/math/calculus/integrate.hpp"

#include "SHE_GST_IceBRG_lensing/magnification/detail/expected_count_cache.hpp"
#include "SHE_GST_IceBRG_lensing/magnification/detail/magnification_functors.hpp"
#include "SHE_GST_IceBRG_lensing/magnification/mag_global_values.hpp"

namespace IceBRG {

// Initialise the cache
DEFINE_BRG_CACHE( mag_weight_integral_cache,
		flt_t, decltype(custom_unit_type<0,0,0,-2,0>()),
		0.2,mag_z_max-0.01,0.01
		,
			mu_weight_integration_functor func(in_param);

			return integrate_Romberg(func,mag_m_min,mag_m_max,
					0.000001);
		,
			expected_count_cache().load();
		,
)

} // namespace IceBRG
