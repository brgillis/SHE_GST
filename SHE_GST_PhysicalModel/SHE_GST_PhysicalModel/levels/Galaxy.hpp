/**********************************************************************\
 @file Galaxy.hpp
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

#ifndef SHE_SIM_GAL_PARAMS_LEVELS_GALAXY_HPP_
#define SHE_SIM_GAL_PARAMS_LEVELS_GALAXY_HPP_

#include "SHE_GST_PhysicalModel/common.hpp"
#include "SHE_GST_PhysicalModel/default_values.hpp"
#include "SHE_GST_PhysicalModel/level_names.hpp"
#include "SHE_GST_PhysicalModel/ParamHierarchyLevel.hpp"

namespace SHE_GST_PhysicalModel
{

/**
 * TODO Auto-generated comment stub
 */
class Galaxy: public ParamHierarchyLevel
{
private:

	bool _is_background;

public:
	Galaxy(ParamHierarchyLevel * const & parent = nullptr);
	virtual ~Galaxy() {}

	/**
	 * Get the hierarchy level for this class.
	 * @return The hierachy level. 0 = highest, 1 = just below 0, etc.
	 */
	virtual int_t get_hierarchy_level() const override {return dv::galaxy_level;}

	virtual name_t get_name() const override {return galaxy_name;}

	/**
	 * Set up so that parameters will be generated in background-galaxy mode.
	 */
	void set_as_background_galaxy();

	bool is_background_galaxy() { return _is_background; }

	/**
	 * Set up so that parameters will be generated in foreground-galaxy mode.
	 */
	void set_as_foreground_galaxy();

	bool is_foreground_galaxy() { return !_is_background; }

	bool is_central_galaxy();
	bool is_field_galaxy();
	bool is_satellite_galaxy();

	virtual ParamHierarchyLevel * clone() const override { return new Galaxy(*this); }

};

} // namespace SHE_GST_PhysicalModel

#endif // SHE_SIM_GAL_PARAMS_LEVELS_GALAXY_HPP_
