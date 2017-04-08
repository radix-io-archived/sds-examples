#-----------------------------------------------------------#
# Copyright (C) 2017 University of Chicago.
# See COPYRIGHT notice in top-level directory.
#-----------------------------------------------------------#
cmake_minimum_required (VERSION 2.8)

#-----------------------------------------------------------#
# Starting project definition
#-----------------------------------------------------------#
project(SDS_EXAMPLES)

set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -pedantic")

#-----------------------------------------------------------#
# Set the location of CMake Modules
#-----------------------------------------------------------#
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
		"${SDS_EXAMPLES_SOURCE_DIR}/cmake/Modules/")

if(NOT DEFINED CMAKE_INSTALL_PREFIX)
	set(CMAKE_INSTALL_PREFIX /usr/local)
endif(NOT DEFINED CMAKE_INSTALL_PREFIX)

find_package(ABT)
if(${ABT_FOUND})
  include_directories(${ABT_INCLUDE_DIR})
endif()

find_package(MERCURY)
if(${MERCURY_FOUND})
  find_package(BMI REQUIRED)
  find_package(Boost REQUIRED)
  include_directories(${MERCURY_INCLUDE_DIRS})
  include_directories(${BMI_INCLUDE_DIRS})
  include_directories(${Boost_INCLUDE_DIRS})
endif()

if(${MERCURY_FOUND} AND ${ABT_FOUND})
  find_package(ABT_SNOOZER)
  if(${ABT_SNOOZER_FOUND})
    find_package(MARGO)
    include_directories(${MARGO_INCLUDE_DIRS})
    include_directories(${ABT_SNOOZER_INCLUDE_DIRS})
  endif()
endif()

#-----------------------------------------------------------#
include_directories(${SDS_EXAMPLES_SOURCE_DIR})

#-----------------------------------------------------------#
# Set subdirectories for the compilation process
#-----------------------------------------------------------#
add_subdirectory(mercury)
add_subdirectory(argobots)
add_subdirectory(margo)
