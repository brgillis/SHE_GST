/**********************************************************************\
  @file lensing_tNFW_caches.cpp

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

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/math/cache/cache_3d.hpp"
#include "SHE_GST_IceBRG_main/math/cache/cache_4d.hpp"
#include "SHE_GST_IceBRG_main/units/units.hpp"

#include "SHE_GST_IceBRG_lensing/lensing_tNFW_profile.hpp"
#include "SHE_GST_IceBRG_lensing/detail/lensing_tNFW_caches.hpp"

namespace IceBRG {

// Initialisation for IceBRG::tNFW_sig_cache
DEFINE_BRG_CACHE_3D( tNFW_sig_cache,
		flt_t,flt_t,flt_t,surface_density_type,
		std::log(1e9*unitconv::Msuntokg),std::log(1e15*unitconv::Msuntokg),(std::log(1e15)-std::log(1e9))/300,
		0.1,1.5,0.1,
		std::log(1*unitconv::kpctom),std::log(10000*unitconv::kpctom),(std::log(10000)-std::log(1))/300
		,
			const auto mass = units_cast<mass_type>(std::exp(in_param_1));
			const auto z = in_param_2;
			const auto R = units_cast<distance_type>(std::exp(in_param_3));
			IceBRG::lensing_tNFW_profile profile(mass,z);
			return profile.Delta_Sigma( R );
		,

		,

)

// Initialisation for IceBRG::tNFW_offset_sig_cache
DEFINE_BRG_CACHE_4D( tNFW_offset_sig_cache,
		flt_t,flt_t,flt_t,flt_t,surface_density_type,
		std::log(1e13*unitconv::Msuntokg),std::log(1e16*unitconv::Msuntokg),(std::log(1e16)-std::log(1e13))/30,
		0.1,1.5,0.4,
		std::log(0.1*unitconv::kpctom),std::log(4000*unitconv::kpctom),(std::log(4000)-std::log(0.1))/1000,
		std::log(0.01*unitconv::kpctom),std::log(2000*unitconv::kpctom),(std::log(2000)-std::log(0.01))/100
		,
			const auto mass = units_cast<mass_type>(std::exp(in_param_1));
			const auto z = in_param_2;
			const auto R = units_cast<distance_type>(std::exp(in_param_3));
			const auto offset_R = units_cast<distance_type>(std::exp(in_param_4));
			IceBRG::lensing_tNFW_profile profile(mass,z);
			return profile.offset_Delta_Sigma( R, offset_R );
		,
			tNFW_sig_cache().load();
		,

)

// Initialisation for IceBRG::tNFW_group_sig_cache
DEFINE_BRG_CACHE_4D( tNFW_group_sig_cache,
		flt_t,flt_t,flt_t,flt_t,surface_density_type,
		std::log(1e13*unitconv::Msuntokg),std::log(1e15*unitconv::Msuntokg),(std::log(1e16)-std::log(1e13))/30,
		0.1,1.5,0.4,
		std::log(0.1*unitconv::kpctom),std::log(2000*unitconv::kpctom),(std::log(2000)-std::log(0.1))/100,
		2.5,10.,2.5
		,
			const auto mass = units_cast<mass_type>(std::exp(in_param_1));
			const auto z = in_param_2;
			const auto R = units_cast<distance_type>(std::exp(in_param_3));
			const auto group_c = in_param_4;
			IceBRG::lensing_tNFW_profile profile(mass,z);
			return profile.semiquick_group_Delta_Sigma( R, group_c );
		,
			tNFW_offset_sig_cache().load();
		,

)

// Initialisation for IceBRG::tNFW_Sigma_cache
DEFINE_BRG_CACHE_3D( tNFW_Sigma_cache,
		flt_t,flt_t,flt_t,surface_density_type,
		std::log(1e7*unitconv::Msuntokg),std::log(1e16*unitconv::Msuntokg),(std::log(1e16)-std::log(1e7))/300,
		0.1,1.5,0.1,
		std::log(0.1*unitconv::kpctom),std::log(2000*unitconv::kpctom),(std::log(2000)-std::log(0.1))/300
		,
			const auto mass = units_cast<mass_type>(std::exp(in_param_1));
			const auto z = in_param_2;
			const auto R = units_cast<distance_type>(std::exp(in_param_3));
			IceBRG::lensing_tNFW_profile profile(mass,z);
			return profile.proj_dens( R );
		,

		,

		)

// Initialisation for IceBRG::tNFW_offset_Sigma_cache
DEFINE_BRG_CACHE_4D( tNFW_offset_Sigma_cache,
		flt_t,flt_t,flt_t,flt_t,surface_density_type,
		std::log(1e13*unitconv::Msuntokg),std::log(1e16*unitconv::Msuntokg),(std::log(1e16)-std::log(1e13))/30,
		0.1,1.5,0.4,
		std::log(0.1*unitconv::kpctom),std::log(4000*unitconv::kpctom),(std::log(4000)-std::log(0.1))/1000,
		std::log(0.1*unitconv::kpctom),std::log(4000*unitconv::kpctom),(std::log(4000)-std::log(0.01))/100
		,
			const auto mass = units_cast<mass_type>(std::exp(in_param_1));
			const auto z = in_param_2;
			const auto R = units_cast<distance_type>(std::exp(in_param_3));
			const auto offset_R = units_cast<distance_type>(std::exp(in_param_4));
			IceBRG::lensing_tNFW_profile profile(mass,z);
			return profile.offset_Sigma( R, offset_R );
		,
			tNFW_Sigma_cache().load();
		,

		)

// Initialisation for IceBRG::tNFW_group_Sigma_cache
DEFINE_BRG_CACHE_4D( tNFW_group_Sigma_cache,
		flt_t,flt_t,flt_t,flt_t,surface_density_type,
		std::log(1e13*unitconv::Msuntokg),std::log(1e16*unitconv::Msuntokg),(std::log(1e16)-std::log(1e13))/30,
		0.1,1.5,0.4,
		std::log(0.1*unitconv::kpctom),std::log(4000*unitconv::kpctom),(std::log(4000)-std::log(0.1))/200,
		2.5,5.0,2.5
		,
			const auto mass = units_cast<mass_type>(std::exp(in_param_1));
			const auto z = in_param_2;
			const auto R = units_cast<distance_type>(std::exp(in_param_3));
			const auto group_c = in_param_4;
			IceBRG::lensing_tNFW_profile profile(mass,z);
			return profile.semiquick_group_Sigma( R, group_c );
		,
			tNFW_offset_Sigma_cache().load();
		,
			tNFW_offset_Sigma_cache().unload();
		)

} // namespace IceBRG


