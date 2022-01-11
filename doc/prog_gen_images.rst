SHE_GST_GenGalaxyImages
=======================

Executable to generate mock galaxy images and associated data, which can be used as test data, for bias/sensitivity measurement, etc.

In order to ensure that shear estimation methods meet the requirements on bias, it is necessary to test and calibrate them against a large number of simulated galaxy images. OU-SIM is tasked with providing these images, but it is not certain that the volume they will be able to provide will be sufficient to guarantee that requirements on shear estimation methods will be met. It is thus necessary for OU-SHE to generate its own simulations, using a simplified methodology specialised for galaxies and omitting the simulation of and correction for certain time-consuming effects such as CTI. The shear estimation methods will be tested on the available images from OU-SIM to ensure consistency, and then tested and calibrated on a larger set of images from this PE if it is determined to be necessary.

This executable is intended to be a replacement for PF-SIM within the pipeline when needed and when it can be demonstrated to provide a suitably-accurate alternative.


Running the Program on EDEN/LODEEN
----------------------------------

To run the ``SHE_GST_GenGalaxyImages`` program with Elements, use the following command in an EDEN 2.1 environment:

.. code:: bash

    E-Run SHE_GST 8.2 SHE_GST_GenGalaxyImages --workdir <dir> --pipeline_config <filename> --config_files <filename1> [<filename2> ...] --data_images <filename> --detections_tables <filename> --details_table <filename> --psf_images_and_tables <filename> --segmentation_images <filename> --stacked_data_image <filename> --stacked_segmentation_image <filename> [--log-file <filename>] [--log-level <value>]

**Note:** Due to the unusually large number of arguments available for this executable, the command syntax provided here is limited to the most relevant options.

This allows the following arguments:

Common Elements Arguments
~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :widths: 15 50 10 25
   :header-rows: 1

   * - Argument
     - Description
     - Required
     - Default
   * - --workdir ``<path>``
     - Name of the working directory, where input data is stored and output data will be created.
     - yes
     - N/A
   * - --log-file ``<filename>``
     - Name of a filename to store logs in, relative to the workdir. If not provided, logging data will only be output to the terminal. Note that this will only contain logs directly from the run of this executable. Logs of executables called during the pipeline execution will be stored in the "logs" directory of the workdir.
     - no
     - None
   * - --logdir ``<path>``
     - Path where logging data will be stored. This only has effect if some other option is enabled which produces logging data, such as ``--profile``.
     - no
     - ``"."``
   * - --log-level ``<level>``
     - Minimum severity level at which to print logging information. Valid values are DEBUG, INFO, WARNING, and ERROR. Note that this will only contain logs directly from the run of this executable. The log level of executables called during pipeline execut will be set based on the configuration of the pipeline server (normally INFO).
     - no
     - INFO


Input Arguments
~~~~~~~~~~~~~~~

.. list-table::
   :widths: 15 50 10 25
   :header-rows: 1

   * - Argument
     - Description
     - Required
     - Default
   * - ``--config_files <filename1> [<filename2> ...]``
     - One or more ``.xml`` data product of type ``DpdSheSimulationConfig``, specifying configuration options for this executable. If multiple configuration files are provided, they are processed in order, with options specified in multiple files using the value from the last file to specify it. If any options are also specified at the command-line, the command-line value takes precedence.
     - no
     - Individual defaults used for all parameters, unless specified at the command-line
   * - ``--pipeline_config <filename>``
     - ``.xml`` data product or pointing to configuration file (described below), or .json listfile (Cardinality 0-1) either pointing to such a data product, or empty.
     - no
     - None (equivalent to providing an empty listfile)


Output Arguments
~~~~~~~~~~~~~~~~

