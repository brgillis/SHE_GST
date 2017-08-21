/**********************************************************************\
 @file labeled_array_iterator_chooser.hpp
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

// body file: labeled_array_iterator_chooser.cpp

#ifndef _BRG_BRG_CONTAINER_LABELED_ARRAY_LABELED_ARRAY_ITERATOR_CHOOSER_HPP_INCLUDED_
#define _BRG_BRG_CONTAINER_LABELED_ARRAY_LABELED_ARRAY_ITERATOR_CHOOSER_HPP_INCLUDED_

#include <type_traits>

#include <boost/type_traits.hpp>

#include "SHE_GST_IceBRG_main/container/labeled_array/labeled_array_element_iterator.hpp"
#include "SHE_GST_IceBRG_main/Eigen.hpp"


namespace IceBRG {

template< typename value_type, char tag >
struct labeled_array_iterator_chooser {};

template< typename value_type >
struct labeled_array_iterator_chooser<value_type,Eigen::ColMajor>
{
	typedef value_type * col_element_iterator;
	typedef labeled_array_element_iterator<value_type> row_element_iterator;
};

template< typename value_type >
struct labeled_array_iterator_chooser<value_type,Eigen::RowMajor>
{
	typedef labeled_array_element_iterator<value_type> col_element_iterator;
	typedef value_type * row_element_iterator;
};

}

#endif // _BRG_BRG_CONTAINER_LABELED_ARRAY_LABELED_ARRAY_ITERATOR_CHOOSER_HPP_INCLUDED_
