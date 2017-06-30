/**********************************************************************\
  @file astro_caches.h

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

// body file: astro_caches.cpp

#ifndef _BRG_ASTRO_CACHES_H_INCLUDED_
#define _BRG_ASTRO_CACHES_H_INCLUDED_

#include <string>

#include "SHE_GST_IceBRG_main/common.hpp"

#include "SHE_GST_IceBRG_main/math/cache/cache.hpp"
#include "SHE_GST_IceBRG_main/math/cache/cache_2d.hpp"
#include "SHE_GST_IceBRG_main/units/units.hpp"

namespace IceBRG
{

DECLARE_BRG_CACHE(dfa_cache,dfa,flt_t,distance_over_angle_type)

DECLARE_BRG_CACHE_2D(add_cache,ang_di_d,flt_t,flt_t,distance_type)

DECLARE_BRG_CACHE(tfa_cache,tfa,flt_t,time_type)

DECLARE_BRG_CACHE_2D(lum_func_integral_cache,lum_int,flt_t,flt_t,inverse_volume_type)

DECLARE_BRG_CACHE(sigma_r_cache,sigma_r,distance_type,flt_t)

DECLARE_BRG_CACHE_2D(l10_mass_function_cache,massfunc,flt_t,flt_t,inverse_volume_type)

DECLARE_BRG_CACHE_2D(l10_mass_function_integral_cache,mass_int,flt_t,flt_t,inverse_volume_type)

DECLARE_BRG_CACHE(visible_cluster_density_cache,viscdens,flt_t,inverse_volume_type)

DECLARE_BRG_CACHE(visible_clusters_cache,vis_clus,flt_t,inverse_square_angle_type)

DECLARE_BRG_CACHE(visible_galaxy_density_cache,visgdens,flt_t,inverse_volume_type)

DECLARE_BRG_CACHE(visible_galaxies_cache,vis_gal,flt_t,inverse_square_angle_type)

DECLARE_BRG_CACHE(cluster_richness_at_z_cache,crichz,flt_t,flt_t)

DECLARE_BRG_CACHE_2D(cluster_richness_cache,crich,flt_t,flt_t,flt_t)

} // end namespace IceBRG

#endif // __BRG_ASTRO_CACHES_H_INCLUDED__

