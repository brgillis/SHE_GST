/**********************************************************************\
 @file Image.cpp
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
#include "SHE_GST_PhysicalModel/levels/Cluster.hpp"
#include "SHE_GST_PhysicalModel/levels/ClusterGroup.hpp"
#include "SHE_GST_PhysicalModel/levels/Field.hpp"
#include "SHE_GST_PhysicalModel/levels/FieldGroup.hpp"
#include "SHE_GST_PhysicalModel/levels/Galaxy.hpp"
#include "SHE_GST_PhysicalModel/levels/GalaxyGroup.hpp"
#include "SHE_GST_PhysicalModel/levels/GalaxyPair.hpp"
#include "SHE_GST_PhysicalModel/levels/Image.hpp"

namespace SHE_GST_PhysicalModel
{

using namespace IceBRG;

Image::Image(ParamHierarchyLevel * const & p_parent)
: ParamHierarchyLevel(p_parent)
{
}

// Methods to add children
#if(1)

ClusterGroup * Image::add_cluster_group()
{
	return static_cast<ClusterGroup *>(ParamHierarchyLevel::spawn_child<ClusterGroup>());
}

void Image::add_cluster_groups(int_t const & N)
{
	return ParamHierarchyLevel::spawn_children<ClusterGroup>(N);
}

Cluster * Image::add_cluster()
{
	return static_cast<Cluster *>(ParamHierarchyLevel::spawn_child<Cluster>());
}

void Image::add_clusters(int_t const & N)
{
	return ParamHierarchyLevel::spawn_children<Cluster>(N);
}

FieldGroup * Image::add_field_group()
{
	return static_cast<FieldGroup *>(ParamHierarchyLevel::spawn_child<FieldGroup>());
}

void Image::add_field_groups(int_t const & N)
{
	return ParamHierarchyLevel::spawn_children<FieldGroup>(N);
}

Field * Image::add_field()
{
	return static_cast<Field *>(ParamHierarchyLevel::spawn_child<Field>());
}

void Image::add_fields(int_t const & N)
{
	return ParamHierarchyLevel::spawn_children<Field>(N);
}

GalaxyGroup * Image::add_galaxy_group()
{
	return static_cast<GalaxyGroup *>(ParamHierarchyLevel::spawn_child<GalaxyGroup>());
}

void Image::add_galaxy_groups(int_t const & N)
{
	return ParamHierarchyLevel::spawn_children<GalaxyGroup>(N);
}

GalaxyPair * Image::add_galaxy_pair()
{
    return static_cast<GalaxyPair *>(ParamHierarchyLevel::spawn_child<GalaxyPair>());
}

void Image::add_galaxy_pairs(int_t const & N)
{
    return ParamHierarchyLevel::spawn_children<GalaxyPair>(N);
}

Galaxy * Image::add_galaxy()
{
	return static_cast<Galaxy *>(ParamHierarchyLevel::spawn_child<Galaxy>());
}

void Image::add_galaxies(int_t const & N)
{
	return ParamHierarchyLevel::spawn_children<Galaxy>(N);
}

Galaxy * Image::add_background_galaxy()
{
	Galaxy * gal = static_cast<Galaxy *>(ParamHierarchyLevel::spawn_child<Galaxy>());
	gal->set_as_background_galaxy();

	return gal;
}

void Image::add_background_galaxies(int_t const & N)
{
	for(int i=0; i<N; ++i) add_background_galaxy();
}

Galaxy * Image::add_foreground_galaxy()
{
	Galaxy * gal = static_cast<Galaxy *>(ParamHierarchyLevel::spawn_child<Galaxy>());
	gal->set_as_foreground_galaxy();

	return gal;
}

void Image::add_foreground_galaxies(int_t const & N)
{
	for(int i=0; i<N; ++i) add_foreground_galaxy();
}

#endif

// Methods to automatically add children
#if(1)

void Image::fill_children()
{
	fill_clusters();
	fill_field();
}

void Image::fill_clusters()
{
	int_t N = round_int(get_param_value(num_clusters_name));
	add_clusters(N);
}
void Image::autofill_clusters()
{
	int_t N = round_int(get_param_value(num_clusters_name));
	for( int_t i=0; i<N; ++i )
	{
		auto p_new = add_cluster();
		p_new->autofill_children();
	}
}

void Image::fill_field()
{
	int_t N = round_int(get_param_value(num_fields_name));
	add_fields(N);
}
void Image::autofill_field()
{
	int_t N = round_int(get_param_value(num_fields_name));
	for( int_t i=0; i<N; ++i )
	{
		auto p_new = add_field();
		p_new->autofill_children();
	}
}

#endif

} // namespace SHE_GST_PhysicalModel
