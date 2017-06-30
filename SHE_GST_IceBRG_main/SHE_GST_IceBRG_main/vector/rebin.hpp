/**********************************************************************\
 @file rebin.hpp
 ------------------

 TODO <Insert file description here>

 **********************************************************************

 Copyright (C) 2016 brg

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

\**********************************************************************/

#ifndef ICEBRG_MAIN_VECTOR_REBIN_HPP_
#define ICEBRG_MAIN_VECTOR_REBIN_HPP_

#include <cmath>
#include <cstdio>
#include <type_traits>

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/Eigen.hpp"
#include "SHE_GST_IceBRG_main/container/is_eigen_container.hpp"

namespace IceBRG {

template< typename T >
typename std::enable_if<is_eigen_container<T>::value,
Eigen::Array<typename T::Scalar, Eigen::Dynamic,Eigen::Dynamic>>::type
	rebin( Eigen::DenseBase< T > const & subsampled_image,
			int_t const & x_shift=0,
			int_t const & y_shift=0,
			int_t const & subsampling_factor=1 )
{
	typedef Eigen::Array<typename T::Scalar,Eigen::Dynamic,Eigen::Dynamic> Tout;

	ssize_t ss_nx = subsampled_image.cols();
	ssize_t ss_ny = subsampled_image.rows();

	// Get the size of the final image
	ssize_t rb_nx = 2 * (((ss_nx-2*std::abs(x_shift))/subsampling_factor - 1) / 2) + 1;
	ssize_t rb_ny = 2 * (((ss_ny-2*std::abs(y_shift))/subsampling_factor - 1) / 2) + 1;

	int_t x_offset = (ss_nx - rb_nx*subsampling_factor)/2 - x_shift;
	int_t y_offset = (ss_ny - rb_ny*subsampling_factor)/2 - y_shift;

	// Make the rebinned array
	Tout rebinned_array = Tout::Zero(rb_nx,rb_ny);

	for( int xi=0; xi<rb_nx; ++xi)
	{
		for( int yi=0; yi<rb_ny; ++yi)
		{
			int_t xm = x_offset + subsampling_factor*xi;
			int_t ym = y_offset + subsampling_factor*yi;

			rebinned_array(xi,yi) =
					subsampled_image.block(xm,ym,subsampling_factor,subsampling_factor).sum();
		}
	}

	return rebinned_array;
}

template< typename T >
typename std::enable_if<!is_eigen_container<T>::value,
Eigen::Array<T,Eigen::Dynamic,Eigen::Dynamic>>::type
	rebin( T const * const & p_subsampled_image,
			int_t const & ss_nx,
			int_t const & ss_ny,
			int_t const & x_shift=0,
			int_t const & y_shift=0,
			int_t const & subsampling_factor=1 )
{
	Eigen::Map<const Eigen::Array<T,Eigen::Dynamic,Eigen::Dynamic>>
		subsampled_image(p_subsampled_image, ss_nx, ss_ny);

	return rebin(subsampled_image,
					x_shift,
					y_shift,
					subsampling_factor);
}

} // end namespace IceBRG



#endif // ICEBRG_MAIN_VECTOR_REBIN_HPP_
