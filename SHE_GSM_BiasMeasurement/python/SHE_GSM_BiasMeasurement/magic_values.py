""" @file magic_values.py

    Created 7 Apr 2017

    Magic values for measuring bias of shear estimates.

    ---------------------------------------------------------------------

    Copyright (C) 2017 Bryan R. Gillis

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
"""

from SHE_SIM_galaxy_image_generation import magic_values as sim_mv
from SHE_GSM_ShearEstimation import magic_values as est_mv

logger_name = "SHE_GSM_MeasureBias"

default_output_filename = "shear_biases.fits"

fits_table_ID_label = sim_mv.detections_table_ID_label
fits_table_sim_g1_label = "GAL_SIM_G1"
fits_table_sim_g2_label = "GAL_SIM_G2"
fits_table_est_g1_label = est_mv.fits_table_gal_g1_label
fits_table_est_g2_label = est_mv.fits_table_gal_g2_label
fits_table_est_gerr_label = est_mv.fits_table_gal_gerr_label

details_table_g_label = 'shear_magnitude'
details_table_beta_label = 'shear_angle'
