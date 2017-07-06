# Declare this directory as an Elements module
elements_subdir(SHE_GST_IceBRG_lensing)

# Declare Elements module dependencies
elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(SHE_GST_IceBRG_main)
elements_depends_on_subdirs(SHE_GST_IceBRG_physics)

# Declare library dependencies
find_package(Boost)
find_package(Eigen3)

# Instruction for creating a C++ library
elements_add_library(SHE_GST_IceBRG_lensing src/lib/*.cpp
                     LINK_LIBRARIES ElementsKernel Boost boost_serialization Eigen3 SHE_GST_IceBRG_main SHE_GST_IceBRG_physics
                     INCLUDE_DIRS ElementsKernel Boost Eigen3 SHE_GST_IceBRG_main SHE_GST_IceBRG_physics
                     PUBLIC_HEADERS SHE_GST_IceBRG_lensing)
