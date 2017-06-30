/**********************************************************************\
 @file expected_count_cache.cpp
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


#include "SHE_GST_IceBRG_lensing/magnification/detail/expected_count_cache.hpp"

#include <cstdlib>

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_lensing/magnification/detail/expected_count_loader.hpp"
#include "SHE_GST_IceBRG_lensing/magnification/mag_global_values.hpp"


namespace IceBRG {

// Initialise the cache
DEFINE_BRG_CACHE_2D( expected_count_cache,
		flt_t,flt_t,decltype(custom_unit_type<0,0,0,-2,0>()),
		mag_m_min,mag_m_max,0.01,
		mag_z_min,mag_z_max-0.01,0.01,

			const flt_t & m = std::fabs(in_param_1);
			const long_flt_t z_min = std::fabs(in_param_2);

			return expected_count_loader::get_count(m,z_min);
		,
			expected_count_loader::get_count(0,0);
		,

)

} // namespace IceBRG
