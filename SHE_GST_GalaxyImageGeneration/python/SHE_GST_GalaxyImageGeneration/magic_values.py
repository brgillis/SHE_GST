"""
    @file magic_values.py

    Created 23 Jul 2015

    Magic values for the SHE_GST_GalaxyImageGeneration module
"""

__updated__ = "2018-08-14"

# Copyright (C) 2012-2020 Euclid Science Ground Segment
#
# This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General
# Public License as published by the Free Software Foundation; either version 3.0 of the License, or (at your option)
# any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

# Version info
version_label = 'SS_VER'
version_str = '1.0.4'

galsim_version_label = 'GS_VER'

# Lists of allowed values for certain configuration parameters
allowed_image_types = ['32f', '64f']

# Hard bounds for input
min_num_images = 1
min_image_size = 2
min_pixel_scale = 0.0001

default_output_folder = './sims/'
default_output_file_name_base = 'simulated_image'
default_magnitude_limit = 24.5
default_num_parallel_threads = 1
default_details_output_format = 'fits'
default_image_datatype = '32f'
default_random_seed = 8241573
default_suppress_noise = False
default_shape_noise_cancellation = False
default_compress_images = False
default_dithering_scheme = 'none'

default_num_images = 1
default_gain = 3.3
default_pixel_scale = 0.1 / 3600
default_psf_scale_factor = 5
default_psf_stamp_size = 256
default_psf_center_offset = (-0.5, -2.5)
default_read_noise = 5.4
default_sky_level = 32570.
default_exp_time = 565.  # seconds

default_truncation_radius_factor = 4.5

default_data_dir = "AUX/SHE_GST_GalaxyImageGeneration"

# mag_vis_zeropoint = 25.50087633632 # From ETC
# mag_vis_zeropoint = 25.4534 # From Sami's sims' config file
mag_vis_zeropoint = 25.6527  # From Lance's code
mag_i_zeropoint = 25.3884  # From Lance's code

galsim_sersic_index_min = 0.3
galsim_sersic_index_max = 6.2

galsim_stamp_size_factor = 10

dist_param_tail = "_dist"

fpack_lossless_command = "fpack -g2 -q 0.0 "
fpack_lossy_command = "fpack -g2 -q 4.0 "
rm_command = "rm -f "

bulge_model_head = "2dmodel_bulge_n"
disk_model_head = "3dmodel_disk_n"
galaxy_model_tail = ".dat"
galaxy_model_path = "galaxy_models"

psf_model_path = "psf_models"
psf_model_scale = 0.02  # arcsec/pixel

fftw_wisdom_filename = ".fftw_wisdom"

logger_name = "SHE_GST_GalaxyImageGeneration"

detections_table_ID_label = 'ID'
detections_table_gal_xc_label = 'x_center_pix'
detections_table_gal_yc_label = 'y_center_pix'
detections_table_psf_xc_label = 'psf_x_center_pix'
detections_table_psf_yc_label = 'psf_y_center_pix'

fits_header_subtracted_sky_level_label = "S_SKYLV"
fits_header_unsubtracted_sky_level_label = "US_SKYLV"
fits_header_read_noise_label = "RD_NOISE"
fits_header_gain_label = "CCDGAIN"

# TODO check actual gaps
image_gap_x_pix = 50
image_gap_y_pix = 50
