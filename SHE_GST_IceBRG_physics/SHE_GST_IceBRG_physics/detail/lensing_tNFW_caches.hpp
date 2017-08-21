/**********************************************************************\
  @file lensing_tNFW_caches.h

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

// body file: lensing_tNFW_caches.cpp

#ifndef _BRG_TNFW_CACHES_H_
#define _BRG_TNFW_CACHES_H_

#include <string>
#include <vector>

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/math/cache/cache_3d.hpp"
#include "SHE_GST_IceBRG_main/math/cache/cache_4d.hpp"

namespace IceBRG {

DECLARE_BRG_CACHE_3D(tNFW_sig_cache,tNFW_sig,
		flt_t,flt_t,flt_t,surface_density_type)

DECLARE_BRG_CACHE_4D(tNFW_offset_sig_cache,tN_o_sig,
		flt_t,flt_t,flt_t,flt_t,surface_density_type)

DECLARE_BRG_CACHE_4D(tNFW_group_sig_cache,tN_g_sig,
		flt_t,flt_t,flt_t,flt_t,surface_density_type)

DECLARE_BRG_CACHE_3D(tNFW_Sigma_cache,tNFWSigm,
		flt_t,flt_t,flt_t,surface_density_type)

DECLARE_BRG_CACHE_4D(tNFW_offset_Sigma_cache,tNoSigma,
		flt_t,flt_t,flt_t,flt_t,surface_density_type)

DECLARE_BRG_CACHE_4D(tNFW_group_Sigma_cache,tNgSigma,
		flt_t,flt_t,flt_t,flt_t,surface_density_type)

} // end namespace IceBRG

#endif /* _BRG_TNFW_CACHES_H_ */