.. list-table::
   :widths: 15 50 10 25
   :header-rows: 1

   * - Argument
     - Description
     - Required
     - Default
   * - data_images
     - Desired filename of ``.json`` listfile pointing to ``.xml`` data products of type `DpdVisCalibratedFrame <https://euclid.esac.esa.int/dm/dpdd/latest/visdpd/dpcards/vis_calibratedframe.html>`__, which will contain simulated VIS science images for each exposure in an observation.
     - yes
     - N/A
   * - detections_tables
     - Desired filename of ``.json`` listfile pointing to ``.xml`` data product of type `DpdMerFinalCatalog <https://euclid.esac.esa.int/dm/dpdd/latest/merdpd/dpcards/mer_finalcatalog.html>`__, containing a catalog of all simulated objects in the images, in the format of the detections catalog that will normally be provided by PF-MER.
     - yes
     - N/A
   * - details_table
     - Desired filename of ``.xml`` data product of type ``DpdSheSimulatedCatalog``, containing a catalog of all objects in the observation with true input data.
     - yes
     - N/A
   * - psf_images_and_tables
     - Desired filename of ``.xml`` data product of type `DpdShePsfModelImage <https://euclid.esac.esa.int/dm/dpdd/latest/shedpd/dpcards/she_psfmodelimage.html>`__, containing PSF images for each simulated galaxy, in the format normally provided by the ``SHE_PSFToolkit_ModelPSFs`` task.
     - yes
     - N/A
   * - segmentation_images
     - Desired filename of ``.json`` listfile pointing to ``.xml`` data product of type `DpdSheExposureReprojectedSegmentationMap <https://euclid.esac.esa.int/dm/dpdd/latest/shedpd/dpcards/she_exposurereprojectedsegmentationmap.html>`__, containing segmentation maps for the simulated area, in the format of the segmentation maps that will normally be generated within PF-SHE by reprojecting PF-MER's segmentation maps to match the VIS image frames.
     - yes
     - N/A
   * - stacked_data_image
     - Desired filename of ``.xml`` data product of type `DpdVisStackedFrame <https://euclid.esac.esa.int/dm/dpdd/latest/visdpd/dpcards/vis_visstackedframe.html>`__, containing simulated VIS stacked science image.
     - yes
     - N/A
   * - stacked_segmentation_image
     - Desired filename of ``.xml`` data product of type `DpdSheStackReprojectedSegmentationMap <https://euclid.esac.esa.int/dm/dpdd/latest/shedpd/dpcards/she_stackreprojectedsegmentationmap.html>`__, containing segmentation map for the simulated area, in the format of the segmentation map that will normally be generated within PF-SHE by reprojecting PF-MER's segmentation maps to match the VIS stacked image frame.
     - yes
     - N/A

Options
~~~~~~~

**Note:** This executable allows a very large number of optional arguments which can be set at the command line. As these are normally expected to be set in the provided configuration file, they are documented in that section of this page. Any of these options can also be provided at the command-line with the same name (and value in quotes if it contains spaces), and if so, the value provided at the command-line will take precedence.

.. list-table::
   :widths: 15 50 10 25
   :header-rows: 1

   * - Argument
     - Description
     - Required
     - Default
   * - ``--profile`` (``store_true``)
     - If set, Python code will be profiled, and the resulting profiling data will be output to a file in the directory specified with ``--logdir``.
     - no
     - False


Inputs
------

.. _config_files:

``config_files``:

**Description:** One or more ``.xml`` data products of type ``DpdSheSimulationConfig``, specifying configuration options for this executable. If multiple configuration files are provided, they are processed in order, with options specified in multiple files using the value from the last file to specify it. If any options are also specified at the command-line, the command-line value takes precedence. These ``.xml`` data products each point to a ``.txt`` textfile which contains the configuration options. For use outside of a pipeline, the name(s) of these text files may be supplied instead, without need for an ``.xml`` data product.

The ``.txt`` configuration file contains one option per line (blank lines are allowed, as are comment lines which start with "#"), each with the format ``option = value`` or ``long_option = 'string value'`` in the case of values provided as strings. Below is an example of the contents of such a file, containing most common options:

