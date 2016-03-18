/**********************************************************************\
 @file mag_weight_integral_cache.h
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

// body file: mag_weight_integral_cache.cpp

#ifndef _BRG_MAG_WEIGHT_INTEGRAL_CACHE_H_INCLUDED_
#define _BRG_MAG_WEIGHT_INTEGRAL_CACHE_H_INCLUDED_

#include "IceBRG_main/units/units.hpp"
#include <string>

#include "IceBRG_main/common.hpp"

#include "IceBRG_main/math/cache/cache.hpp"


namespace IceBRG {

DECLARE_BRG_CACHE(mag_weight_integral_cache,mg_w_int,
		flt_t,decltype(custom_unit_type<0,0,0,-2,0>()));

} // end namespace IceBRG

#endif // _BRG_MAG_WEIGHT_INTEGRAL_CACHE_H_INCLUDED_
