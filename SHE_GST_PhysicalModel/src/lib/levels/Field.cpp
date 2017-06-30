/**********************************************************************\
 @file Field.cpp
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

#include "SHE_GST_IceBRG_main/math/misc_math.hpp"

#include "SHE_GST_PhysicalModel/common.hpp"
#include "SHE_GST_PhysicalModel/params_list.hpp"
#include "SHE_GST_PhysicalModel/param_declarations.hpp"
#include "SHE_GST_PhysicalModel/dependency_functions/galaxy_type.hpp"
#include "SHE_GST_PhysicalModel/levels/Field.hpp"
#include "SHE_GST_PhysicalModel/levels/Galaxy.hpp"
#include "SHE_GST_PhysicalModel/levels/GalaxyGroup.hpp"
#include "SHE_GST_PhysicalModel/levels/GalaxyPair.hpp"

namespace SHE_SIM
{

using namespace IceBRG;

Field::Field(ParamHierarchyLevel * const & p_parent)
: ParamHierarchyLevel(p_parent)
{
}

// Methods to add children
#if(1)

GalaxyGroup * Field::add_galaxy_group()
{
	return static_cast<GalaxyGroup *>(ParamHierarchyLevel::spawn_child<GalaxyGroup>());
}

void Field::add_galaxy_groups(int_t const & N)
{
	return ParamHierarchyLevel::spawn_children<GalaxyGroup>(N);
}

GalaxyPair * Field::add_galaxy_pair()
{
    return static_cast<GalaxyPair *>(ParamHierarchyLevel::spawn_child<GalaxyPair>());
}

void Field::add_galaxy_pairs(int_t const & N)
{
    return ParamHierarchyLevel::spawn_children<GalaxyPair>(N);
}

Galaxy * Field::add_galaxy()
{
	Galaxy * gal = static_cast<Galaxy *>(ParamHierarchyLevel::spawn_child<Galaxy>());
	gal->set_param_params(galaxy_type_name,"fixed",field_galaxy_type);

	return gal;
}

void Field::add_galaxies(int_t const & N)
{
	for(int i=0; i<N; ++i) add_galaxy();
}

#endif

// Methods to automatically add children
#if(1)

void Field::fill_children()
{
	fill_galaxies();
}

void Field::fill_galaxies()
{
	add_galaxies( round_int( get_param_value( num_field_galaxies_name ) ) );
}

#endif

} // namespace SHE_SIM
