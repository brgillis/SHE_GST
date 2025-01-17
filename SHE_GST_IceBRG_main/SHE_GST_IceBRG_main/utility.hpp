/**********************************************************************\
  @file utility.hpp

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

#ifndef _BRG_UTILITY_HPP_INCLUDED_
#define _BRG_UTILITY_HPP_INCLUDED_

#include <cassert>
#include <complex>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include <boost/tuple/tuple.hpp>

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/container/is_container.hpp"

namespace IceBRG
{

// Functions
#if(1)

// Function to get the size of a container casted to a signed integer
#if(1)

template<typename T, BRG_F_IS_CONTAINER(T)>
int_t ssize( const T & container)
{
	return container.size(); // The return type really does all the work here
}

template<typename T, BRG_F_IS_TUPLE(T)>
constexpr int_t ssize( const T & tuple)
{
	return boost::tuples::length<T>::value;
}

template<typename T, BRG_F_NOT_CONTAINER(T)>
int_t ssize( const T & container)
{
	return container.size(); // TODO: Fix so it'll give a result of 1 for those without a size() method
}

#endif // Function to get the size of a container casted to a signed integer

// Generic functions
#if (1)

// Set_zero function - a way for other template functions to "clear" or initialize a value in various ways
inline void set_zero( int_t & obj )
{
	obj = 0;
}
inline void set_zero( short_int_t & obj )
{
	obj = 0;
}
inline void set_zero( long_int_t & obj )
{
	obj = 0;
}
inline void set_zero( flt_t & obj )
{
	obj = 0;
}
inline void set_zero( long_flt_t & obj )
{
	obj = 0;
}
template< typename T >
inline void set_zero( std::vector< T > & vec )
{
	vec.clear();
}
template< typename T >
inline void set_zero( T * &obj )
{
	obj = NULL;
}
template< typename obj_type >
inline void set_zero( obj_type & obj )
{
	obj = obj_type();
}

// Various "make" functions, to allocate dynamic memory.
// After allocating memory, these functions initialize the new variables using the
// set_zero function (see above).

template< typename obj_type >
inline void make_obj( BRG_UNIQUE_PTR<obj_type> & obj_pointer )
{
	obj_pointer = BRG_UNIQUE_PTR<obj_type>(new obj_type);
	set_zero(*obj_pointer);
}

#endif // Generic functions

#endif // Functions


} // end namespace IceBRG

#endif // _BRG_UTILITY_HPP_INCLUDED_