::

    # Adjustment for the random seeding. Set this to a different value each time you run
    # the script to get entirely different images each time.
    seed =   1

    # Set to False and error will be applied as normal
    # Set to True and no noise (not even Poisson) will be present on the images
    suppress_noise = False

    # Maximum magnitude allowed for target galaxies
    magnitude_limit = 24.5

    # Number of target galaxies to render in the generated image
    num_target_galaxies = 16

    # Whether or not to render background galaxies
    render_background_galaxies = False

    # "mode" is one of "stamps" (to make postage stamps), "field" (to render a simulated field of galaxies), or "cutouts" (to render a field, then output cutout stamps from it)
    mode = stamps

    # The output size of postage stamps (for "stamps" and "field" modes)
    stamp_size = 128

    # The size of the generated image in pixels (for "field" and "cutouts" modes)
    image_size_xp = 4096
    image_size_yp = 4132

    # Parameters which affect background noise

    read_noise = 5.4 # e-/pixel

    # If you wish for the sky background to be included in the image, set it with unsubtracted_background_setting
    # Otherwise, use subtracted_background_setting and it will be be subtracted off, but the Poisson noise from it will remain
    subtracted_background_setting = 'Fixed 4571.' # ADU/arcsec
    unsubtracted_background_setting = 'Fixed 0.' # ADU/arcsec

    # Galaxy model settings

    bulge_axis_ratio_setting = 'Fixed 0.6'
    disk_height_ratio_setting = 'Fixed 0.1'
    sersic_index_setting = 'Fixed 4.0'

    disk_truncation_factor_setting = 'Fixed 4.5'

    # Galaxy model generation levels - these configure whether values are set once per galaxy, once per pair of galaxies (for shape-noise cancellation), once per group
    # of galaxies (also for shape-noise cancellation), or once per image

    apparent_mag_vis_level = pair
    apparent_size_bulge_level = pair
    apparent_size_disk_level = pair
    bulge_fraction_level = pair
    bulge_ellipticity_level = pair
    rotation_level = galaxy
    sersic_index_level = pair
    shear_angle_level = galaxy_group
    shear_magnitude_level = galaxy_group
    spin_level = pair
    tilt_level = pair

    # Settings for the psf used

    # The base of the filename for the output PSF, its scale factor, and its stamp size in subsampled pixels
    psf_file_name_base = sensitivity_testing_sample_psf
    psf_scale_factor = 5
    psf_stamp_size = 256

    # Use a specific PSF file as the PSF in the images
    chromatic_psf = False
    model_psf_file_name = /home/user/git/she_sim_galaxy_generation/SHE_SIM_galaxy_image_generation/auxdir/SHE_SIM_galaxy_image_generation/psf_models/el_cb2004a_001.fits_0.000_0.804_0.00.fits
    model_psf_scale = 0.02

    # The offset of the PSF's centre from the centre of the FITS image
    model_psf_x_offset = -0.5
    model_psf_y_offset = -2.5

    suppress_noise = False # Set to true and no noise (neither read noise nor Poisson noise) will be rendered on the image

    # Shape noise cancellation - if enabled, will pair up galaxies and use identical paramters for each galaxy in the pair, except for rotation which will be offset by
    # 90 degrees.
    shape_noise_cancellation = True

    # Stable RNG - Will use a Gaussian approximation for Poisson noise, which is beneficial for sensitivity testing as its stable to small changes in the image
    # That is, it always varies smoothly in response to smooth changes in the image.
    stable_rng = True

**Source:** May be generated manually for a desired simulation, or generated as output of the ``SHE_GST_PrepareConfigs`` executable either manually or as part of a pipeline run. See `that task's documentation <prog_prepare_configs.html>`__ for details on how it can be used to generate config files, or else one may be written manually.

If generating a configuration file manually, generally only the actuall ``.txt`` configuration file is needed, and not the ``.xml`` data product. This can be written with your text editor of choice (e.g. ``gedit``). It is generally easiest to start with an existing file and modifying it as desired. Many such files are provided in this project in the directory ``SHE_GST/SHE_GST_GalaxyImageGeneration/conf/SHE_GST_GalaxyImageGeneration`` which can be used as a basis for a new configuration file.

``pipeline_config``:

**Description:** One of the following:

1. The word "None" (without quotes), which signals that default values
   for all configuration parameters shall be used.
2. The filename of an empty ``.json`` listfile, which similarly
   indicates the use of all default values.
3. The filename of a ``.txt`` file in the workdir listing configuration
   parameters and values for executables in the current pipeline run.
   This shall have the one or more lines, each with the format
   "SHE\_MyProject\_config\_parameter = config\_value".
4. The filename of a ``.xml`` data product of format
   DpdSheAnalysisConfig, pointing to a text file as described above. The
   format of this data product is described in detail in the Euclid DPDD
   at
   https://euclid.esac.esa.int/dm/dpdd/latest/shedpd/dpcards/she\_analysisconfig.html.
5. The filename of a ``.json`` listfile which contains the filename of a
   ``.xml`` data product as described above.

Any of the latter three options may be used for equivalent
functionality.

The ``.txt`` pipeline configuration file may have any number of
configuration arguments which apply to other executables, in addition to
optionally any of the following which apply to this executable:

.. list-table::
   :widths: 20 50 30
   :header-rows: 1

   * - Option
     - Description
     - Default Behaviour
   * - SHE_Pipeline_profile
     - If set to "True", Python code will be profiled, and the resulting profiling data will be output to a file in the directory specified with ``--logdir``.
     - Profiling will not be enabled

