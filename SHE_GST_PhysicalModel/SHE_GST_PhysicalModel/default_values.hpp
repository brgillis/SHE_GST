/**********************************************************************\
 @file values.h
 ------------------

 Default values for galaxy simulation parameters.

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

#ifndef SHE_SIM_GAL_PARAMS_VALUES_H_
#define SHE_SIM_GAL_PARAMS_VALUES_H_

#include "SHE_GST_PhysicalModel/common.hpp"

namespace SHE_GST_PhysicalModel { namespace dv {

// Survey-level

constexpr const int_t survey_level = 0;

constexpr const flt_t num_image_groups = 1;
constexpr const flt_t pixel_scale = 0.1; // arcsec/pixel

// ImageGroup-level

constexpr const int_t image_group_level = 1;

constexpr const flt_t num_images = 1;

// Image-level

constexpr const int_t image_level = 2;

constexpr const flt_t num_fields = 1.; // #/arcsec^2

constexpr const flt_t image_size_xp = 4096; // pixels
constexpr const flt_t image_size_yp = 2048; // pixels

constexpr const flt_t exp_time = 565.; // seconds

constexpr const flt_t sky_level = 32570.; // ADU/arcsec
constexpr const flt_t subtracted_background_l10_mean = std::log10(sky_level);
constexpr const flt_t subtracted_background_l10_stddev = 0.05;

constexpr const flt_t unsubtracted_background = 0.; // ADU/arcsec

// ClusterGroup-level

constexpr const int_t cluster_group_level = 3;

// FieldGroup-level

constexpr const int_t field_group_level = 3;

// Cluster-level

constexpr const int_t cluster_level = 4;

constexpr const flt_t cluster_redshift_enhancement = 1.;
constexpr const flt_t cluster_redshift_min = 0.2;
constexpr const flt_t cluster_redshift_max = 1.3;

constexpr const flt_t cluster_richness = 7.;

constexpr const flt_t cluster_xp_min = 0.; // pixels
constexpr const flt_t cluster_xp_max = image_size_xp; // pixels

constexpr const flt_t cluster_yp_min = 0.; // pixels
constexpr const flt_t cluster_yp_max = image_size_yp; // pixels

constexpr const flt_t cluster_num_satellites = 6;

// Field-level

constexpr const int_t field_level = 4;

// GalaxyGroup-level

constexpr const int_t galaxy_group_level = 5;

// GalaxyPair-level

constexpr const int_t galaxy_pair_level = 6;

// Galaxy-level

constexpr const int_t galaxy_level = 7;

constexpr const flt_t apparent_size_bulge = 1.; // kpc
constexpr const flt_t apparent_size_disk = 1.; // kpc

constexpr const flt_t apparent_mag_vis_min = 19.; // kpc
constexpr const flt_t apparent_mag_vis_max = 24.5; // kpc

constexpr const flt_t bulge_fraction = 1./3.;

constexpr const flt_t bulge_axis_ratio_high_n = 0.64;
constexpr const flt_t bulge_axis_ratio_low_n = 0.54;
constexpr const flt_t bulge_axis_ratio_n_cutoff = 2.25;

constexpr const flt_t bulge_intrinsic_ellipticity_sigma = 0.25;
constexpr const flt_t bulge_intrinsic_ellipticity_max = 0.9;
constexpr const flt_t bulge_intrinsic_ellipticity_p = 4.;

constexpr const flt_t disk_height_ratio = 0.1;

constexpr const flt_t galaxy_type = 0.;

constexpr const flt_t sersic_index_min = 0.3; // Limits coded into GalSim
constexpr const flt_t sersic_index_max = 6.2;

constexpr const flt_t galaxy_redshift_enhancement = 1.;
constexpr const flt_t galaxy_redshift_min = 0.2;
constexpr const flt_t galaxy_redshift_max = 2.0;

constexpr const flt_t rotation_min = 0.; // degrees
constexpr const flt_t rotation_max = 180.; // degrees

constexpr const flt_t sersic_index = 2.;

constexpr const flt_t shear_angle_min = 0.; // degrees
constexpr const flt_t shear_angle_max = 180.; // degrees

constexpr const flt_t shear_magnitude_sigma = 0.03;
constexpr const flt_t shear_magnitude_max = 0.9;
constexpr const flt_t shear_magnitude_p = 4.;

constexpr const flt_t spin_min = 0.; // degrees
constexpr const flt_t spin_max = 360.; // degrees

constexpr const flt_t theta_sat_min = 0.;
constexpr const flt_t theta_sat_max = 360.;

constexpr const flt_t tilt_cos_min = 0.;
constexpr const flt_t tilt_cos_max = 1.;

constexpr const flt_t xp_min = 0.;
constexpr const flt_t xp_max = image_size_xp;

constexpr const flt_t yp_min = 0.;
constexpr const flt_t yp_max = image_size_yp;

} } // namespace SHE_GST_PhysicalModel{ namespace dv{



#endif // SHE_SIM_GAL_PARAMS_VALUES_H_
