# Declare this directory as an Elements module
elements_subdir(SHE_GST_cIceBRGpy)

# Declare Elements module dependencies
elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(SHE_GST_IceBRG_main)
elements_depends_on_subdirs(SHE_GST_IceBRG_physics)
elements_depends_on_subdirs(SHE_GST_IceBRG_lensing)

# Declare library dependencies

# Instruction for creating a C++ library

# Instructions for creating a SWIG binding
elements_add_swig_binding(SHE_GST_cIceBRGpy SHE_GST_cIceBRGpy/SHE_GST_cIceBRGpy.i
                          LINK_LIBRARIES SHE_GST_IceBRG_main SHE_GST_IceBRG_physics SHE_GST_IceBRG_lensing
                          INCLUDE_DIRS SHE_GST_IceBRG_main SHE_GST_IceBRG_physics SHE_GST_IceBRG_lensing
						  NO_PUBLIC_HEADERS)

