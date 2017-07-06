# Declare this directory as an Elements module
elements_subdir(SHE_GST_PhysicalModel)

# Declare Elements module dependencies
elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(SHE_GST_IceBRG_main)
elements_depends_on_subdirs(SHE_GST_IceBRG_physics)

# Declare library dependencies
find_package(Boost)
find_package(Eigen3)

# Instruction for creating a C++ library
elements_add_library(_SHE_GST_PhysicalModel src/lib/*.cpp src/lib/*/*.cpp
                     LINK_LIBRARIES ElementsKernel Boost Eigen3 SHE_GST_IceBRG_main
                         SHE_GST_IceBRG_physics
                     INCLUDE_DIRS ElementsKernel Boost Eigen3 SHE_GST_IceBRG_main
                         SHE_GST_IceBRG_physics SHE_GST_PhysicalModel
                     PUBLIC_HEADERS SHE_GST_PhysicalModel)

# Instructions for creating a SWIG binding
elements_add_swig_binding(SHE_GST_PhysicalModel SHE_GST_PhysicalModel/SHE_GST_PhysicalModel.i
						  LINK_LIBRARIES ElementsKernel Boost Eigen3 SHE_GST_IceBRG_main
						      SHE_GST_IceBRG_physics _SHE_GST_PhysicalModel
						  INCLUDE_DIRS ElementsKernel Boost Eigen3 SHE_GST_IceBRG_main
						      SHE_GST_IceBRG_physics SHE_GST_PhysicalModel
						  NO_PUBLIC_HEADERS)

# Instruction for building C++ tests
elements_add_unit_test(SHE_GST_PhysicalModel_tests                  
                       tests/src/*_test.cpp
                       LINK_LIBRARIES ElementsKernel Boost Eigen3 SHE_GST_IceBRG_main
                           SHE_GST_IceBRG_physics _SHE_GST_PhysicalModel
                       INCLUDE_DIRS ElementsKernel Boost Eigen3 SHE_GST_IceBRG_main
                           SHE_GST_IceBRG_physics SHE_GST_PhysicalModel
                       TYPE Boost )