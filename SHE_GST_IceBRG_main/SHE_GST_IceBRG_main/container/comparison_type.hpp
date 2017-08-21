/**********************************************************************\
 @file comparison_type.hpp
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

// body file: comparison_type.cpp

#ifndef _BRG_BRG_CONTAINER_COMPARISON_TYPE_HPP_INCLUDED_
#define _BRG_BRG_CONTAINER_COMPARISON_TYPE_HPP_INCLUDED_

#include <type_traits>

// Applies both std::decay and std::remove_reference at once, for convenience

namespace IceBRG {

template <typename T>
struct ct
{
	typedef typename std::remove_cv<typename std::decay<T>::type>::type type;
};

}



#endif // _BRG_BRG_CONTAINER_COMPARISON_TYPE_HPP_INCLUDED_
