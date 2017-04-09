# - Try to find Argobots 
# Once done this will define
#  SDS_UTILS_FOUND - System has Argobots
#  SDS_UTILS_INCLUDE_DIRS - The Argobots include directories
#  SDS_UTILS_LIBRARIES - The libraries needed to use Argobots
#  SDS_UTILS_DEFINITIONS - Compiler switches required for using Argobots

find_package(PkgConfig)
pkg_check_modules(PC_SDS_UTILS QUIET libxml-2.0)
set(SDS_UTILS_DEFINITIONS ${PC_SDS_UTILS_CFLAGS_OTHER})

find_path(SDS_UTILS_INCLUDE_DIR sds-utils/abt-utils.hh
          HINTS ${PC_SDS_UTILS_INCLUDEDIR} ${PC_SDS_UTILS_INCLUDE_DIRS} ${SDS_UTILS_ROOT}/include)

set(SDS_UTILS_INCLUDE_DIRS ${SDS_UTILS_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set SDS_UTILS2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(SDS_UTILS  DEFAULT_MSG
                                  SDS_UTILS_INCLUDE_DIR)

mark_as_advanced(SDS_UTILS_INCLUDE_DIR)
