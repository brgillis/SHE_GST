"""
    @file combine_dithers.py

    Created 6 Oct 2015

    Function to combine various dithers into a stacked image.
"""

__updated__ = "2021-08-17"

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

import os

from SHE_PPT import products
from SHE_PPT.constants.fits import (SCI_TAG, MASK_TAG, NOISEMAP_TAG, SEGMENTATION_TAG, BACKGROUND_TAG, WEIGHT_TAG,
                                    MODEL_HASH_LABEL, MODEL_SEED_LABEL, NOISE_SEED_LABEL, SCALE_LABEL,
                                    EXTNAME_LABEL)
from SHE_PPT.constants.misc import SHORT_INSTANCE_ID_MAXLEN
from SHE_PPT.file_io import read_listfile, read_xml_product, get_allowed_filename, write_xml_product, append_hdu
from SHE_PPT.mask import masked_off_image
from astropy.io import fits
import galsim
from numpy.lib.stride_tricks import as_strided

import SHE_GST
import numpy as np

from . import magic_values as mv


products.mer_segmentation_map.init()
products.she_stack_segmentation_map.init()


def combine_dithers(dithers,
                    dithering_scheme,
                    mode="SUM"):
    """
        @brief Combine the dithered images in a list, according to the specific plan for a
            given dithering scheme.

        @param dithers List of galsim Image objects of the same size/shape/dtype.
        @param dithering_scheme String representing the name of the dithering scheme
        @param mode How to combine dithers - SUM, MEAN, or MAX

        @returns Combined image
        @returns Modified output table
    """

    if mode not in ("SUM", "NOISE_SUM", "MEAN", "MAX", "BIT_OR"):
        raise ValueError("Invalid combine mode for combine_dithers: " + str(mode) + ". " +
                         "Allowed modes are SUM, NOISE_SUM, MEAN, MAX, and BIT_OR.")

    if mode == "SUM":
        combine_operation = np.sum
    elif mode == "NOISE_SUM":
        def noise_sum(a, *args, **kwargs):
            a2 = np.square(a)
            sa2 = np.sum(a2, *args, **kwargs)
            return np.sqrt(sa2)
        combine_operation = noise_sum
    elif mode == "MEAN":
        combine_operation = np.mean
    elif mode == "MAX":
        combine_operation = np.max
    elif mode == "BIT_OR":
        combine_operation = np.bitwise_or.reduce

    # Check which dithering scheme we're using
    if dithering_scheme == '2x2':

        # Check we have the right number of dithers
        num_dithers = 4
        assert len(dithers) == num_dithers

        # For this scheme, the offsets are (in x,y):
        # 0: (0.0,0.0) (Lower-left)
        # 1: (0.5,0.0) (Lower-right)
        # 2: (0.0,0.5) (Upper-left)
        # 3: (0.5,0.5) (Upper-right)

        ll_data = np.pad(dithers[0], pad_width=((1, 1), (1, 1)), mode='constant')
        lr_data = np.pad(dithers[1], pad_width=((1, 1), (1, 1)), mode='constant')
        ul_data = np.pad(dithers[2], pad_width=((1, 1), (1, 1)), mode='constant')
        ur_data = np.pad(dithers[3], pad_width=((1, 1), (1, 1)), mode='constant')

        # Initialize the combined image
        dither_shape = np.shape(ll_data)
        combined_shape = (2 * dither_shape[0] - 2, 2 * dither_shape[1] - 2)
        combined_data = np.zeros(shape=combined_shape, dtype=ll_data.dtype)

        # We'll use strides to represent each corner of the combined image
        base_strides = combined_data.strides
        dither_strides = (2 * base_strides[0], 2 * base_strides[1])

        lower_left_corners = as_strided(combined_data[:-1, :-1],
                                        shape=dither_shape,
                                        strides=dither_strides)
        lower_right_corners = as_strided(combined_data[:-1, 1:],
                                         shape=dither_shape,
                                         strides=dither_strides)
        upper_left_corners = as_strided(combined_data[1:, :-1],
                                        shape=dither_shape,
                                        strides=dither_strides)
        upper_right_corners = as_strided(combined_data[1:, 1:],
                                         shape=dither_shape,
                                         strides=dither_strides)

        # We'll combine four arrays for each corner of the dithering (remember x-y ordering swap!)
        # We use roll here to shift by 1 pixel left/down. Since it's all initially zero, we can use +=
        # to assign the values we want to it

        lower_left_corners += combine_operation((ll_data,
                                                 lr_data,
                                                 ul_data,
                                                 ur_data), axis=0)
        lower_right_corners += combine_operation((np.roll(ll_data, -1, axis=1),
                                                  lr_data,
                                                  np.roll(ul_data, -1, axis=1),
                                                  ur_data), axis=0)
        upper_left_corners += combine_operation((np.roll(ll_data, -1, axis=0),
                                                 np.roll(lr_data, -1, axis=0),
                                                 ul_data,
                                                 ur_data), axis=0)
        upper_right_corners += combine_operation((np.roll(np.roll(ll_data, -1, axis=1), -1, axis=0),
                                                  np.roll(lr_data, -1, axis=0),
                                                  np.roll(ul_data, -1, axis=1),
                                                  ur_data), axis=0)
    elif dithering_scheme == '4':

        # Check we have the right number of dithers
        num_dithers = 4
        assert len(dithers) == num_dithers

        # Initialize the combined image
        dither_shape = np.shape(dithers[0])
        combined_shape = dither_shape
        combined_data = np.zeros(shape=combined_shape, dtype=dithers[0].dtype)

        combined_data += combine_operation(dithers, axis=0)

    else:
        raise Exception("Unrecognized dithering scheme: " + dithering_scheme)

    return combined_data


