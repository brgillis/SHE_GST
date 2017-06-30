/**********************************************************************\
 @file shifting_cache.cpp
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


#include "SHE_GST_IceBRG_lensing/shifting/detail/shifting_cache.hpp"

#include <cstdlib>

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/math/safe_math.hpp"

#include "SHE_GST_IceBRG_main/units/unit_conversions.hpp"

#include "SHE_GST_IceBRG_lensing/shifting/detail/shifting_loader.hpp"

// Initialise the cache

namespace IceBRG {

DEFINE_BRG_CACHE_2D(shifting_cache,
		flt_t,flt_t,angle_type,
		std::log(0.1*IceBRG::unitconv::amintorad),std::log(3800.*IceBRG::unitconv::amintorad),
				(std::log(3800.)-std::log(0.1))/1000,
		0.3,1.5,0.1
		,
			const angle_type & t = units_cast<angle_type>(std::exp(value_of(in_param_1)));
			const flt_t & z = std::fabs(in_param_2);

			if(value_of(t)==0) return units_cast<angle_type>(0);

			return safe_sqrt(shifting_loader().get(0,z)-shifting_loader().get(t,z));
		,

		,

		)

} // namespace IceBRG
