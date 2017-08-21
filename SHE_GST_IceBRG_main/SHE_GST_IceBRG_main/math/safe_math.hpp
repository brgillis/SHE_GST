/**********************************************************************\
  @file safe_math.hpp

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

#ifndef _BRG_SAFE_MATH_HPP_INCLUDED_
#define _BRG_SAFE_MATH_HPP_INCLUDED_

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <type_traits>

#include <boost/units/get_dimension.hpp>
#include <boost/units/is_quantity.hpp>

#include "SHE_GST_IceBRG_main/container/is_stl_container.hpp"
#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/container/is_eigen_container.hpp"
#include "SHE_GST_IceBRG_main/utility.hpp"

namespace IceBRG {

// "Safe" functions - perform the operation specified, but will
// take necessary actions to ensure it won't crash the program
// if the argument is invalid (eg. taking square-root of a negative
// number).
template< class T >
auto safe_sqrt( const T & a ) -> decltype(sqrt(a))
{

#ifdef _BRG_WARN_FOR_SAFE_FUNCTIONS_TRIGGERED_
	if(a < 0)
	{
		handle_error_message("safe_sqrt() prevented error from negative input.");
	}
#endif
	return sqrt( fabs( a ) );
}
inline flt_t safe_sqrt( const int_t a ) // Special case for integers due to -INT_MIN > INT_MAX
{
	using std::sqrt;

	flt_t res;

#ifdef _BRG_WARN_FOR_SAFE_FUNCTIONS_TRIGGERED_
	if(a < 0)
	{
		handle_error_message("safe_sqrt() prevented error from negative input.");
	}
#endif

	if ( a == std::numeric_limits<int_t>::min() )
	{
		res = std::numeric_limits<int_t>::max();
	}
	else
	{
		res = a < 0 ? -a : a;
	}
	return sqrt( res );
}
template< class Ta >
inline const Ta safe_pow( const Ta & a, const flt_t & x )
{
	Ta res;
	flt_t ipart;

	std::modf( x, &ipart );

#pragma GCC diagnostic ignored "-Wfloat-equal"
	if ( ( a < Ta(0.) ) && ( ipart != x ) )
	{
#ifdef _BRG_WARN_FOR_SAFE_FUNCTIONS_TRIGGERED_
		handle_error_message("WARNING: safe_pow() prevented error from negative input.");
#endif
		res = -a;
	}
	else
	{
		res = a;
	}

	return std::pow( res, x );
}

// Subtract one value from another in quadrature
template < typename T1, typename T2 >
inline const T1 safe_quad_sub( const T1 & v1, const T2 & v2 )
{
	return safe_sqrt( v1 * v1 - v2 * v2 );
}

// Safe_d is used a bit differently: Put it around the denominator to make
// sure you don't hit a divide-by-zero error.
template< class T,
typename std::enable_if<!IceBRG::is_stl_container<T>::value,char>::type = 0,
typename std::enable_if<!IceBRG::is_eigen_container<T>::value,char>::type = 0,
typename std::enable_if<!boost::units::is_quantity<T>::value,char>::type = 0>
const T safe_d( const T & a )
{

#ifdef _BRG_WARN_FOR_SAFE_FUNCTIONS_TRIGGERED_
	if( (a == 0) || isbad(a) )
	{
		handle_error_message("safe_d() prevented error from zero input or bad input.");
	}
#endif
	T min_d(MIN_DIVISOR);

	if ( isnan( a ) )
		return min_d;
	if ( isinf( a ) )
		return 1. / min_d;

	if (fabs(a)>min_d)
		return a;

	if(min_d == 0) return 1; // In case of integers

	return min_d;

}

// Version for units
template< class T,
typename std::enable_if<!IceBRG::is_stl_container<T>::value,char>::type = 0,
typename std::enable_if<!IceBRG::is_eigen_container<T>::value,char>::type = 0,
typename std::enable_if<boost::units::is_quantity<T>::value,char>::type = 0>
const T safe_d( const T & a )
{

#ifdef _BRG_WARN_FOR_SAFE_FUNCTIONS_TRIGGERED_
	if( (a.value() == 0) || isbad(a) )
	{
		handle_error_message("safe_d() prevented error from zero input or bad input.");
	}
#endif
	//boost::units::quantity<boost::units::si::length>::

	T min_d(MIN_DIVISOR * typename T::unit_type() );

	if ( isnan( a.value() ) )
		return min_d;
	if ( isinf( a.value() ) )
		return typename T::unit_type() /MIN_DIVISOR;

	if (fabs(a.value())>min_d.value())
		return a;

	return min_d;

}

// Eigen-like container specialization
template< class T,
typename std::enable_if<!IceBRG::is_stl_container<T>::value,char>::type = 0,
typename std::enable_if<IceBRG::is_eigen_container<T>::value,char>::type = 0>
T safe_d( T array )
{
	auto p = array.data();
	for( decltype(ssize(array)) i = 0; i < ssize(array); ++i)
	{

		#ifdef _BRG_WARN_FOR_SAFE_FUNCTIONS_TRIGGERED_
		if( (*p == 0) || isbad(*p) )
		{
			handle_error_message("safe_d() prevented error from zero input or bad input.");
		}
		#endif

		typename std::decay<decltype(*p)>::type min_d;

		if(std::is_integral<decltype(min_d)>::value)
			min_d = 1;
		else
			min_d = MIN_DIVISOR;

		if ( isnan( *p ) )
		{
			*p = min_d;
			continue;
		}
		if ( isinf( *p ) )
		{
			*p = 1. / min_d;
			continue;
		}

		if (std::fabs(*p)<min_d)
		{
			*p = min_d;
			continue;
		}

		++p;
	}

	return array;
}

} // namespace IceBRG

#endif /* _BRG_SAFE_MATH_HPP_INCLUDED_ */
