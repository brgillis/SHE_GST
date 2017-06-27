/**********************************************************************\
 @file GalaxyGroup.cpp
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

#include <vector>

#include "SHE_SIM/common.hpp"
#include "SHE_SIM/params_list.hpp"
#include "SHE_SIM/levels/Galaxy.hpp"
#include "SHE_SIM/levels/GalaxyGroup.hpp"
#include "SHE_SIM/levels/GalaxyPair.hpp"

namespace SHE_SIM
{

GalaxyGroup::GalaxyGroup(ParamHierarchyLevel * const & p_parent)
: ParamHierarchyLevel(p_parent)
{
}

// Methods to add children
#if(1)

GalaxyPair * GalaxyGroup::add_galaxy_pair()
{
    return static_cast<GalaxyPair *>(ParamHierarchyLevel::spawn_child<GalaxyPair>());
}

void GalaxyGroup::add_galaxy_pairs(int_t const & N)
{
    return ParamHierarchyLevel::spawn_children<GalaxyPair>(N);
}

Galaxy * GalaxyGroup::add_galaxy()
{
	return static_cast<Galaxy *>(ParamHierarchyLevel::spawn_child<Galaxy>());
}

void GalaxyGroup::add_galaxies(int_t const & N)
{
	return ParamHierarchyLevel::spawn_children<Galaxy>(N);
}

#endif

} // namespace SHE_SIM
