/**********************************************************************\
 @file ClusterGroup.hpp
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

#ifndef SHE_SIM_GAL_PARAMS_LEVELS_CLUSTERGROUP_HPP_
#define SHE_SIM_GAL_PARAMS_LEVELS_CLUSTERGROUP_HPP_

#include <utility>

#include "SHE_GST_PhysicalModel/common.hpp"
#include "SHE_GST_PhysicalModel/default_values.hpp"
#include "SHE_GST_PhysicalModel/level_names.hpp"
#include "SHE_GST_PhysicalModel/ParamHierarchyLevel.hpp"

namespace SHE_GST_PhysicalModel
{

// Forward-declare children
class Cluster;

/**
 * TODO Auto-generated comment stub
 */
class ClusterGroup: public ParamHierarchyLevel
{

public:
	ClusterGroup(ParamHierarchyLevel * const & parent = nullptr);
	virtual ~ClusterGroup() {}

	/**
	 * Get the hierarchy level for this class.
	 * @return The hierachy level. 0 = highest, 1 = just below 0, etc.
	 */
	virtual int_t get_hierarchy_level() const override {return dv::cluster_group_level;}

	virtual name_t get_name() const override {return cluster_group_name;}

	// Methods to add children
#if(1)

	Cluster * add_cluster();

	void add_clusters(int_t const & N);

#endif

	virtual ParamHierarchyLevel * clone() const override { return new ClusterGroup(*this); }

};

} // namespace SHE_GST_PhysicalModel

#endif // SHE_SIM_GAL_PARAMS_LEVELS_CLUSTERGROUP_HPP_
