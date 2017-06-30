/**********************************************************************\
 @file magnification_functors.cpp
 ------------------

 Source file for the classes defined in magnification_functors.h.

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

#include "SHE_GST_IceBRG_lensing/magnification/detail/magnification_functors.hpp"

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/math/misc_math.hpp"
#include "SHE_GST_IceBRG_main/units/units.hpp"

#include "SHE_GST_IceBRG_lensing/magnification/detail/expected_count_cache.hpp"
#include "SHE_GST_IceBRG_lensing/magnification/magnification_alpha.hpp"

namespace IceBRG {

IceBRG::custom_unit_type<0,0,0,-2,0> mag_expected_count_functor::operator() (const flt_t & m) const
{
	IceBRG::custom_unit_type<0,0,0,-2,0> res = expected_count_cache().get(m,_z_mean_);

	return res;
}

IceBRG::custom_unit_type<0,0,0,-2,0> mu_signal_integration_functor::operator() (const flt_t & m) const
{
	flt_t alpha = magnification_alpha(m,_z_mean_);
	IceBRG::custom_unit_type<0,0,0,-2,0> count = expected_count_cache().get(m,_z_mean_);
	return count*(alpha-1);
}

IceBRG::custom_unit_type<0,0,0,-2,0> mu_weight_integration_functor::operator() (const flt_t & m) const
{
	flt_t alpha = magnification_alpha(m,_z_mean_);
	IceBRG::custom_unit_type<0,0,0,-2,0> count = expected_count_cache().get(m,_z_mean_);
	return count*square(alpha-1);
}

} // namespace IceBRG

