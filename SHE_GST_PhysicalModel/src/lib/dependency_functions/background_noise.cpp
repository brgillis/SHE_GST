/**********************************************************************\
 @file background_noise.cpp
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

#include <cmath>

#include "SHE_GST_IceBRG_main/math/misc_math.hpp"

#include "SHE_GST_PhysicalModel/common.hpp"

namespace SHE_GST_PhysicalModel {

flt_t get_background_noise( flt_t const & subtracted_background, flt_t const & unsubtracted_background,
		flt_t const & read_noise, flt_t const & gain, flt_t const & pixel_scale )
{
	flt_t background_ADU_per_arcsec = subtracted_background + unsubtracted_background;
  flt_t background_e_per_pixel = background_ADU_per_arcsec * IceBRG::square(pixel_scale * 3600) * gain;

	flt_t background_noise_e = std::sqrt( background_e_per_pixel + read_noise );
	flt_t background_noise_ADU = background_noise_e / gain;

	return background_noise_ADU;
}


} // namespace SHE_GST_PhysicalModel


