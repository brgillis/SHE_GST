/**********************************************************************\
 @file Image.hpp
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

#ifndef SHE_SIM_GAL_PARAMS_LEVELS_IMAGE_HPP_
#define SHE_SIM_GAL_PARAMS_LEVELS_IMAGE_HPP_

#include <vector>

#include "../../../SHE_GST_PhysicalModel/SHE_SIM/common.hpp"
#include "../../../SHE_GST_PhysicalModel/SHE_SIM/default_values.hpp"
#include "../../../SHE_GST_PhysicalModel/SHE_SIM/level_names.hpp"
#include "../../../SHE_GST_PhysicalModel/SHE_SIM/ParamHierarchyLevel.hpp"

namespace SHE_SIM
{

// Forward-declare children
class ClusterGroup;
class Cluster;
class FieldGroup;
class Field;
class GalaxyGroup;
class Galaxy;

/**
 * TODO Auto-generated comment stub
 */
class Image: public ParamHierarchyLevel
{

public:
	Image(ParamHierarchyLevel * const & parent = nullptr);
	virtual ~Image() {}

	/**
	 * Get the hierarchy level for this class.
	 * @return The hierachy level. 0 = highest, 1 = just below 0, etc.
	 */
	virtual int_t get_hierarchy_level() const override {return dv::image_level;}

	virtual name_t get_name() const override {return image_name;}

	// Methods to add children
#if(1)

	ClusterGroup * add_cluster_group();

	void add_cluster_groups(int_t const & N);

	Cluster * add_cluster();

	void add_clusters(int_t const & N);

	FieldGroup * add_field_group();

	void add_field_groups(int_t const & N);

	Field * add_field();

	void add_fields(int_t const & N);

	GalaxyGroup * add_galaxy_group();

	void add_galaxy_groups(int_t const & N);

    GalaxyPair * add_galaxy_pair();

    void add_galaxy_pairs(int_t const & N);

	Galaxy * add_galaxy();

	void add_galaxies(int_t const & N);

	Galaxy * add_background_galaxy();

	void add_background_galaxies(int_t const & N);

	Galaxy * add_foreground_galaxy();

	void add_foreground_galaxies(int_t const & N);

#endif

	// Methods to automatically add children
#if(1)

	virtual void fill_children() override;

	void fill_clusters();
	void autofill_clusters();

	void fill_field();
	void autofill_field();

#endif

	virtual ParamHierarchyLevel * clone() const override { return new Image(*this); }

};

} // namespace SHE_SIM

#endif // SHE_SIM_GAL_PARAMS_LEVELS_IMAGE_HPP_
