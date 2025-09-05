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

# For ${MODULE_NAME}::headers, ${MODULE_NAME}::imports, ${MODULE_NAME}::includes
option(SIMICS_AUTO_ADD_DEPENDENCIES_ON_MODULE_WITH_HEADERS
  "Automatically invoke add_dependencies(...) on modules we use ::includes from" ON)

option(USE_CCACHE "Use a fast C/C++ compiler cache" OFF)
if(USE_CCACHE)
  find_program(CCACHE_EXECUTABLE ccache)
  if(CCACHE_EXECUTABLE)
    message(STATUS "ccache enabled")
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_EXECUTABLE}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_EXECUTABLE}")
  endif()
endif()

# TODO(ah): When we are at minimum 3.24; we should rely on
#           CMAKE_COLOR_DIAGNOSTICS and let user control the coloring output of
#           GNU Make using a standard CMake variable.
option(USE_COLORED_OUTPUT "Enable ANSI-colored output during compilation" ON)
if(USE_COLORED_OUTPUT)
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-fdiagnostics-color=always)
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-fcolor-diagnostics)
  endif()
endif()

# Generic GCOV coverage option; see README.md for more details
option(USE_COVERAGE "Enable GCOV during the build" OFF)
if(USE_COVERAGE)
  # Force -Og to make sure GCOV can capture detailed coverage
  # NOTE: user should consider changing CMAKE_BUILD_TYPE to Debug instead
  add_compile_options(--coverage -g -Og)
  add_link_options(--coverage -Wl,--dynamic-list-data)
endif()

# Sanitizes configuration
option(USE_ASAN
  "Compile modules marked with simics_add_sanitizers with ASAN" OFF)
option(USE_UBSAN
  "Compile modules marked with simics_add_sanitizers with UBSAN" OFF)
option(ASAN_STACK_USE_AFTER_RETURN
  "Same as USE_ASAN but also enables Stack-use-after-return" OFF)

# DML dead methods check
option(USE_DML_DEAD_METHODS_CHECK
  "Check modules marked with simics_add_dead_methods_check for dead methods" OFF)
