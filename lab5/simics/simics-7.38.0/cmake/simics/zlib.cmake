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
macro(find_ZLIB root)
  set(ZLIB_ROOT ${root})
  find_package(ZLIB REQUIRED)
  message(DEBUG "ZLIB include: ${ZLIB_INCLUDE_DIRS}")
  message(DEBUG "ZLIB library: ${ZLIB_LIBRARIES}")
  if(NOT (    ${root} STRLESS ${ZLIB_INCLUDE_DIRS}
          AND ${root} STRLESS ${ZLIB_LIBRARIES}))
    message(FATAL_ERROR "Located paths for ZLIB are not all within ${root}")
  endif()
endmacro()

macro(add_dependency_ZLIB target required)
  if(required)
    find_package(ZLIB REQUIRED)
  else()
    find_package(ZLIB)
  endif()
  if(ZLIB_FOUND)
    target_include_directories(${target} PRIVATE ${ZLIB_INCLUDE_DIRS})
    target_link_libraries(${target} PRIVATE ${ZLIB_LIBRARIES})
  endif()
endmacro()
