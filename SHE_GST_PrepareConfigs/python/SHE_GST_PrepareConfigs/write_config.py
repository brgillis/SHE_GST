""" @file write_config.py

    Created 25 Aug 2017

    Contains function to write out a configuration file.
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

from SHE_PPT.file_io import replace_multiple_in_file
from SHE_GST_PrepareConfigs import magic_values as mv

def write_config( filename,
                  template_filename,
                  model_seed=None,
                  noise_seed=None,
                  suppress_noise=None,
                  num_detectors=None,
                  num_galaxies=None,
                  render_background=None):
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
    
    def add_replacement(replacement_tag, value, dtype, inrange = lambda v : True):
        if value is None:
            return
        if not isinstance(value, dtype):
            raise TypeError("Invalid replacement type for " + replacement_tag + ". Should be " + str(dtype) + "." )
        if not inrange(value):
            raise ValueError("Replacement value for " + replacement_tag + " is out of range.")
        
        input_strings.append(replacement_tag)
        output_strings.append(str(value))
    
    # Check validity of each variable, and add to input/output strings if not None
    add_replacement(mv.repstr_model_seed, model_seed, dtype=int)
    add_replacement(mv.repstr_noise_seed, noise_seed, dtype=int)
    add_replacement(mv.repstr_suppress_noise, model_seed, dtype=bool)
    add_replacement(mv.repstr_num_detectors, model_seed, dtype=int,
                    inrange = lambda v : (v>=1) and (v<=36))
    add_replacement(mv.repstr_num_galaxies, model_seed, dtype=int,
                    inrange = lambda v : (v>=1))
    add_replacement(mv.repstr_render_background, model_seed, dtype=bool)
    
    replace_multiple_in_file(template_filename, filename, input_strings, output_strings)
    
    return
        