def save_hdu(full_image,
             image_dithers,
             wcs,
             pixel_factor,
             data_filename,
             extname,
             workdir, ):

    hdu = fits.ImageHDU(data=full_image)
    hdu.header[MODEL_HASH_LABEL] = image_dithers[0][0].header[MODEL_HASH_LABEL]
    hdu.header[MODEL_SEED_LABEL] = image_dithers[0][0].header[MODEL_SEED_LABEL]
    hdu.header[NOISE_SEED_LABEL] = image_dithers[0][0].header[NOISE_SEED_LABEL]
    hdu.header[EXTNAME_LABEL] = extname
    hdu.header[SCALE_LABEL] = image_dithers[0][0].header[SCALE_LABEL] / pixel_factor

    wcs.writeToFitsHeader(hdu.header, galsim.Image(full_image).bounds)

    append_hdu(os.path.join(workdir, data_filename), hdu)

    return


def combine_segmentation_dithers(segmentation_listfile_name,
                                 stacked_segmentation_filename,
                                 dithering_scheme,
                                 workdir):
    if dithering_scheme == '2x2':
        pixel_factor = 2
        extra_pixels = 2
    elif dithering_scheme == '4':
        pixel_factor = 1
        extra_pixels = 0
    else:
        raise ValueError("Unknown dithering scheme: " + dithering_scheme)

    # Get the individual dithers
    segmentation_product_filenames = read_listfile(os.path.join(workdir,
                                                                segmentation_listfile_name))

    num_dithers = len(segmentation_product_filenames)

    # Get out the fits filenames and load them in memory-mapped mode
    segmentation_dithers = []
    max_len = 0
    max_x_size = 0
    max_y_size = 0

    for segmentation_product_filename in segmentation_product_filenames:

        p = read_xml_product(os.path.join(workdir, segmentation_product_filename))
        f = fits.open(os.path.join(workdir, p.get_data_filename()), memmap=True, mode="denywrite")

        if len(f) > max_len:
            max_len = len(f)

        x_size = -mv.image_gap_x_pix
        y_size = -mv.image_gap_y_pix

        for i in range(len(f)):
            shape = f[i].data.shape

            if i < 6:
                x_size += shape[0] + mv.image_gap_x_pix

            if i % 6 == 0:
                y_size += shape[1] + mv.image_gap_y_pix

        if x_size > max_x_size:
            max_x_size = x_size
        if y_size > max_y_size:
            max_y_size = y_size

        segmentation_dithers.append(f)

    # Get the WCS from the first dither
    first_wcs, first_origin = galsim.wcs.readFromFitsHeader(segmentation_dithers[0][0].header)

    stack_wcs = galsim.wcs.AffineTransform(dudx=first_wcs.dudx / pixel_factor,
                                           dudy=first_wcs.dudy / pixel_factor,
                                           dvdx=first_wcs.dvdx / pixel_factor,
                                           dvdy=first_wcs.dvdy / pixel_factor,
                                           origin=first_origin / pixel_factor)

    max_x_size = pixel_factor * max_x_size + extra_pixels
    max_y_size = pixel_factor * max_y_size + extra_pixels

    full_image = np.zeros((max_x_size, max_y_size), dtype=np.int32)

    # Loop over hdus, combining them for each dither and adding to the full image
    x_offset = 0
    y_offset = 0
    for x in range(max_len):
        dithers = []
        for i in range(num_dithers):
            if x < len(segmentation_dithers[i]):
                dithers.append(segmentation_dithers[i][x].data)

        detector_stack = combine_dithers(dithers,
                                         dithering_scheme,
                                         mode="MAX")

        full_image[x_offset:x_offset + detector_stack.shape[0],
                   y_offset:y_offset + detector_stack.shape[1]] += detector_stack

        if x % 6 != 5:
            x_offset += detector_stack.shape[0] + pixel_factor * mv.image_gap_x_pix - extra_pixels
        else:
            x_offset = 0
            y_offset += detector_stack.shape[1] + pixel_factor * mv.image_gap_y_pix - extra_pixels

    # Print out the stacked segmentation map
    model_hash_fn = segmentation_dithers[0][0].header[MODEL_HASH_LABEL][0:SHORT_INSTANCE_ID_MAXLEN]
    model_hash_fn = model_hash_fn.replace('.', '-').replace('+', '-')
    data_filename = get_allowed_filename("GST-SEG-STACK",
                                         model_hash_fn,
                                         extension=".fits",
                                         version=SHE_GST.__version__)

    save_hdu(full_image, segmentation_dithers, stack_wcs, pixel_factor,
             data_filename, SEGMENTATION_TAG, workdir)

    p = products.she_stack_segmentation_map.create_dpd_she_stack_segmentation_map(data_filename)
    write_xml_product(p, stacked_segmentation_filename, workdir=workdir)

    return


