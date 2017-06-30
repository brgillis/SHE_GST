/**********************************************************************\
 @file shifting_loader.cpp
 ------------------

 This class is used to load in the data tables provided by Chang and
 Jain for the mean relative shift of two points in space, so the data
 can be stored in the same format as my other caches.

 **********************************************************************

 Copyright (C) 2014  Bryan R. Gillis

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


#include "SHE_GST_IceBRG_lensing/shifting/detail/shifting_loader.hpp"

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/file_access/ascii_table.hpp"
#include "SHE_GST_IceBRG_main/math/misc_math.hpp"
#include "SHE_GST_IceBRG_main/units/unit_conversions.hpp"
#include "SHE_GST_IceBRG_main/utility.hpp"
#include "SHE_GST_IceBRG_main/vector/manipulations.hpp"

#include "SHE_GST_IceBRG_lensing/shifting/detail/corr_alpha.hpp"


// Initialisation of static vars
#if (1)
bool IceBRG::shifting_loader::_loaded_(false);
IceBRG::labeled_array<IceBRG::flt_t> IceBRG::shifting_loader::_data_;
#endif

void IceBRG::shifting_loader::_load()
{
	#ifdef _OPENMP
	#pragma omp critical(brg_load_shifting_loader)
	#endif
	{
		if(!_loaded_)
		{
			std::stringstream ss(corr_alph_data);

			std::vector<std::string> labels({"theta"});

			for( const auto & zval : _zvals_)
			{
				labels.push_back(boost::lexical_cast<std::string>(zval));
			}

			_data_.read(ss);
			_data_.set_labels(std::move(labels));

			assert(_data_.num_cols()==_zvals_size_+1);
			assert(_data_.num_rows()>=2);

			_data_.reverse_vertical();

			// Change to SI units
			_data_.at_label("theta").raw() *= unitconv::amintorad;

			for( const auto & zval : _zvals_)
			{
				_data_.at_label(boost::lexical_cast<std::string>(zval)).raw()
					*= square(unitconv::amintorad);
			}

			_loaded_ = true;
		}
	}

}
ssize_t IceBRG::shifting_loader::_lower_theta_index(const angle_type & theta)
{
	if(!_loaded_) _load();

	auto size = _data_.num_rows();

	for(ssize_t i=1; i<size; ++i)
	{
		if(theta<units_cast<angle_type>(_data_.col(0).row(i)))
			return i-1;
	}
	return size-2;
}
ssize_t IceBRG::shifting_loader::_lower_z_index(const flt_t & z)
{
	assert(_zvals_size_>=2);

	for(ssize_t i=1; i<_zvals_size_; ++i)
	{
		if(z<_zvals_[i])
			return i-1;
	}
	return _zvals_size_-2;
}

IceBRG::square_angle_type IceBRG::shifting_loader::get(const angle_type & t, const flt_t & z)
{
	if(!_loaded_) _load();

	const ssize_t ti = _lower_theta_index(t);
	const ssize_t zi = _lower_z_index(z);

	const angle_type & tlo = units_cast<angle_type>(_data_.col(0).row(ti));
	const angle_type & thi = units_cast<angle_type>(_data_.col(0).row(ti+1));
	const flt_t & zlo = _zvals_[zi];
	const flt_t & zhi = _zvals_[zi+1];

	const angle_type weight = (thi-tlo)*(zhi-zlo);

	square_angle_type result;

	decltype(weight*result) weighted_result;

	weighted_result = units_cast<square_angle_type>(_data_.col(zi+1).row(ti))*(zhi-z)*(thi-t);
	weighted_result += units_cast<square_angle_type>(_data_.col(zi+1).row(ti+1))*(zhi-z)*(t-tlo);
	weighted_result += units_cast<square_angle_type>(_data_.col(zi+2).row(ti))*(z-zlo)*(thi-t);
	weighted_result += units_cast<square_angle_type>(_data_.col(zi+2).row(ti+1))*(z-zlo)*(t-tlo);

	result = weighted_result/weight;

	return result;
}

void IceBRG::shifting_loader::unload()
{
	_data_.clear();
	_loaded_ = false;
}
