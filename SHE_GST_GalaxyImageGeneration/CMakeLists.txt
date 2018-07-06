# Declare this directory as an Elements module
elements_subdir(SHE_GST_GalaxyImageGeneration)

# Declare Elements module dependencies
elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(SHE_GST_cIceBRGpy)
elements_depends_on_subdirs(SHE_GST_IceBRGpy)
elements_depends_on_subdirs(SHE_GST_PhysicalModel)
elements_depends_on_subdirs(SHE_PPT)

# Instruction for Python module installation
elements_install_python_modules()

# Instruction for creating a Python executable
elements_add_python_program(GenGalaxyImages SHE_GST_GalaxyImageGeneration.GenGalaxyImages)
elements_add_python_program(SHE_GST_GenGalaxyImages SHE_GST_GalaxyImageGeneration.GenGalaxyImages)
elements_add_python_program(GenPOfE SHE_GST_GalaxyImageGeneration.GenPOfE)

# Install the configuration files
elements_install_conf_files()

# Install the auxiliary files
elements_install_aux_files()
                