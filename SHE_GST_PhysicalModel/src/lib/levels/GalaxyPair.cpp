/**********************************************************************\
 @file GalaxyPair.cpp
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

#include <vector>

#include "SHE_GST_PhysicalModel/common.hpp"
#include "SHE_GST_PhysicalModel/params_list.hpp"
#include "SHE_GST_PhysicalModel/levels/Galaxy.hpp"
#include "SHE_GST_PhysicalModel/levels/GalaxyPair.hpp"

namespace SHE_GST_PhysicalModel
{

GalaxyPair::GalaxyPair(ParamHierarchyLevel * const & p_parent)
: ParamHierarchyLevel(p_parent)
{
}

// Methods to add children
#if(1)

Galaxy * GalaxyPair::add_galaxy()
{
	return static_cast<Galaxy *>(ParamHierarchyLevel::spawn_child<Galaxy>());
}

void GalaxyPair::add_galaxies(int_t const & N)
{
	return ParamHierarchyLevel::spawn_children<Galaxy>(N);
}

#endif

} // namespace SHE_GST_PhysicalModel
