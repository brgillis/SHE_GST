/**********************************************************************\
 @file ipow.hpp
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

#ifndef BRG_MATH_IPOW_HPP_
#define BRG_MATH_IPOW_HPP_

#include <type_traits>

#include "SHE_GST_IceBRG_main/container/is_stl_container.hpp"

namespace IceBRG {

// Integer power - use only when you know it'll be an integer, but not the specific value,
// and when it isn't likely to be too high

#if (1)
// This version is optimized so that it won't check for the p==0 and p<0 cases, and generally
// shouldn't be called directly
template< typename T,
typename std::enable_if<!IceBRG::is_stl_container<T>::value,T>::type* = nullptr >
T _runtime_ipow( T v, int_t p )
{
	if(p==1) return v;
	T tmp = _runtime_ipow(v,p/2);
	if(p%2==0) return tmp*tmp;
	return v*tmp*tmp;
}
#endif

template< typename T,
typename std::enable_if<!IceBRG::is_stl_container<T>::value,T>::type* = nullptr >
T runtime_ipow( T v, int_t p )
{
	if(p<0) return 1/_runtime_ipow(v,-p);
	if(p==0) return 1;
	if(p==1) return v;
	T tmp = _runtime_ipow(v,p/2);
	if(p%2==0) return tmp*tmp;
	return v*tmp*tmp;
}

template< int_t p >
struct _ipow_s {
	const flt_t & value;

	_ipow_s( const flt_t & v )
	: value(v*_ipow_s<p-1>(v).value) // Using a lambda for a multi-line initializer
	{
	}
};

template<>
struct _ipow_s<1> {
	const flt_t & value;

	_ipow_s( const flt_t & v )
	: value(v)
	{
	}
};

template<>
struct _ipow_s<0> {
	const flt_t value;

	_ipow_s( const flt_t & )
	: value(1.)
	{
	}
};

template< int_t p >
flt_t ipow( const flt_t & v)
{
	if(p>0)	return _ipow_s<abs(p)>(v).value;
	if(p<0) return 1./_ipow_s<abs(p)>(v).value;
	return 1.;
}

} // namespace IceBRG



#endif // BRG_MATH_IPOW_HPP_
