# Declare this directory as an Elements module
elements_subdir(SHE_GST_cIceBRGpy)

# Declare Elements module dependencies
elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(SHE_GST_IceBRG_main)
elements_depends_on_subdirs(SHE_GST_IceBRG_physics)

# Declare library dependencies

# Instruction for creating a C++ library

# Instructions for creating a SWIG binding
elements_add_swig_binding(SHE_GST_cIceBRGpy SHE_GST_cIceBRGpy/SHE_GST_cIceBRGpy.i
                          LINK_LIBRARIES SHE_GST_IceBRG_main SHE_GST_IceBRG_physics
                          INCLUDE_DIRS SHE_GST_IceBRG_main SHE_GST_IceBRG_physics /cvmfs/euclid.in2p3.fr/CentOS7/EDEN-2.0/usr/lib64/python3.6/site-packages/numpy/core/include
						  NO_PUBLIC_HEADERS)

