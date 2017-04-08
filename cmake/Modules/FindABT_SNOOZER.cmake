# - Try to find Argobots 
# Once done this will define
#  ABT_SNOOZER_FOUND - System has Argobots
#  ABT_SNOOZER_INCLUDE_DIRS - The Argobots include directories
#  ABT_SNOOZER_LIBRARIES - The libraries needed to use Argobots
#  ABT_SNOOZER_DEFINITIONS - Compiler switches required for using Argobots

find_package(PkgConfig)
pkg_check_modules(PC_ABT_SNOOZER QUIET libxml-2.0)
set(ABT_SNOOZER_DEFINITIONS ${PC_ABT_SNOOZER_CFLAGS_OTHER})

find_path(ABT_SNOOZER_INCLUDE_DIR abt-snoozer.h
          HINTS ${PC_ABT_SNOOZER_INCLUDEDIR} ${PC_ABT_SNOOZER_INCLUDE_DIRS} ${ABT_SNOOZER_ROOT}/include)

find_library(ABT_SNOOZER_LIBRARY NAMES abt-snoozer libabt-snoozer
             HINTS ${PC_ABT_SNOOZER_LIBDIR} ${PC_ABT_SNOOZER_LIBRARY_DIRS} ${ABT_SNOOZER_ROOT}/lib)

set(ABT_SNOOZER_LIBRARIES ${ABT_SNOOZER_LIBRARY} )
set(ABT_SNOOZER_INCLUDE_DIRS ${ABT_SNOOZER_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set ABT_SNOOZER2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(ABT_SNOOZER  DEFAULT_MSG
                                  ABT_SNOOZER_LIBRARY ABT_SNOOZER_INCLUDE_DIR)

mark_as_advanced(ABT_SNOOZER_INCLUDE_DIR ABT_SNOOZER_LIBRARY)
