/**********************************************************************\
  @file tNFW_profile_functors.hpp

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

#ifndef _BRG_TNFW_PROFILE_FUNCTORS_HPP_
#define _BRG_TNFW_PROFILE_FUNCTORS_HPP_

#include <cstdlib>

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_physics/density_profile/tNFW_profile.hpp"

namespace IceBRG
{

class tNFW_solve_rvir_functor
{
private:
	const tNFW_profile *_halo_;

public:

	tNFW_solve_rvir_functor(const tNFW_profile *halo)
	: _halo_(halo)
	{
	}

	density_type operator()( const distance_type &  in_param ) const
	{
		return virial_density_factor*_halo_->rho_crit()-_halo_->enc_dens(in_param);
	}

};

} // end namespace IceBRG

#endif /* _BRG_TNFW_PROFILE_FUNCTORS_HPP_ */