If both these arguments are supplied in the pipeline configuration file
and the equivalent command-line arguments are set, the command-line
arguments will take precedence.

**Source:** One of the following:

1. May be generated manually, creating the ``.txt`` file with your text
   editor of choice.
2. Retrieved from the EAS, querying for a desired product of type
   DpdSheAnalysisConfig.
3. If run as part of a pipeline triggered by the
   `SHE_Pipeline_Run <https://gitlab.euclid-sgs.uk/PF-SHE/SHE_IAL_Pipelines>`__
   helper program, may be created automatically by providing the argument
   ``--config_args ...`` to it (see documentation of that executable for
   further information).


Outputs
-------

``data_images``:

**Description:** Desired filename of ``.json`` listfile pointing to ``.xml`` data products of type `DpdVisCalibratedFrame <https://euclid.esac.esa.int/dm/dpdd/latest/visdpd/dpcards/vis_calibratedframe.html>`__, which will contain simulated VIS science images for each exposure in an observation.

**Details:** The generated products are a simulated versions of the ``DpdVisCalibratedFrame`` product, with the following notable differences:

#. Only one CCD image is generated (1-1). This image is not necessarily the same size as the normal VIS CCD images
#. Galaxies are normally rendered onto individual postage stamps within the image, unless requested otherwise through setting the ``mode`` option to "field"
#. The dithering scheme is greatly simplified. By default, exposures are dithered by a half pixel in the x and y directions (for 4 total positions)

``detections_tables``:

**Description:** Desired filename of ``.json`` listfile pointing to ``.xml`` data product of type `DpdMerFinalCatalog <https://euclid.esac.esa.int/dm/dpdd/latest/merdpd/dpcards/mer_finalcatalog.html>`__, containing a catalog of all simulated objects in the images, in the format of the detections catalog that will normally be provided by PF-MER.

**Details:** The generated product is a simulated version of the ``DpdMerFinalCatalog`` product. In the present implementation of this executable, it includes no actual mock detections step, and so all target galaxies are included in this product. This is expected to change in the future to include a proper detections step.

``details_table``:

**Description:** Desired filename of ``.xml`` data product of type ``DpdSheSimulatedCatalog``, containing a catalog of all objects in the observation with true input data.

**Details:** This catalog contains the following columns, with entries for each target galaxy:

.. list-table::
   :widths: 20 20 60
   :header-rows: 1

   * - Column Name
     - Data Type
     - Description
   * - OBJECT_ID
     - 64-bit int
     - Unique ID for each object, linking to the table in the simulated ``DpdMerFinalCatalog`` product
   * - GROUP_ID
     - 64-bit int
     - ID of the galaxy group (used for shape-noise cancellation) for this object
   * - RIGHT_ASCENSION
     - 32-bit float
     - J2000 right ascension position of the object in degrees
   * - DECLINATION
     - 32-bit float
     - J2000 declination position of the object in degrees
   * - HLR_BULGE
     - 32-bit float
     - Half-light radius of the object's bulge, in arcseconds
   * - HLR_DISK
     - 32-bit float
     - Half-light radius of the object's disk, in arcseconds
   * - BULGE_ELLIPTICITY
     - 32-bit float
     - Magnitude of the object's bulge component's ellipticity, defined as (a-b)/(a+b), where a is the major axis length and b the minor axis length
   * - BULGE_AXIS_RATIO
     - 32-bit float
     - Ratio b/a for the bulge, where a is the major axis length and b the minor axis length
   * - BULGE_FRACTION
     - 32-bit float
     - The fraction of the total flux of the object contributed by the bulge component
   * - DISK_HEIGHT_RATIO
     - 32-bit float
     - The ratio of the scale height of the object's disk component to its scale radius
   * - REDSHIFT
     - 32-bit float
     - The simulated redshift of the object
   * - MAGNITUDE
     - 32-bit float
     - The apparent magnitude of the object in the Euclid VIS filter
   * - SNR
     - 32-bit float
     - The signal-to-noise ratio of the object
   * - SERSIC_INDEX
     - 32-bit float
     - The sersic index of the object's bulge component
   * - ROTATION
     - 32-bit float
     - The position angle of the object's major axis in degrees north of west on the sky
   * - SPIN
     - 32-bit float
     - For non-axisymmetric objects (not yet implemented), the rotation in degrees of the object's 3D model around its minor axis
   * - TILT
     - 32-bit float
     - The inclination of the object relative to the line-of-sight in degrees, where ``0 deg.`` is face-on and ``90 deg.`` is fully-inclined
   * - G1_WORLD
     - 32-bit float
     - The true value of the lensing shear applied to the object's image, component 1 (in the (-R.A., Dec) frame of reference).
   * - G2_WORLD
     - 32-bit float
     - The true value of the lensing shear applied to the object's image, component 2 (in the (-R.A., Dec) frame of reference).
   * - is_target_galaxy
     - bool
     - Whether or not the object is fully rendered as a target galaxy, or rendered more simply as a background galaxy. Depending on configuration, all background galaxies may be excluded from this table

