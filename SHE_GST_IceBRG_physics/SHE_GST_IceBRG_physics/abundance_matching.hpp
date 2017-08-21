/**********************************************************************\
 @file abundance_matching.hpp
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

#ifndef ICEBRG_PHYSICS_ABUNDANCE_MATCHING_HPP_
#define ICEBRG_PHYSICS_ABUNDANCE_MATCHING_HPP_

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/units/units.hpp"
#include "SHE_GST_IceBRG_main/math/misc_math.hpp"

namespace IceBRG
{

flt_t get_abs_mag_B_from_mass( mass_type const & m, flt_t const & z );
mass_type get_mass_from_abs_mag_B( flt_t const & abs_mag, flt_t const & z );
flt_t get_app_mag_B_from_mass( mass_type const & m, flt_t const & z );
mass_type get_mass_from_app_mag_B( flt_t const & app_mag, flt_t const & z );

} // namespace IceBRG



#endif // ICEBRG_PHYSICS_ABUNDANCE_MATCHING_HPP_
