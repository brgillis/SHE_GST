# Declare this directory as an Elements module
elements_subdir(SHE_GST)

# Declare Elements module dependencies
elements_depends_on_subdirs(ElementsKernel)

# Instruction for Python module installation
elements_install_python_modules()
                
