/**********************************************************************\
 @file level_names.hpp
 ------------------

 Names of the PHLs.

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

#ifndef SHE_SIM_GAL_PARAMS_LEVEL_NAMES_HPP_
#define SHE_SIM_GAL_PARAMS_LEVEL_NAMES_HPP_

#define DEF_NAME(level) constexpr const char * level##_name = #level;

DEF_NAME(survey)
DEF_NAME(image_group)
DEF_NAME(image)
DEF_NAME(cluster_group)
DEF_NAME(cluster)
DEF_NAME(field_group)
DEF_NAME(field)
DEF_NAME(galaxy_group)
DEF_NAME(galaxy_pair)
DEF_NAME(galaxy)

#undef DEF_NAME

#endif // SHE_SIM_GAL_PARAMS_LEVEL_NAMES_HPP_
