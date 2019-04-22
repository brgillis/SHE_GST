# Declare this directory as an Elements module
elements_subdir(SHE_GST_IceBRG_physics)

# Declare Elements module dependencies
elements_depends_on_subdirs(ElementsKernel SHE_GST_IceBRG_main)
elements_depends_on_subdirs(SHE_GST)

# Declare library dependencies
find_package(Boost)
find_package(Eigen3)

# Instruction for creating a C++ library
elements_add_library(SHE_GST_IceBRG_physics src/lib/*.cpp src/lib/*/*.cpp
                     LINK_LIBRARIES ElementsKernel Boost Eigen3 SHE_GST_IceBRG_main
                     INCLUDE_DIRS ElementsKernel Boost Eigen3 SHE_GST_IceBRG_main
                     PUBLIC_HEADERS SHE_GST_IceBRG_physics)

# Instruction for building C++ tests
elements_add_unit_test(SHE_GST_IceBRG_physics_test                  
                       tests/src/*_test.cpp
                       LINK_LIBRARIES ElementsKernel Boost Eigen3 SHE_GST_IceBRG_main SHE_GST_IceBRG_physics
                       INCLUDE_DIRS ElementsKernel Boost Eigen3 SHE_GST_IceBRG_main SHE_GST_IceBRG_physics
                       TYPE Boost)

# Install the auxiliary files
elements_install_aux_files()