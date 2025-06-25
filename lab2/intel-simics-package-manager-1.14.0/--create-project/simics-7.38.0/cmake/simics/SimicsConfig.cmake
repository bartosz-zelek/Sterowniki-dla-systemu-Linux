# © 2021 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

# Allows find_package to find the Simics.cmake module:
#
#     find_package(Simics REQUIRED)
#
# This usage ^ requires CMAKE_PREFIX_PATH to be set to ${SIMICS_BASE}/cmake,
# or Simics_DIR/Simics_ROOT to be set to ${SIMICS_BASE}/cmake/simics,
# else use the following syntax:
#
#     find_package(Simics REQUIRED CONFIG HINTS ${SIMICS_BASE}/cmake)
#
# Once found, sets up the project and enables use of simics_add_module() to
# build Simics modules using CMake.
#

include(${CMAKE_CURRENT_LIST_DIR}/Simics.cmake)
include(FindPackageHandleStandardArgs)

if(NOT TARGET Simics::Simics)
  # Sets up global CMake properties common for all Simics targets
  # NOTE: must be run before we setup Simics::Simics or else some
  #       targets will not get the common properties
  simics_global_cmake_properties()

  # Detects where Simics project and Simics package is located;
  # and defines the Simics::Simics target
  simics_project_setup()
endif()

find_package_handle_standard_args(Simics
  REQUIRED_VARS SIMICS_BASE_DIR SIMICS_PROJECT_DIR
  VERSION_VAR Simics_VERSION)
