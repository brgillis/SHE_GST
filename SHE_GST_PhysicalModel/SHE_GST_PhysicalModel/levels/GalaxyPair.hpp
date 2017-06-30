/**********************************************************************\
 @file GalaxyPair.hpp
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

#ifndef SHE_SIM_GAL_PARAMS_LEVELS_GALAXYPAIR_HPP_
#define SHE_SIM_GAL_PARAMS_LEVELS_GALAXYPAIR_HPP_

#include <vector>

#include "SHE_GST_PhysicalModel/common.hpp"
#include "SHE_GST_PhysicalModel/default_values.hpp"
#include "SHE_GST_PhysicalModel/ParamHierarchyLevel.hpp"

namespace SHE_GST_PhysicalModel
{

// Forward-declare children
class Galaxy;

/**
 * TODO Auto-generated comment stub
 */
class GalaxyPair: public ParamHierarchyLevel
{

public:
	GalaxyPair(ParamHierarchyLevel * const & parent = nullptr);
	virtual ~GalaxyPair() {}

	/**
	 * Get the hierarchy level for this class.
	 * @return The hierachy level. 0 = highest, 1 = just below 0, etc.
	 */
	virtual int_t get_hierarchy_level() const override {return dv::galaxy_pair_level;}

	virtual name_t get_name() const override {return galaxy_pair_name;}

	// Methods to add children
#if(1)

	Galaxy * add_galaxy();

	void add_galaxies(int_t const & N);

#endif

	virtual ParamHierarchyLevel * clone() const override { return new GalaxyPair(*this); }

};

} // namespace SHE_GST_PhysicalModel

#endif // SHE_SIM_GAL_PARAMS_LEVELS_GALAXYPAIR_HPP_
