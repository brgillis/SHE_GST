""" @file write_configs.py

    Created 25 Aug 2017

    Contains functions to write out configuration files.
"""

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

from astropy.table import Table
import numpy as np
import os

from SHE_PPT.file_io import (get_allowed_filename, replace_multiple_in_file,
                             write_pickled_product, write_listfile, find_file,
                             read_pickled_product, get_data_filename)
from SHE_PPT import products
from SHE_PPT.table_formats.simulation_plan import tf as sptf , \
    simulation_plan_table_format
from SHE_PPT.table_utility import is_in_format

products.simulation_config.init()
products.simulation_plan.init()

from SHE_GST_PrepareConfigs import magic_values as mv

def write_configs_from_plan(plan_filename,
                             template_filename,
                             listfile_filename,
                             workdir):
    """Writes out configuration files based on a template and plan.

    Parameters
    ----------
    plan_filename : str
        Filename of the plan table or XML product
    template_filename : str
        Filename of the template configuration file
    listfile_filename : str
        Desired name of the listfile of config files
    workdir : str
        Work directory - where files will be generated

    Raises
    ------
    TypeError
        One of the input variables is of invalid type
    ValueError
        num_detectors is outside of the range 1-36 or num_galaxies is < 1

    Returns
    -------
    None

    """

    qualified_plan_filename = find_file(get_data_filename(plan_filename, workdir), workdir)
    qualified_template_filename = find_file(template_filename, path = workdir)

    # Read in the plan table
    simulation_plan_table = None
    try:
        simulation_plan_table = Table.read(qualified_plan_filename, format = "fits")
    except Exception as _e2:
        # Not a known table format, maybe an ascii table?
        try:
            simulation_plan_table = Table.read(qualified_plan_filename, format = "ascii")
        except IOError as _e3:
            pass
    # If it's still none, we couldn't identify it, so raise the initial exception
    if simulation_plan_table is None:
        raise TypeError("Unknown file format for simulation plan table in " + qualified_plan_filename)

    # FIXME - fix is_in_format so strict=False won't care about different int types
#     if not is_in_format(simulation_plan_table, sptf):
#         raise TypeError("Table stored in " + qualified_plan_filename + " is of invalid type.")

    # Keep a list of all configuration files generated
    all_config_products = []

    # Keep a list of tags used so we don't overlap
    tags_used = set()

    # Get column references first for efficiency
    tags = simulation_plan_table[sptf.tag]
    model_seed_mins = simulation_plan_table[sptf.model_seed_min]
    model_seed_maxes = simulation_plan_table[sptf.model_seed_max]
    model_seed_steps = simulation_plan_table[sptf.model_seed_step]
    noise_seed_mins = simulation_plan_table[sptf.noise_seed_min]
    noise_seed_maxes = simulation_plan_table[sptf.noise_seed_max]
    noise_seed_steps = simulation_plan_table[sptf.noise_seed_step]
    suppress_noises = simulation_plan_table[sptf.suppress_noise]
    num_detectorses = simulation_plan_table[sptf.num_detectors]
    num_galaxieses = simulation_plan_table[sptf.num_galaxies]
    render_backgrounds = simulation_plan_table[sptf.render_background]

    # Write up configs for the plan in each row of the table
    for row_index in range(len(simulation_plan_table)):

        desired_tag = tags[row_index]

        if desired_tag not in tags_used:
            tags_used.add(desired_tag)
            tag = desired_tag
        else:
            i = 0
            test_tag = desired_tag + "_" + str(i)
            while test_tag not in tags_used:
                i += 1
                test_tag = desired_tag + "_" + str(i)
            tags_used.add(test_tag)
            tag = test_tag

        # Get lists of model and noise seed values, and check they're the same length

        mseed_min = model_seed_mins[row_index]
        mseed_max = model_seed_maxes[row_index]
        mseed_step = model_seed_steps[row_index]

        if mseed_step <= 0:
            raise ValueError("Model seed step cannot be <= zero.")

        if mseed_max < mseed_min:
            raise ValueError("Model seed max cannot be less than model seed min")

        model_seeds = np.linspace(start = mseed_min,
                                  stop = mseed_max,
                                  num = (mseed_max - mseed_min) // mseed_step + 1,
                                  endpoint = True)

        nseed_min = noise_seed_mins[row_index]
        nseed_max = noise_seed_maxes[row_index]
        nseed_step = noise_seed_steps[row_index]

        if mseed_step == 0:
            raise ValueError("Noise seed step cannot be zero.")

        if nseed_max < nseed_min:
            raise ValueError("Nois seed max cannot be less than noise seed min")

        noise_seeds = np.linspace(start = nseed_min,
                                  stop = nseed_max,
                                  num = (nseed_max - nseed_min) // nseed_step + 1,
                                  endpoint = True)

        if len(model_seeds) != len(noise_seeds):
            raise ValueError("Plan gives different lengths for sets of model seeds and noise seeds.")

        suppress_noise = suppress_noises[row_index]
        num_detectors = num_detectorses[row_index]
        num_galaxies = num_galaxieses[row_index]
        render_background = render_backgrounds[row_index]

        # Write a config file for each model/noise seed value

        for i in range(len(model_seeds)):

            prod_filename = get_allowed_filename(type_name = "GST_CFG_P",
                                                 instance_id = tag + "-" + str(i),
                                                 extension = ".xml")
            filename = get_allowed_filename(type_name = "GST_CFG",
                                                 instance_id = tag + "-" + str(i),
                                                 extension = ".txt")

            cfg_prod = products.simulation_config.create_simulation_config_product(filename)
            write_pickled_product(cfg_prod, os.path.join(workdir, prod_filename))
            all_config_products.append(prod_filename)

            write_config(filename = os.path.join(workdir, filename),
                         template_filename = qualified_template_filename,
                         model_seed = int(model_seeds[i]),
                         noise_seed = int(noise_seeds[i]),
                         suppress_noise = suppress_noise,
                         num_detectors = num_detectors,
                         num_galaxies = num_galaxies,
                         render_background = render_background)

    # Write out the listfile of these files
    write_listfile(os.path.join(workdir, listfile_filename), all_config_products)

    return