def combine_image_dithers(image_listfile_name,
                          stacked_image_filename,
                          dithering_scheme,
                          workdir):

    if dithering_scheme == '2x2':
        pixel_factor = 2
        extra_pixels = 2
    elif dithering_scheme == '4':
        pixel_factor = 1
        extra_pixels = 0
    else:
        raise ValueError("Unknown dithering scheme: " + dithering_scheme)

    # Get the individual dithers
    image_product_filenames = read_listfile(os.path.join(workdir, image_listfile_name))

    num_dithers = len(image_product_filenames)

    # Get out the fits filenames and load them in memory-mapped mode
    image_dithers = []
    bkg_image_dithers = []
    wgt_image_dithers = []
    max_len = 0
    max_x_size = 0
    max_y_size = 0
    max_num_x = 0
    max_num_y = 0

    for image_product_filename in image_product_filenames:

        p = read_xml_product(os.path.join(workdir, image_product_filename))
        f = fits.open(os.path.join(workdir, p.get_data_filename()), memmap=True, mode="denywrite")
        fb = fits.open(os.path.join(workdir, p.get_bkg_filename()), memmap=True, mode="denywrite")
        fw = fits.open(os.path.join(workdir, p.get_wgt_filename()), memmap=True, mode="denywrite")

        if len(f) > max_len:
            max_len = len(f)

        x_size = -mv.image_gap_x_pix
        y_size = -mv.image_gap_y_pix
        num_x = 0
        num_y = 0

        for i in range(len(f) // 3):

            assert f[3 * i].header[EXTNAME_LABEL][-4:] == "." + SCI_TAG
            assert fb[i].header[EXTNAME_LABEL] == f[3 * i].header[EXTNAME_LABEL][:-4]
            assert fw[i].header[EXTNAME_LABEL] == f[3 * i].header[EXTNAME_LABEL][:-4]

            shape = f[3 * i].data.shape

            if i < 6:
                x_size += shape[0] + mv.image_gap_x_pix
                num_x += 1

            if i % 6 == 0:
                y_size += shape[1] + mv.image_gap_y_pix
                num_y += 1

        if x_size > max_x_size:
            max_x_size = x_size
        if y_size > max_y_size:
            max_y_size = y_size
        if num_x > max_num_x:
            max_num_x = num_x
        if num_y > max_num_y:
            max_num_y = num_y

        image_dithers.append(f)
        bkg_image_dithers.append(fb)
        wgt_image_dithers.append(fw)

    # Get the WCS from the first dither
    first_wcs, first_origin = galsim.wcs.readFromFitsHeader(image_dithers[0][0].header)

    stack_wcs = galsim.wcs.AffineTransform(dudx=first_wcs.dudx / pixel_factor,
                                           dudy=first_wcs.dudy / pixel_factor,
                                           dvdx=first_wcs.dvdx / pixel_factor,
                                           dvdy=first_wcs.dvdy / pixel_factor,
                                           origin=first_origin / pixel_factor)

    # Create the image we'll need

    max_x_size = pixel_factor * max_x_size + extra_pixels
    max_y_size = pixel_factor * max_y_size + extra_pixels

    full_sci_image = np.zeros((max_x_size, max_y_size), dtype=np.float32)
    full_flg_image = np.ones((max_x_size, max_y_size), dtype=np.int32) * masked_off_image
    full_rms_image = np.zeros((max_x_size, max_y_size), dtype=np.float32)
    full_bkg_image = np.zeros((max_x_size, max_y_size), dtype=np.float32)
    full_wgt_image = np.zeros((max_x_size, max_y_size), dtype=np.float32)

    # Loop over hdus, combining them for each dither and adding to the full image
    x_offset = 0
    y_offset = 0
    for x in range(max_len // 3):
        sci_dithers = []
        flg_dithers = []
        rms_dithers = []
        bkg_dithers = []
        wgt_dithers = []
        for i in range(num_dithers):
            if 3 * x + 2 < len(image_dithers[i]):

                assert image_dithers[i][3 * x].header[EXTNAME_LABEL][-4:] == "." + SCI_TAG
                sci_dithers.append(image_dithers[i][3 * x].data)

                assert image_dithers[i][3 * x + 1].header[EXTNAME_LABEL][-4:] == "." + NOISEMAP_TAG
                rms_dithers.append(image_dithers[i][3 * x + 1].data)

                assert image_dithers[i][3 * x + 2].header[EXTNAME_LABEL][-4:] == "." + MASK_TAG
                flg_dithers.append(image_dithers[i][3 * x + 2].data)

                bkg_dithers.append(bkg_image_dithers[i][x].data)
                wgt_dithers.append(wgt_image_dithers[i][x].data)

        sci_stack = combine_dithers(sci_dithers,
                                    dithering_scheme,
                                    mode="SUM")

        flg_stack = combine_dithers(flg_dithers,
                                    dithering_scheme,
                                    mode="BIT_OR")

        rms_stack = combine_dithers(rms_dithers,
                                    dithering_scheme,
                                    mode="NOISE_SUM")

        bkg_stack = combine_dithers(bkg_dithers,
                                    dithering_scheme,
                                    mode="SUM")

        wgt_stack = combine_dithers(wgt_dithers,
                                    dithering_scheme,
                                    mode="SUM")

        full_sci_image[x_offset:x_offset + sci_stack.shape[0],
                       y_offset:y_offset + sci_stack.shape[1]] += sci_stack
        full_flg_image[x_offset:x_offset + flg_stack.shape[0],
                       y_offset:y_offset + flg_stack.shape[1]] += flg_stack - masked_off_image
        full_rms_image[x_offset:x_offset + rms_stack.shape[0],
                       y_offset:y_offset + rms_stack.shape[1]] += rms_stack
        full_bkg_image[x_offset:x_offset + bkg_stack.shape[0],
                       y_offset:y_offset + bkg_stack.shape[1]] += bkg_stack
        full_wgt_image[x_offset:x_offset + wgt_stack.shape[0],
                       y_offset:y_offset + wgt_stack.shape[1]] += wgt_stack

        if x % 6 != 5:
            x_offset += sci_stack.shape[0] + pixel_factor * mv.image_gap_x_pix - extra_pixels
        else:
            x_offset = 0
            y_offset += sci_stack.shape[1] + pixel_factor * mv.image_gap_y_pix - extra_pixels

    # Subtract the background from the science image
    full_sci_image -= full_bkg_image

    # Print out the stacked segmentation map
    model_hash_fn = image_dithers[0][0].header[MODEL_HASH_LABEL][0:SHORT_INSTANCE_ID_MAXLEN]
    model_hash_fn = model_hash_fn.replace('.', '-').replace('+', '-')
    data_filename = get_allowed_filename("GST-IMAGE-STACK",
                                         model_hash_fn,
                                         extension=".fits",
                                         version=SHE_GST.__version__)
    save_hdu(full_sci_image, image_dithers, stack_wcs, pixel_factor,
             data_filename, SCI_TAG, workdir)
    save_hdu(full_flg_image, image_dithers, stack_wcs, pixel_factor,
             data_filename, MASK_TAG, workdir)
    save_hdu(full_rms_image, image_dithers, stack_wcs, pixel_factor,
             data_filename, NOISEMAP_TAG, workdir)

    bkg_filename = get_allowed_filename("GST-BKG-STACK",
                                        model_hash_fn,
                                        extension=".fits",
                                        version=SHE_GST.__version__)
    save_hdu(full_bkg_image, bkg_image_dithers, stack_wcs, pixel_factor,
             bkg_filename, BACKGROUND_TAG, workdir)

    wgt_filename = get_allowed_filename("GST-WGT-STACK",
                                        model_hash_fn,
                                        extension=".fits",
                                        version=SHE_GST.__version__)
    save_hdu(full_bkg_image, wgt_image_dithers, stack_wcs, pixel_factor,
             wgt_filename, WEIGHT_TAG, workdir)

    p = products.vis_stacked_frame.create_dpd_vis_stacked_frame(data_filename=data_filename,
                                                                bkg_filename=bkg_filename,
                                                                wgt_filename=wgt_filename)
    write_xml_product(p, stacked_image_filename, workdir=workdir)

    return
