/**********************************************************************\
  @file lensing_tNFW_profile.h

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

// body file: lensing_tNFW_profile.cpp

#ifndef _BRG_LENSING_TNFW_PROFILE_H_
#define _BRG_LENSING_TNFW_PROFILE_H_

#include <SHE_GST_IceBRG_physics/density_profile/tNFW_profile.hpp>
#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_physics/detail/lensing_profile_extension.hpp"

namespace IceBRG {

/*
 *
 */
class lensing_tNFW_profile: public tNFW_profile, public lensing_profile_extension<lensing_tNFW_profile> {
private:

	// Lensing related calculation methods
#if (1)
	surface_density_type _proj_dens( const distance_type & R ) const;
	surface_density_type _proj_enc_dens( const distance_type & R ) const;
	mass_type _proj_enc_mass( const distance_type & R ) const;
	surface_density_type _quick_Delta_Sigma( const distance_type & R ) const;
	surface_density_type _quick_offset_Delta_Sigma( const distance_type & R,
			const distance_type & offset_R ) const;
	surface_density_type _quick_group_Delta_Sigma( const distance_type & R,
			const flt_t & group_c ) const;
	surface_density_type _quick_shifted_Delta_Sigma( const distance_type & R ) const;
	surface_density_type _quick_shifted_no_enh_Delta_Sigma( const distance_type & R ) const;
	surface_density_type _quick_Sigma( const distance_type & R ) const;
	surface_density_type _quick_offset_Sigma( const distance_type & R,
			const distance_type & offset_R ) const;
	surface_density_type _quick_group_Sigma( const distance_type & R,
			const flt_t & group_c ) const;

#endif // Lensing related calculation methods

	friend class lensing_profile_extension<lensing_tNFW_profile>;

public:
	// Constructors and destructor
#if (1)
	lensing_tNFW_profile()
	{
	}
	lensing_tNFW_profile( const mass_type & init_mvir0, const flt_t & init_z,
			const flt_t & init_c = -1, const flt_t & init_tau = -1 )
	: tNFW_profile(init_mvir0, init_z, init_c, init_tau)
	{
	}
	virtual ~lensing_tNFW_profile()
	{
	}
#endif // Constructors and destructor

	// Implementations of clone functions
#if (1)
	virtual density_profile *density_profile_clone() const
	{
		return new lensing_tNFW_profile( *this );
	}
	virtual tNFW_profile *tNFW_profile_clone() const
	{
		return new lensing_tNFW_profile( *this );
	}
#endif
};

} // end namespace IceBRG

#endif /* _BRG_LENSING_TNFW_PROFILE_H_ */
