/**********************************************************************\
 @file units.hpp
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

#ifndef BRG_UNITS_UNITS_HPP_
#define BRG_UNITS_UNITS_HPP_

#include <cmath>

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/container/is_boost_tuple.hpp"
#include "SHE_GST_IceBRG_main/math/ipow.hpp"

#include "SHE_GST_IceBRG_main/units/unit_conversions.hpp"

// Enable dimensional analysis unless the NDEBUG flag is defined
#if not (defined(NDEBUG) or defined(ELEMENTS_LINKER_LIBRARY) or defined(_FORTIFY_SOURCE) or defined(_GNU_SOURCE))
#define _BRG_USE_UNITS_
#endif // #ifndef NDEBUG

#ifdef _BRG_USE_UNITS_

#include <boost/units/cmath.hpp>
#include <boost/units/derived_dimension.hpp>
#include <boost/units/is_quantity.hpp>
#include <boost/units/systems/si.hpp>
#include <boost/any.hpp>
#include <boost/serialization/split_free.hpp>

#define UNIT_CONSTEXPR const // Will instead be constexpr if not using units

namespace IceBRG {

// Quantity types
#if(1)

typedef boost::any any_units_type;

typedef boost::units::quantity<boost::units::si::dimensionless> dimensionless_type;

typedef boost::units::quantity<boost::units::si::length> distance_type;
typedef boost::units::quantity<boost::units::si::area> area_type;
typedef boost::units::quantity<boost::units::si::volume> volume_type;
typedef boost::units::quantity<boost::units::si::wavenumber> inverse_distance_type;
typedef decltype(1./area_type()) inverse_area_type;
typedef decltype(1./volume_type()) inverse_volume_type;

typedef boost::units::quantity<boost::units::si::time> time_type;
typedef boost::units::quantity<boost::units::si::frequency> inverse_time_type;

typedef boost::units::quantity<boost::units::si::mass> mass_type;

typedef boost::units::quantity<boost::units::si::plane_angle> angle_type;
typedef decltype(angle_type()*angle_type()) square_angle_type;
typedef decltype(1./angle_type()) inverse_angle_type;
typedef decltype(1./square_angle_type()) inverse_square_angle_type;

typedef boost::units::quantity<boost::units::si::temperature> temperature_type;

typedef boost::units::quantity<boost::units::si::velocity> velocity_type;
typedef boost::units::quantity<boost::units::si::acceleration> acceleration_type;

typedef boost::units::quantity<boost::units::si::mass_density> density_type;
typedef decltype(1./density_type()) inverse_density_type;
typedef boost::units::quantity<boost::units::si::surface_density> surface_density_type;
typedef decltype(1./surface_density_type()) inverse_surface_density_type;

typedef decltype(distance_type()/angle_type()) distance_over_angle_type;
typedef decltype(1./volume_type()/mass_type()) inverse_volume_inverse_mass_type;

// Gotta be a bit hacky here to handle zeros - When defining a dimension, passing 0 as the exponent
// isn't treated the same as not passing it at all. We can get around that with decltype of a division
// by a power zero of that unit
template< short int d_p,
			short int t_p=0,
			short int m_p=0,
			short int a_p=0,
			short int T_p=0>
using custom_unit_type =
  decltype(boost::units::pow<(d_p)>(distance_type())/boost::units::pow<(0)>(distance_type()) *
		   boost::units::pow<(t_p)>(time_type())/boost::units::pow<(0)>(time_type()) *
		   boost::units::pow<(m_p)>(mass_type())/boost::units::pow<(0)>(mass_type()) *
		   boost::units::pow<(a_p)>(angle_type())/boost::units::pow<(0)>(angle_type()) *
		   boost::units::pow<(T_p)>(temperature_type())/boost::units::pow<(0)>(temperature_type()));

#endif // Quantity types

// Base unit values
#if(1)

const auto meter = boost::units::si::meter;
const auto second = boost::units::si::second;
const auto kilogram = boost::units::si::kilogram;
const auto radian = boost::units::si::radian;
const auto meter_per_second(boost::units::si::meter/boost::units::si::second);

const auto m = meter;
const auto s = second;
const auto kg = kilogram;
const auto rad = radian;
const auto mps = meter_per_second;

#endif // Base unit values

// Utility functions
#if(1)

using boost::any_cast;

template<typename T,
	typename std::enable_if<!std::is_same<T,any_units_type>::value &&
		boost::units::is_quantity<T>::value,char>::type = 0>
flt_t value_of( const T & x)
{
	return static_cast<flt_t>(x.value());
}
template<typename T,
typename std::enable_if<!std::is_same<T,any_units_type>::value &&
!boost::units::is_quantity<T>::value,char>::type = 0>
flt_t value_of( const T & x)
{
	return static_cast<flt_t>(x);
}
template<typename Tstored, typename T,
	typename std::enable_if<std::is_same<T,any_units_type>::value,char>::type = 0>
flt_t value_of( const T & x)
{
	return value_of(any_cast<Tstored>(x));
}

template<typename Tout, typename Tin, typename Tout_copy=Tout,
		typename std::enable_if<boost::units::is_quantity<Tout_copy>::value,char>::type = 0>
Tout units_cast( const Tin & x)
{
	return static_cast<Tout>(value_of(x) * typename Tout::unit_type());
}

template<typename Tout, typename Tin, typename Tout_copy=Tout,
		typename std::enable_if<!boost::units::is_quantity<Tout_copy>::value,char>::type = 0>
Tout units_cast( const Tin & x)
{
	return static_cast<Tout>(value_of(x));
}

template<typename Tout, typename Tin>
any_units_type any_units_cast( const Tin & x)
{
	return units_cast<Tout>(x);
}

#endif // Utility functions

// Math functions
#if(1)

template<typename T,
typename std::enable_if<boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
T abs( const T & x)
{
	return boost::units::abs(x);
}
template<typename T,
typename std::enable_if<!boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
T abs( const T & x)
{
	return std::fabs(x);
}

template<typename T,
typename std::enable_if<boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
auto sqrt( const T & x) -> decltype(boost::units::sqrt(x))
{
	return boost::units::sqrt(x);
}
template<typename T,
typename std::enable_if<!boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
auto sqrt( const T & x) -> decltype(std::sqrt(x))
{
	return std::sqrt(x);
}

template< int_t e, typename T,
typename std::enable_if<boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
auto ipow(const T & x) -> decltype(boost::units::pow<e>(x))
{
	return boost::units::pow<e>(x);
}

template< int_t en, int_t ed, typename T,
typename std::enable_if<boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
auto ipow( const T & x) -> decltype(boost::units::pow<boost::units::static_rational<en,ed>>(x))
{
	return boost::units::pow<boost::units::static_rational<en,ed>>(x);
}
template< int_t en, int_t ed, typename T,
typename std::enable_if<!boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
auto ipow( const T & x) -> decltype(boost::units::pow<boost::units::static_rational<en,ed>>(x))
{
	return std::pow(x,static_cast<flt_t>(en)/ed);
}

template<typename T,
typename std::enable_if<boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
flt_t sin( const T & x)
{
	return boost::units::sin(x);
}
template<typename T,
typename std::enable_if<!boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
flt_t sin( const T & x)
{
	return std::sin(x);
}

template<typename T,
typename std::enable_if<boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
flt_t cos( const T & x)
{
	return boost::units::cos(x);
}
template<typename T,
typename std::enable_if<!boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
flt_t cos( const T & x)
{
	return std::cos(x);
}

template<typename T,
typename std::enable_if<boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
flt_t tan( const T & x)
{
	return boost::units::tan(x);
}
template<typename T,
typename std::enable_if<!boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
flt_t tan( const T & x)
{
	return std::tan(x);
}

template<typename T,
typename std::enable_if<boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
angle_type asin( const T & x)
{
	return boost::units::asin(x);
}
template<typename T,
typename std::enable_if<!boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
T asin( const T & x)
{
	return std::asin(x);
}

template<typename T,
typename std::enable_if<boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
angle_type acos( const T & x)
{
	return boost::units::acos(x);
}
template<typename T,
typename std::enable_if<!boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
T acos( const T & x)
{
	return std::acos(x);
}

template<typename T,
typename std::enable_if<boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
angle_type atan( const T & y)
{
	return boost::units::atan(y);
}
template<typename T,
typename std::enable_if<!boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
flt_t atan( const T & y)
{
	return std::atan(y);
}

template<typename T,
typename std::enable_if<boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
angle_type atan2( const T & y, const T & x=1.)
{
	return boost::units::atan2(y,x);
}
template<typename T,
typename std::enable_if<!boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
flt_t atan2( const T & y, const T & x=1.)
{
	return std::atan2(y,x);
}

#endif // Math functions

} // namespace IceBRG

// Serialization

namespace boost {
namespace serialization {

template<class Archive, class T>
void save(Archive & ar, const boost::units::quantity<T> & g, const unsigned int )
{
	IceBRG::flt_t val = g.value();
    ar & val;
}
template<class Archive, class T>
void load(Archive & ar, boost::units::quantity<T> & g, const unsigned int )
{
	IceBRG::flt_t val;
	ar & val;
    g = IceBRG::units_cast<boost::units::quantity<T>>(std::move(val));
}

template<class Archive, class T>
inline void serialize(Archive & ar, boost::units::quantity<T> & g, const unsigned int file_version)
{
        split_free(ar, g, file_version);
}

} // namespace serialization
} // namespace boost

#else // #ifdef _BRG_USE_UNITS_

#define UNIT_CONSTEXPR constexpr // Will instead be const if using units

namespace IceBRG {

template< short int d_p,
			short int t_p=0,
			short int m_p=0,
			short int a_p=0,
			short int T_p=0>
using custom_unit_type = flt_t;

typedef flt_t dimensionless_type;

typedef flt_t distance_type;
typedef flt_t area_type;
typedef flt_t volume_type;
typedef flt_t inverse_distance_type;
typedef flt_t inverse_area_type;
typedef flt_t inverse_volume_type;

typedef flt_t time_type;
typedef flt_t inverse_time_type;

typedef flt_t mass_type;

typedef flt_t angle_type;
typedef flt_t square_angle_type;
typedef flt_t inverse_angle_type;
typedef flt_t inverse_square_angle_type;

typedef flt_t temperature_type;

typedef flt_t velocity_type;
typedef flt_t acceleration_type;

typedef flt_t density_type;
typedef flt_t inverse_density_type;
typedef flt_t surface_density_type;
typedef flt_t inverse_surface_density_type;

typedef flt_t distance_over_angle_type;
typedef flt_t inverse_volume_inverse_mass_type;

typedef flt_t any_units_type;

// Base units

constexpr flt_t meter(1.);
constexpr flt_t second(1.);
constexpr flt_t kilogram(1.);
constexpr flt_t radian(1.);
constexpr flt_t meter_per_second(1.);

constexpr flt_t m = meter;
constexpr flt_t s = second;
constexpr flt_t kg = kilogram;
constexpr flt_t rad = radian;
constexpr flt_t mps = meter_per_second;

template<typename T>
inline flt_t value_of( const T & x)
{
	return static_cast<flt_t>(x);
}

template<typename Tout, typename Tin>
inline Tout units_cast( const Tin & x)
{
	return static_cast<Tout>(x);
}

template<typename Tout, typename Tin>
inline Tout any_cast( const Tin & x)
{
	return static_cast<Tout>(x);
}

template<typename Tout, typename Tin>
inline any_units_type any_units_cast( const Tin & x)
{
	return static_cast<Tout>(x);
}

// Math functions
#if(1)

template<typename T,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
inline T abs( const T & x)
{
	return std::abs(x);
}

template<typename T,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
inline T sqrt( const T & x)
{
  return std::sqrt(x);
}

//template< int_t e>
//flt_t ipow( const flt_t & x)
//{
//	return ipow<e>(x);
//}

template< int_t en, int_t ed>
inline flt_t ipow( const flt_t & x)
{
	return std::pow(x,static_cast<flt_t>(en)/ed);
}

template<typename T,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
inline T sin( const T & x)
{
  return std::sin(x);
}

template<typename T,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
inline T cos( const T & x)
{
  return std::cos(x);
}

template<typename T,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
inline T tan( const T & x)
{
  return std::tan(x);
}

template<typename T,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
inline T asin( const T & x)
{
  return std::asin(x);
}

template<typename T,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
inline T acos( const T & x)
{
  return std::acos(x);
}

template<typename T,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
inline T atan( const T & x)
{
  return std::atan(x);
}

template<typename T1, typename T2,
typename std::enable_if<!is_boost_tuple<T1>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T2>::value,char>::type = 0>
inline T1 atan2( const T1 & y, const T2 & x)
{
  return std::atan2(y,x);
}

#endif // Math functions

} // namespace IceBRG


#endif // #ifdef _BRG_USE_UNITS_ // #else

#endif // BRG_UNITS_UNITS_HPP_
