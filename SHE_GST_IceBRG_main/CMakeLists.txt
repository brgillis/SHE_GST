# Declare this directory as an Elements module
elements_subdir(SHE_GST_IceBRG_main)

# Declare Elements module dependencies
elements_depends_on_subdirs(ElementsKernel)

# Declare library dependencies
find_package(Boost)
find_package(Eigen3)

# Instruction for creating a C++ library
elements_add_library(SHE_GST_IceBRG_main src/lib/*.cpp
                     LINK_LIBRARIES ElementsKernel Boost Eigen3
                     INCLUDE_DIRS ElementsKernel Boost Eigen3
                     PUBLIC_HEADERS SHE_GST_IceBRG_main)
                     