/**********************************************************************\
 @file Survey.cpp
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

#include "IceBRG_main/math/misc_math.hpp"

#include "SHE_SIM/common.hpp"
#include "SHE_SIM/params_list.hpp"
#include "SHE_SIM/param_declarations.hpp"
#include "SHE_SIM/levels/Image.hpp"
#include "SHE_SIM/levels/ImageGroup.hpp"
#include "SHE_SIM/levels/Survey.hpp"

namespace SHE_SIM
{

using namespace IceBRG;

Survey::Survey()
: ParamHierarchyLevel(nullptr)
{
}

// Methods to add children
#if(1)

ImageGroup * Survey::add_image_group()
{
	return static_cast<ImageGroup *>(ParamHierarchyLevel::spawn_child<ImageGroup>());
}

void Survey::add_image_groups(int_t const & N)
{
	return ParamHierarchyLevel::spawn_children<ImageGroup>(N);
}

Image * Survey::add_image()
{
	return static_cast<Image *>(ParamHierarchyLevel::spawn_child<Image>());
}

void Survey::add_images(int_t const & N)
{
	return ParamHierarchyLevel::spawn_children<Image>(N);
}

#endif

// Methods to automatically add children
#if(1)

void Survey::fill_images()
{
	 add_images( round_int(get_param_value(num_images_name)) );
}

#endif

} // namespace SHE_SIM
