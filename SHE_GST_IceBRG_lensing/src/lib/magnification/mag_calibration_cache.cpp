/**********************************************************************\
 @file mag_calibration_cache.cpp
 ------------------

 TODO <Insert file description here>

 **********************************************************************

 Copyright (C) 2015  Bryan R. Gillis

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

#include "SHE_GST_IceBRG_lensing/magnification/detail/mag_calibration_cache.hpp"

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_lensing/magnification/detail/mag_calibration_loader.hpp"
#include "SHE_GST_IceBRG_lensing/magnification/mag_global_values.hpp"


namespace IceBRG {

// Initialise the cache
DEFINE_BRG_CACHE( mag_calibration_cache,IceBRG::flt_t,IceBRG::flt_t,
				  IceBRG::mag_z_min,IceBRG::mag_z_max,IceBRG::mag_z_step
				  ,
				  	  return mag_calibration_loader::get(in_param);
				  ,

				  ,
)

} // namespace IceBRG


