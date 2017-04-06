# - Try to find BMI
# Once done, this will define
#
#  BMI_FOUND - system has BMI
#  BMI_INCLUDE_DIRS - the BMI include directories
#  BMI_LIBRARIES - link these to use BMI

# Include dir
find_path(BMI_INCLUDE_DIR
  bmi.h
  HINTS ${BMI_ROOT}/include
)

# Finally the library itself
find_library(BMI_LIBRARY
  bmi
  HINTS ${BMI_ROOT}/lib
)

set(BMI_LIBRARIES ${BMI_LIBRARY})
set(BMI_INCLUDE_DIRS ${BMI_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set BMI_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(BMI  DEFAULT_MSG
                                  BMI_LIBRARY BMI_INCLUDE_DIR)

mark_as_advanced(BMI_INCLUDE_DIR BMI_LIBRARY)
