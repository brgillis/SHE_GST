# Declare this directory as an Elements module
elements_subdir(SHE_GST_IceBRGpy)

# Declare Elements module dependencies
elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(SHE_GST)
elements_depends_on_subdirs(SHE_GST_cIceBRGpy)

# Declare library dependencies

# Instruction for creating a C++ library

# Instruction for creating a C++ executable

# Instruction for Python module installation
elements_install_python_modules()

# Install the configuration files
         
