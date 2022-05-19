/**********************************************************************\
 @file Eigen.hpp
 ------------------

 A file to include the Eigen library, plus set up some basic extensions
 for it - most notably, begin() and end() support

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

#ifndef _BRG_EIGEN_HPP_
#define _BRG_EIGEN_HPP_

#include <boost/serialization/split_free.hpp>
#include <eigen3/Eigen/Core>

#include "SHE_GST_IceBRG_main/common.hpp"

// Free begin and end definitions
#if(1)
namespace Eigen {

template <typename Derived, int Nrow, int Ncol, int Options, int Maxrow, int Maxcol>
auto begin(const Eigen::Array<Derived,Nrow,Ncol,Options,Maxrow,Maxcol> & vec) -> decltype(vec.data())
{
	return vec.data();
}

template <typename Derived, int Nrow, int Ncol, int Options, int Maxrow, int Maxcol>
auto begin(Eigen::Array<Derived,Nrow,Ncol,Options,Maxrow,Maxcol> & vec) -> decltype(vec.data())
{
	return vec.data();
}

template <typename Derived, int Nrow, int Ncol, int Options, int Maxrow, int Maxcol>
auto end(const Eigen::Array<Derived,Nrow,Ncol,Options,Maxrow,Maxcol> & vec) -> decltype(vec.data())
{
	return vec.data()+vec.size();
}

template <typename Derived, int Nrow, int Ncol, int Options, int Maxrow, int Maxcol>
auto end(Eigen::Array<Derived,Nrow,Ncol,Options,Maxrow,Maxcol> & vec) -> decltype(vec.data())
{
	return vec.data()+vec.size();
}

} // namespace Eigen

#endif // Global namespace begin and end definitions

// Array typedefs
#if(1)

namespace IceBRG {

template<typename T, ssize_t Nrow=Eigen::Dynamic, ssize_t Ncol=1>
using array_t = Eigen::Array<T,Nrow,Ncol>;

typedef array_t<flt_t> flt_array_t;
typedef array_t<long_flt_t> long_flt_array_t;

typedef array_t<int_t> int_array_t;
typedef array_t<long_int_t> long_int_array_t;
typedef array_t<short_int_t> short_int_array_t;

typedef array_t<unsigned_int_t> unsigned_int_array_t;
typedef array_t<long_unsigned_int_t> long_unsigned_int_array_t;
typedef array_t<short_unsigned_int_t> short_unsigned_int_array_t;

typedef array_t<complex_t> complex_array_t;
typedef array_t<long_complex_t> long_complex_array_t;

}
#endif // Array typedefs

// Serialization
#if(1)

namespace boost {
namespace serialization {

template<typename Archive, typename T_value>
void save(Archive & ar, const Eigen::Array<T_value,-1,-1,1,-1,-1> & v, const unsigned int version)
{
	::IceBRG::int_t nrows(v.rows());
	::IceBRG::int_t ncols(v.cols());

	ar << nrows << ncols;

	for(const auto & val : v)
	{
		ar << val;
	}
}
template<typename Archive, typename T_value>
void load(Archive & ar, Eigen::Array<T_value,-1,-1,1,-1,-1> & v, const unsigned int version)
{
	IceBRG::int_t nrows;
	IceBRG::int_t ncols;

	ar >> nrows >> ncols;

	v.resize(nrows,ncols);

	for(auto & val : v)
	{
		ar >> val;
	}
}

template<typename Archive, typename T_value>
inline void serialize(
        Archive & ar,
		Eigen::Array<T_value,-1,-1,1,-1,-1> & t,
        const unsigned int file_version
)
{
        split_free(ar, t, file_version);
}

} // namespace serialization
} // namespace boost

#endif

#endif // _BRG_EIGEN_HPP_
