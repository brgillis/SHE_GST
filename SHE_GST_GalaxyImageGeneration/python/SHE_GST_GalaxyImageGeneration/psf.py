""" @file psf.py

    Created 11 Dec 2015

    @TODO: File docstring
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

from copy import deepcopy
from functools import lru_cache
from os.path import join

from astropy.io import fits
from astropy.table import Table
import galsim

import SHE_GST_GalaxyImageGeneration.magic_values as mv
from SHE_PPT.file_io import find_file, append_hdu
from SHE_PPT.magic_values import bulge_psf_tag, disk_psf_tag
from SHE_PPT.table_formats.psf import tf as pstf
from SHE_PPT.table_utility import table_to_hdu
from coverage.html import data_filename
import numpy as np


# Magic values for this module
sed_names = {'ell': 'el_cb2004a_001',
             'sbc': 'sbc_cb2004a_001',
             'scd': 'scd_cb2004a_001',
             'sb2': 'sb2_b2004a_001',
             'sb3': 'sb3_b2004a_001',
             }

seds = {4.0: 'sbc',
        3.5: 'sbc',
        3.0: 'sbc',
        2.71: 'scd',
        2.56: 'sb2',
        2.0: 'sb3',
        1.8: 'sb3',
        }

allowed_ns = np.array((1.8, 2.0, 2.56, 2.71, 3.0, 3.5, 4.0))
allowed_zs = np.array((0., 0.5, 1.0, 1.5, 2.0))

gal_id_label = "GAL_ID"
exposure_index_label = "EX_INDEX"
scale_label = "GS_SCALE"
type_label = "PSF_TYPE"


@lru_cache()
def load_psf_model_from_sed_z(sed,
                              z=0.0,
                              data_dir=mv.default_data_dir,
                              gsparams=galsim.GSParams(),
                              workdir="."):

    z_str = "%0.2f" % z

    model_filename = join(data_dir, "psf_models", sed_names[sed] + ".fits_0.000_0.804_" + z_str + ".fits")

    qualified_filename = find_file(model_filename, workdir)

    return load_psf_model_from_file(qualified_filename,
                                    scale=mv.psf_model_scale,
                                    offset=mv.default_psf_center_offset,
                                    gsparams=gsparams)


@lru_cache()
def load_psf_model_from_file(file_name,
                             scale,
                             offset,
                             gsparams=galsim.GSParams()):

    model = galsim.fits.read(file_name)

    return galsim.InterpolatedImage(model,
                                    scale=scale,
                                    offset=offset,
                                    gsparams=gsparams)


@lru_cache()
def get_background_psf_profile(gsparams=galsim.GSParams()):

    prof = galsim.OpticalPSF(lam=725,  # nm
                             diam=1.2,  # m
                             defocus=0,
                             obscuration=0.33,
                             nstruts=3,
                             gsparams=gsparams,
                             )

    return prof


def get_psf_profile(n,
                    z,
                    bulge,
                    use_background_psf=False,
                    data_dir=mv.default_data_dir,
                    model_psf_file_name=None,
                    model_psf_scale=mv.psf_model_scale,
                    model_psf_offset=mv.default_psf_center_offset,
                    gsparams=galsim.GSParams(),
                    workdir="."):

    if use_background_psf:
        return get_background_psf_profile(gsparams=gsparams)

    if model_psf_file_name is not None:
        qualified_model_filename = find_file(model_psf_file_name, workdir)
        return load_psf_model_from_file(qualified_model_filename, model_psf_scale, model_psf_offset, gsparams=gsparams)

    diffs = np.abs(allowed_zs - z)
    zi_best = np.argmin(diffs)

    if(bulge):
        sed = 'ell'
    else:
        diffs = np.abs(allowed_ns - n)
        ni_best = np.argmin(diffs)
        sed = seds[allowed_ns[ni_best]]

    return load_psf_model_from_sed_z(sed, allowed_zs[zi_best], gsparams=gsparams, data_dir=data_dir)


@lru_cache()
def create_psf_hdu(psf_profile,
                   stamp_size=mv.default_psf_stamp_size,
                   scale=mv.default_pixel_scale / mv.default_psf_scale_factor,):
    """Creates an HDU of an image of a PSF profile.
    """

    # Draw the profile onto an image of the proper size
    psf_image = galsim.ImageF(stamp_size, stamp_size, scale=scale)
    psf_profile.drawImage(psf_image)

    # Set up an image HDU with this image
    psf_hdu = fits.ImageHDU(data=psf_image.array)

    # Return
    return psf_hdu


def add_psf_to_archive(psf_profile,
                       archive_hdulist,
                       galaxy_id,
                       exposure_index,
                       psf_type,
                       stamp_size=mv.default_psf_stamp_size,
                       scale=mv.default_pixel_scale / mv.default_psf_scale_factor):
    """Creates a PSF HDU and saves it to an archive file.
    """

    # Create the HDU
    psf_hdu = deepcopy(create_psf_hdu(psf_profile=psf_profile,
                                      stamp_size=stamp_size,
                                      scale=scale))

    # Add needed keywords to the header of this HDU
    psf_hdu.header[gal_id_label] = galaxy_id
    psf_hdu.header[exposure_index_label] = exposure_index
    psf_hdu.header[scale_label] = scale
    psf_hdu.header[type_label] = psf_type

    # Append the HDU to the archive

    archive_hdulist.append(psf_hdu)

    return


def get_psf_from_archive(archive_hdulist,
                         galaxy_id,
                         exposure_index):

    for hdu in archive_hdulist:
        if hdu.header[gal_id_label] == galaxy_id and hdu.header[exposure_index_label] == exposure_index:

            psf_image = galsim.ImageF(hdu.data, scale=hdu.header[scale_label])

            return psf_image

    raise ValueError("PSF for galaxy " + str(galaxy_id) + " for exposure " + str(exposure_index) +
                     " not found in PSF archive.")


def sort_psfs_from_archive(psf_table,
                           psf_data_filename,
                           psf_archive_hdulist,
                           exposure_index,
                           workdir="."):

    qualified_psf_data_filename = join(workdir, psf_data_filename)

    psf_table_hdu = table_to_hdu(psf_table)
    data_hdulist = fits.open(qualified_psf_data_filename, mode='append')
    data_hdulist.append(psf_table_hdu)

    psf_table.remove_indices(pstf.ID)  # Necessary for bug workaround in astropy
    psf_table.add_index(pstf.ID)  # Allow it to be indexed by galaxy ID

    hdu_index = 2  # Start indexing at 2, since 0 is empty and 1 is table
    for hdu in psf_archive_hdulist:

        if hdu.header[exposure_index_label] != exposure_index:
            continue

        gal_id = hdu.header[gal_id_label]

        header = deepcopy(hdu.header)

        is_bulge = header[type_label] == "bulge"

        # Set up the header to point to galaxy id
        if is_bulge:
            header['EXTNAME'] = str(gal_id) + "." + bulge_psf_tag
        else:
            header['EXTNAME'] = str(gal_id) + "." + disk_psf_tag

        # Add to the data file
        psf_hdu = fits.ImageHDU(data=hdu.data,
                                header=header)
        data_hdulist.append(psf_hdu)

        # Update the PSF table with the HDU index of this
        if is_bulge:
            psf_table.loc[gal_id][pstf.bulge_index] = hdu_index
        else:
            psf_table.loc[gal_id][pstf.disk_index] = hdu_index
        hdu_index += 1

    # Now that the table is complete, update the values for it in the fits file
    out_table = data_hdulist[1].data

    out_table[pstf.bulge_index] = psf_table[pstf.bulge_index]
    out_table[pstf.disk_index] = psf_table[pstf.disk_index]

    data_hdulist.flush()
    data_hdulist.close()

    return
