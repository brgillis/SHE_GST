/**********************************************************************\
 @file sersic_index.hpp
 ------------------

 Functions for estimating the sersic indices of galaxies.

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

#ifndef SHE_SIM_GAL_PARAMS_DEPENDENCY_FUNCTIONS_sersic_index_HPP_
#define SHE_SIM_GAL_PARAMS_DEPENDENCY_FUNCTIONS_sersic_index_HPP_

#include "SHE_GST_PhysicalModel/common.hpp"
#include "SHE_GST_IceBRG_main/Eigen.hpp"


namespace SHE_GST_PhysicalModel {

constexpr int_t num_bulge_classes=5;

struct sersic_index_data
{
	static const IceBRG::array_t<flt_t,num_bulge_classes,1> sm_coeffs;
	static const IceBRG::array_t<flt_t,num_bulge_classes,1> z_coeffs;
	static const IceBRG::array_t<flt_t,num_bulge_classes,1> y_intercepts;

	static const IceBRG::array_t<flt_t,num_bulge_classes,1> bulge_sersic_means;
	static const IceBRG::array_t<flt_t,num_bulge_classes,1> bulge_sersic_stds;
};

IceBRG::array_t<flt_t,num_bulge_classes,1> load_morphology_sm_coeffs();
IceBRG::array_t<flt_t,num_bulge_classes,1> load_morphology_z_coeffs();
IceBRG::array_t<flt_t,num_bulge_classes,1> load_morphology_y_intercepts();

IceBRG::array_t<flt_t,num_bulge_classes,1> load_bulge_sersic_means();
IceBRG::array_t<flt_t,num_bulge_classes,1> load_bulge_sersic_stds();


flt_t generate_bulge_class( flt_t const & stellar_mass, flt_t const & redshift, gen_t & rng );

flt_t generate_sersic_index_from_apparent_mag_vis( flt_t const & apparent_mag_vis, gen_t & rng );
flt_t generate_sersic_index_from_bulge_class( flt_t const & bulge_class, gen_t & rng );

flt_t generate_bulge_fraction( flt_t const & apparent_mag_vis, flt_t const & sersic_index, gen_t & rng );

flt_t get_bulge_class_from_fraction( flt_t const & bulge_fraction );
flt_t get_bulge_fraction_from_class( flt_t const & bulge_class );

flt_t get_bulge_axis_ratio( flt_t const & sersic_index );

flt_t get_bulge_ellipticity( flt_t const & bulge_axis_ratio, flt_t const & tilt );

}

#endif // SHE_SIM_GAL_PARAMS_DEPENDENCY_FUNCTIONS_sersic_index_HPP_
