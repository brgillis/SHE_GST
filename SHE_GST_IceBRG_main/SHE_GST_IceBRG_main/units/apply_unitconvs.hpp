/**********************************************************************\
 @file apply_unitconvs.hpp
 ------------------

 File containing methods to apply unitconversions to data tables.

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

#ifndef _BRG_APPLY_UNITCONVS_HPP_INCLUDED_
#define _BRG_APPLY_UNITCONVS_HPP_INCLUDED_

#include "SHE_GST_IceBRG_main/units/unitconv_map.hpp"
#include <map>
#include <string>
#include <vector>

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/container/table_typedefs.hpp"
#include "SHE_GST_IceBRG_main/math/misc_math.hpp"
#include "SHE_GST_IceBRG_main/vector/elementwise_functions.hpp"
#include "SHE_GST_IceBRG_main/vector/make_vector.hpp"


namespace IceBRG
{

/**
 * Note: for unit conversions here, use the standard form. That is, if you want units to be
 * output in km, pass in unitconv::kmtom;
 * @param data_map
 * @param u_map
 * @return
 */
inline table_map_t<flt_t> get_table_after_unitconv(table_map_t<flt_t> data_map,
		const unitconv_map & u_map)
{
	for(auto u_it=u_map.begin(); u_it!=u_map.end(); ++u_it)
	{
		auto d_it = data_map.find(u_it->first);
		// Check if this value is actually in the data table
		if(d_it!=data_map.end())
		{
			// It's in the table, so multiply its associated vector by the inverse unit conversion
			flt_t factor = 1./(u_it->second);
			if(isbad(factor)) factor = 1; // To catch user mistakes
			d_it->second = multiply(d_it->second,factor);
		}
	}
	return data_map;
}

/**
 * Note: for unit conversions here, use the standard form. That is, if you want units to be
 * output in km, pass in unitconv::kmtom;
 * @param data_map
 * @param u_map
 * @return
 */
template<typename T>
table_map_t<flt_t> get_table_after_unitconv(const table_map_t<T> & data_map, const unitconv_map & u_map)
{
	table_map_t<flt_t> result_map;

	for(auto it=data_map.begin(); it!=data_map.end(); ++it)
	{
		make_vector_coerce<1>(result_map[it->first],it->second);
	}
	return get_table_after_unitconv(result_map,u_map);
}

} // namespace IceBRG


#endif // _BRG_APPLY_UNITCONVS_HPP_INCLUDED_
