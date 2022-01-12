SHE_GST_PrepareConfigs
======================

Generates a set of simulation config files from a template and plan, which can be passed to the galaxy image generation function. This executable is used as the within the SHE Shear Calibration pipeline to create a listfile of multiple simulation config files which the IAL pipeline runner can perform a parallel split over, running separate analysis with a simulated image generated from each. The separate simulation config files are generally differentiated only through different RNG seed values in each.


Running the Program on EDEN/LODEEN
----------------------------------

To run the ``SHE_GST_PrepareConfigs`` program with Elements, use the following command in an EDEN 2.1 environment:

.. code:: bash

    E-Run SHE_GST 8.2 SHE_GST_PrepareConfigs --workdir <dir> [--log-file <filename>] [--log-level <value>] [--pipeline_config <filename>]

with the following arguments:


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
   * - simulation_plan
     - .xml data product of type ``DpdSheSimulationPlan``, containing a table detailing how the simulation is to be carried out, listing parameters to be varied for each simulation and how to vary them
     - yes
     - N/A
   * - config_template
     - .xml data product of type ``DpdSheSimulationConfig``, containing template simulation config textfile for image simulations, which specifies all configuration parameters which are constant through all simulations and those which are varied
     - yes
     - N/A
   * - ``--pipeline_config <filename>``
     - ``.xml`` data product or pointing to configuration file (described below), or ``.json`` listfile (Cardinality 0-1) either pointing to such a data product, or empty.
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
   * - simulation_configs
     - Desired filename of output ``.json`` listfile, which will contain filenames of generated simulation config data products
     - yes
     - N/A

Options
~~~~~~~

.. list-table::
   :widths: 15 50 10 25
   :header-rows: 1

   * - Argument
     - Description
     - Required
     - Default
   * -
     -
     -
     -


Inputs
------

.. _simulation_plan:

``simulation_plan``:

**Description:** ``.xml`` data product of type ``DpdSheIntermediateGeneral`` (temporarily, until ``DpdSheSimulationPlan`` is defined for it) pointing to a data table (either ``.txt`` or ``.fits``). This table specifies how simulation parameters are to be varied over different simulation runs within the pipeline run. This is done by generating an array of parameters for each row of the table, and substituting these into the ``config_template`` to generate a full simulation configuration product for each set of parameters, which fully specifies how to generate a single simulation. Normally only one row is needed in the table, but it is possible to include multiple rows, which will result in multiple arrays of parameters being generated and simulation configuration products generated for each element of each row's array.

The columns in the table are:

.. list-table::
   :widths: 20 20 60
   :header-rows: 1

   * - Column Name
     - Data Type
     - Description
   * - ``MSEED_MIN``
     - 64-bit int
     - Minimum value of RNG seed used for the generation of galaxy models
   * - ``MSEED_MAX``
     - 64-bit int
     - Maximum value of RNG seed used for the generation of galaxy models
   * - ``MSEED_STEP``
     - 64-bit int
     - Step size for RNG seed used for the generation of galaxy models
   * - ``NSEED_MIN``
     - 64-bit int
     - Minimum value of RNG seed used for the realization of pixel noise on the image
   * - ``NSEED_MAX``
     - 64-bit int
     - Maximum value of RNG seed used for the realization of pixel noise on the image
   * - ``NSEED_STEP``
     - 64-bit int
     - Step size for RNG seed used for the realization of pixel noise on the image
   * - ``SUP_NOISE``
     - bool
     - If True, will suppress pixel noise in the simulations. If False, pixel noise will be rendered as normal
   * - ``NUM_DETECTORS``
     - 16-bit int
     - Number of separate detector images to render in each simulation
   * - ``NUM_GALAXIES``
     - 16-bit int
     - Number of target galaxies to render per detector image in each simulation
   * - ``RENDER_BKG``
     - bool
     - If True, will suppress pixel noise in the simulations. If False, pixel noise will be rendered as normal

The number of separate simulations to be run is determined through ``MSEED_MIN``. ``MSEED_MAX``, ``MSEED_STEP``, ``NSEED_MIN``. ``NSEED_MAX``, and ``NSEED_STEP``. An array of seed values is created for each of these, starting with the minimum and incrementing it by the step until it equals the maximum (inclusive). For instance, if ``MSEED_MIN = 0``, ``MSEED_MAX = 10``, and ``MSEED_STEP = 2``, and array of 6 values will be generated: ``[0, 2, 4, 6, 8, 10]``.

The lengths of the arrays generated for ``MSEED`` and ``NSEED`` must either be equal, or else one should have length 1. In the former case, the seeds will vary alongside each other. In the latter case, the array of length 1 is interpreted as a constant value for all simulations. So, following the above example, if we also had ``NSEED_MIN = 1``, ``NSEED_MAX = 1``, and ``NSEED_STEP = 0``, an ``NSEED`` value of ``1`` would be used for all 6 simulations of varying ``MSEED``.

The length of the seed arrays are the determinant for how many simulations are performed by this pipeline.

**Source:** Generated manually, or generated through the ``SHE_Pipeline_Run`` script through the use of the ``--plan_args`` argument when running the SHE Shear Calibration pipeline.

``config_template``:

**Description:** A ``.txt`` template configuration file, used to specify configuration parameters which are constant throughout the pipeline run. See the documentation for `SHE_GST_GenGalaxyImages <prog_gen_images.html#inputs>`__ for details on the normal format of a simulation configuration file. This template file differs in that in place of some values, it contains special tags such as ``$REPLACEME_NUM_GALAXIES``. When ``SHE_GST_PrepareConfigs`` is run, it creates modified versions of this template with each of these tags replaced with values determined from the simulation plan.

The tags and the column names in the simulation plan they correspond to are:

.. list-table::
   :widths: 40 60
   :header-rows: 1

   * - Tag Name
     - Column Name(s)
   * - ``$REPLACEME_SEED``
     - ``MSEED_MIN``, ``MSEED_MAX``, ``MSEED_STEP``
   * - ``$REPLACEME_NOISESEED``
     - ``NSEED_MIN``, ``NSEED_MAX``, ``NSEED_STEP``
   * - ``$REPLACEME_SUPPRESSNOISE``
     - ``SUP_NOISE``
   * - ``$REPLACEME_NUMDETECTORS``
     - ``NUM_DETECTORS``
   * - ``$REPLACEME_NUMGALAXIES``
     - ``NUM_GALAXIES``
   * - ``$REPLACEME_RENDERBKG``
     - ``RENDER_BKG``

See the documentation for the `simulation_plan <simulation_plan_>`_ input port above for details on the meanings and use of these values.

**Source:** Generated manually within OU-SHE. Sample templates are stored in the folder ``SHE_GST_PrepareConfigs/auxdir/SHE_GST_PrepareConfigs`` of this project, which can either be used unmodified or copied and modified.

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

    E-Run SHE_GST 8.2 SHE_GST_PrepareConfigs --workdir $WORKDIR [--log-file <filename>] [--log-level <value>] [--pipeline_config <filename>]

where the variable ``$WORKDIR`` corresponds to the path to your workdir and the variables  ... correspond to the filenames of the prepared listfiles and downloaded products for each input port.

This command will generate a new data product with the filename ...
