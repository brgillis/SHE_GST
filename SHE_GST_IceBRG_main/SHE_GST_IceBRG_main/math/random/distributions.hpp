/**********************************************************************\
  @file distributions.hpp

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

#ifndef _BRG_DISTRIBUTIONS_HPP_INCLUDED_
#define _BRG_DISTRIBUTIONS_HPP_INCLUDED_

#include <cmath>
#include <cstdlib>

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/math/misc_math.hpp"

namespace IceBRG {

// Error function - forward to std::erf
template< typename Tx >
inline Tx erf(Tx && x)
{
	return std::erf(std::forward<Tx>(x));
}

template< typename Tx >
inline Tx erfc(Tx && x)
{
	return std::erfc(std::forward<Tx>(x));
}

// Gaussian PDF
template< typename Tx, typename Tmean, typename Tstddev >
inline flt_t Gaus_pdf( const Tx x, const Tmean mean,
		const Tstddev std_dev )
{
	return std::exp( -square( x - mean ) / ( 2 * square(std_dev) ) )
			/ ( std_dev * std::sqrt( 2 * pi ) );
}
template< typename Tx, typename Tmean >
inline flt_t Gaus_pdf( const Tx x, const Tmean mean )
{
	return Gaus_pdf(x,mean,1.);
}
template< typename Tx >
inline flt_t Gaus_pdf( const Tx x )
{
	return Gaus_pdf(x,0.,1.);
}

// Spherical Gaussian PDF
template< typename Tr, typename Tstddev >
inline flt_t spherical_Gaus_pdf( const Tr & r, const Tstddev & stddev )
{
	return std::exp(-square(r/stddev)/2.)/std::pow(2*pi*stddev*stddev,1.5);
}
template< typename Tr >
inline flt_t spherical_Gaus_pdf( const Tr & r )
{
	return spherical_Gaus_pdf(r,1.);
}

// Function to integrate a Gaussian from min to max
template< typename Tlo, typename Thi, typename Tmean, typename Tstddev >
inline flt_t Gaus_int( const Tlo min, const Thi max)
{
	return Gaus_int(min,max,0.,1.);
}
template< typename Tlo, typename Thi, typename Tmean>
inline flt_t Gaus_int( const Tlo min, const Thi max,
		const Tmean mean)
{
	return Gaus_int(min,max,mean,1.);
}
template< typename Tlo, typename Thi, typename Tmean, typename Tstddev >
inline flt_t Gaus_int( const Tlo min, const Thi max,
		const Tmean mean, const Tstddev std_dev )
{
	flt_t klo = ( min - mean ) / std_dev;
	flt_t khi = ( max - mean ) / std_dev;

	return std::fabs( erf( khi ) - ( klo ) ) / 2;
}

} // namespace IceBRG

#endif /* _BRG_DISTRIBUTIONS_HPP_INCLUDED_ */
