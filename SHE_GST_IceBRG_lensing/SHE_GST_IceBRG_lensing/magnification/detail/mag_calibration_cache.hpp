/**********************************************************************\
 @file mag_calibration_cache.h
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

// body file: mag_calibration_cache.cpp

#ifndef _BRG_BRG_LENSING_MAGNIFICATION_MAG_CALIBRATION_CACHE_H_INCLUDED_
#define _BRG_BRG_LENSING_MAGNIFICATION_MAG_CALIBRATION_CACHE_H_INCLUDED_

#include "SHE_GST_IceBRG_main/units/units.hpp"
#include <string>

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/math/cache/cache.hpp"


namespace IceBRG {

DECLARE_BRG_CACHE(mag_calibration_cache,magcalib,flt_t,flt_t)

} // end namespace IceBRG



#endif // _BRG_BRG_LENSING_MAGNIFICATION_MAG_CALIBRATION_CACHE_H_INCLUDED_
