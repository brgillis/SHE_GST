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

``input_port_name``:

**Description:**

**Source:**

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

``output_port_name``:

**Description:**

**Details:**


Example
-------

Prepare the required input data in the desired workdir. This will require ...

The program can then be run with the following command in an EDEN 2.1 environment:

.. code:: bash

    E-Run SHE_GST 8.2 SHE_GST_GenGalaxyImages --workdir $WORKDIR [--log-file <filename>] [--log-level <value>] [--pipeline_config <filename>]

where the variable ``$WORKDIR`` corresponds to the path to your workdir and the variables  ... correspond to the filenames of the prepared listfiles and downloaded products for each input port.

This command will generate a new data product with the filename ...
