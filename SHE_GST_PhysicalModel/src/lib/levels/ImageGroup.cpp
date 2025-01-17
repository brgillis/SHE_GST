/**********************************************************************\
 @file ImageGroup.cpp
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

#include <utility>

#include "SHE_GST_IceBRG_main/math/misc_math.hpp"

#include "SHE_GST_PhysicalModel/common.hpp"
#include "SHE_GST_PhysicalModel/params_list.hpp"
#include "SHE_GST_PhysicalModel/param_declarations.hpp"
#include "SHE_GST_PhysicalModel/levels/Image.hpp"
#include "SHE_GST_PhysicalModel/levels/ImageGroup.hpp"

namespace SHE_GST_PhysicalModel
{

using namespace IceBRG;

ImageGroup::ImageGroup(ParamHierarchyLevel * const & p_parent)
: ParamHierarchyLevel(p_parent)
{
}

// Methods to add children
#if(1)

Image * ImageGroup::add_image()
{
	return static_cast<Image *>(ParamHierarchyLevel::spawn_child<Image>());
}

void ImageGroup::add_images(int_t const & N)
{
	return ParamHierarchyLevel::spawn_children<Image>(N);
}

#endif

// Methods to automatically add children
#if(1)

void ImageGroup::fill_images()
{
     add_images( round_int(get_param_value(num_images_name)) );
}

#endif

} // namespace SHE_GST_PhysicalModel