def write_config(filename,
                  template_filename,
                  model_seed = None,
                  noise_seed = None,
                  suppress_noise = None,
                  num_detectors = None,
                  num_galaxies = None,
                  render_background = None):
    """Writes a configuration file based on a template.

    Parameters
    ----------
    filename : str
        Desired filename of the configuration file to write
    template_filename : str
        Filename of the template configuration file
    model_seed : int
        Model seed to write in the config file
    noise_seed : int
        Noise seed to write in the config file
    suppress_noise : bool
        Whether or not to suppress noise
    num_detectors : int
        Number of detectors per FOV desired. Range 1-36
    num_galaxies : int
        Number of galaxies per detector desired
    render_background : int
        Whether or not to render background galaxies

    Raises
    ------
    TypeError
        One of the input variables is of invalid type
    ValueError
        num_detectors is outside of the range 1-36 or num_galaxies is < 1

    Returns
    -------
    None

    """

    input_strings = []
    output_strings = []

    def add_replacement(replacement_tag, value, dtype, inrange = lambda _v : True):
        if value is None:
            return
        value = dtype(value)
        if not inrange(value):
            raise ValueError("Replacement value for " + replacement_tag + " is out of range.")

        input_strings.append(replacement_tag)
        output_strings.append(str(value))

    def str2bool(v):
        if str(v).lower() == 'true':
            return True
        else:
            return False

    # Check validity of each variable, and add to input/output strings if not None
    add_replacement(mv.repstr_model_seed, model_seed, dtype = int)
    add_replacement(mv.repstr_noise_seed, noise_seed, dtype = int)
    add_replacement(mv.repstr_suppress_noise, suppress_noise, dtype = str2bool)
    add_replacement(mv.repstr_num_detectors, num_detectors, dtype = int,
                    inrange = lambda v : (v >= 1) and (v <= 36))
    add_replacement(mv.repstr_num_galaxies, num_galaxies, dtype = int,
                    inrange = lambda v : (v >= 1))
    add_replacement(mv.repstr_render_background, render_background, dtype = str2bool)

    replace_multiple_in_file(template_filename, filename,
                             input_strings, output_strings)

    return

