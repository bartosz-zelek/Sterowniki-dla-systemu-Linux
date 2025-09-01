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

# cmake-lint: disable=C0103

# Setup custom find_package(<module>) wrappers
# This is generic code that works outside of Simics without build-deps; the
# wrappers fall back on standard find_package() if such a package exists.
function(simics_setup_custom_find_package)
  # Locate packages not supported by CMake out-of-the-box
  set(module_path ${CMAKE_CURRENT_LIST_DIR})
  if(NOT ${module_path} IN_LIST CMAKE_MODULE_PATH)
    list(APPEND CMAKE_MODULE_PATH "${module_path}")
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
      CACHE STRING "Where to find customized CMake modules")
  endif()

  # Load wrappers for Simics build-deps
  set(helpers ${CMAKE_CURRENT_LIST_DIR})
  include(${helpers}/boost.cmake)
  include(${helpers}/hyperscan.cmake)
  include(${helpers}/png.cmake)
  include(${helpers}/zlib.cmake)
endfunction()

# Macro used by modules to setup external dependency
macro(simics_add_external_build_dependency target module)
  if(IGNORE_MISSING_LIBS)
    set(required False)
  else()
    set(required True)
  endif()
  if(${module} STREQUAL Boost)
    add_dependency_Boost(${target} ${required})
  elseif(${module} STREQUAL Hyperscan)
    add_dependency_Hyperscan(${target} ${required})
  elseif(${module} STREQUAL OpenSSL)
    add_dependency_OpenSSL(${target} ${required})
  elseif(${module} STREQUAL PNG)
    add_dependency_PNG(${target} ${required})
  elseif(${module} STREQUAL ZLIB)
    add_dependency_ZLIB(${target} ${required})
  else()
    message(FATAL_ERROR
      "No MODULE named ${module}, in argument passed to macro")
  endif()
endmacro()
