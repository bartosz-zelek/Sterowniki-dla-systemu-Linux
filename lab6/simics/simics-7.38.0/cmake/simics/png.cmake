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

# cmake-lint: disable=C0103,C0111

# See hyperscan.cmake for details
macro(find_PNG root)
  # NOTE: PNG depends on ZLIB, but we assume it has been found and cached by
  # Simics build-deps already OR else it will be found and located by
  # find_package(PNG) using host paths
  set(PNG_ROOT ${root})
  find_package(PNG REQUIRED)
  message(DEBUG "PNG include: ${PNG_INCLUDE_DIRS}")
  message(DEBUG "PNG library: ${PNG_LIBRARIES}")
  foreach(path ${PNG_INCLUDE_DIRS} ${PNG_LIBRARIES})
    if(NOT ${root} STRLESS ${path})
      message(FATAL_ERROR "Located paths for PNG are not all within ${root}")
    endif()
  endforeach()
endmacro()

macro(add_dependency_PNG target required)
  if(required)
    find_package(PNG REQUIRED)
  else()
    find_package(PNG)
  endif()
  if(PNG_FOUND)
    target_include_directories(${target} PRIVATE ${PNG_INCLUDE_DIRS})
    target_link_libraries(${target} PRIVATE ${PNG_LIBRARIES})
  endif()
endmacro()
