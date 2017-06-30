/**********************************************************************\
 @file ImageGroup.cpp
 ------------------

 TODO <Insert file description here>

 **********************************************************************

 Copyright (C) 2015 brg

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <utility>

#include "SHE_GST_PhysicalModel/common.hpp"
#include "SHE_GST_PhysicalModel/params_list.hpp"
#include "SHE_GST_PhysicalModel/levels/Image.hpp"
#include "SHE_GST_PhysicalModel/levels/ImageGroup.hpp"

namespace SHE_SIM
{

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

} // namespace SHE_SIM