``psf_images_and_tables``:

**Description:** Desired filename of ``.xml`` data product of type `DpdShePsfModelImage <https://euclid.esac.esa.int/dm/dpdd/latest/shedpd/dpcards/she_psfmodelimage.html>`__, containing PSF images for each simulated galaxy, in the format normally provided by the ``SHE_PSFToolkit_ModelPSFs`` task.

**Details:** The generated product is a simulated version of the ``DpdShePsfModelImage`` data product. By default, this contains the true PSFs used for rendering simulated galaxy images. If the same PSF is used for all galaxies, this product will save space by only storing it once and pointing to it for all galaxies.

For purposes of Sensitivity Testing, it is possible to instruct the executable to output PSFs to this product other than those used to render galaxies, through use of the ``output_psf_file_name`` option. If the filename of a ``.fits``-format PSF image is provided to this option, this PSF will be supplied as output to this product, while not affecting the PSF used for rendering simulated galaxy images.

``segmentation_images``:

**Description:** Desired filename of ``.json`` listfile pointing to ``.xml`` data product of type `DpdSheExposureReprojectedSegmentationMap <https://euclid.esac.esa.int/dm/dpdd/latest/shedpd/dpcards/she_exposurereprojectedsegmentationmap.html>`__, containing segmentation maps for the simulated area, in the format of the segmentation maps that will normally be generated within PF-SHE by reprojecting PF-MER's segmentation maps to match the VIS image frames.

**Details:** The generated products are a simulated versions of the ``DpdSheExposureReprojectedSegmentationMap`` products. These segmentation maps are generated through a rudimentary approach, where a threshold pixel flux value is used to determine if a pixel is assigned to a given object. In the case of pixels which meet this threshold for multiple objects, they're assigned to the brightest object. This is known to produce unrealistic maps for blends, and so will need to be improved if these maps are relied on for blends in any way.

``stacked_data_image``:

**Description:** Desired filename of ``.xml`` data product of type `DpdVisStackedFrame <https://euclid.esac.esa.int/dm/dpdd/latest/visdpd/dpcards/vis_visstackedframe.html>`__, containing simulated VIS stacked science image.

**Details:** See details for the ``data_images`` output product above; the same notes apply.

``stacked_segmentation_image``:

**Description:** Desired filename of ``.xml`` data product of type `DpdSheStackReprojectedSegmentationMap <https://euclid.esac.esa.int/dm/dpdd/latest/shedpd/dpcards/she_stackreprojectedsegmentationmap.html>`__, containing segmentation map for the simulated area, in the format of the segmentation map that will normally be generated within PF-SHE by reprojecting PF-MER's segmentation maps to match the VIS stacked image frame.

**Details:** See details for the ``segmentation_images`` output product above; the same notes apply.


Example
-------

Prepare a configuration file for this run, for instance by copying `the example contents above <config_files_>`_ into a textfile.

.. code:: bash

    E-Run SHE_GST 8.2 SHE_GST_GenGalaxyImages --workdir $WORKDIR --config_files simulation_config.txt --data_images vis_calibrated_frames_listfile.json --detections_tables mer_final_catalog_listfile.json --details_table she_simulated_catalog_listfile.json --psf_images_and_tables she_model_psf_listfile.json --segmentation_images she_reprojected_exposure_segmentation_map_listfile.json --stacked_data_image vis_stacked_frame_product.xml --stacked_segmentation_image she_reprojected_stack_segmentation_map_product.xml

where the variable ``$WORKDIR`` corresponds to the path to your workdir and the variable $CONFIG_FILE corresponds to the filename of the prepared configuration file.

This command will generate a new data product with the filename ...
