/**********************************************************************\
 @file unitconv_map.hpp
 ------------------

 A simple typedef of IceBRG::unitconv_map.

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


// body file: unitconv_map.cpp


#ifndef _BRG_UNITCONV_MAP_HPP_INCLUDED_
#define _BRG_UNITCONV_MAP_HPP_INCLUDED_

#include <map>
#include <string>

#include "SHE_GST_IceBRG_main/common.hpp"

namespace IceBRG
{

typedef std::map<std::string,flt_t> unitconv_map;

} // namespace IceBRG



#endif // _BRG_UNITCONV_MAP_HPP_INCLUDED_
