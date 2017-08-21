/**********************************************************************\
 @file table_typedefs.hpp
 ------------------

 This header file contains typedefs used for handling data tables and
 their headers.

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


#ifndef _BRG_TABLE_TYPEDEFS_HPP_INCLUDED_
#define _BRG_TABLE_TYPEDEFS_HPP_INCLUDED_

#include <map>
#include <string>
#include <vector>

#include "SHE_GST_IceBRG_main/container/insertion_ordered_map.hpp"

namespace IceBRG
{

// Typedefs
#if(1)

typedef std::vector< std::string > header_t;

template <typename T>
using table_t = std::vector< std::vector< T > >;

template <typename T, typename key_type=std::string>
using table_map_t = IceBRG::insertion_ordered_map<key_type,std::vector<T>>;

#endif

}

#endif // _BRG_TABLE_TYPEDEFS_HPP_INCLUDED_
