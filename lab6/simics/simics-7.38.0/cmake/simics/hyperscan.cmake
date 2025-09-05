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

# This function is expected to be called from simics_setup_build_deps(), to
# cache all libs from build-deps in one location. The function lives here to
# make it easy to verify all paths used by add_dependency_<package> below.
macro(find_Hyperscan root)
  set(HYPERSCAN_ROOT ${root})
  find_package(Hyperscan REQUIRED)
  message(DEBUG "Hyperscan include: ${HYPERSCAN_INCLUDE_DIRS}")
  message(DEBUG "Hyperscan library: ${HYPERSCAN_LIBRARIES}")
  if(NOT (    ${root} STRLESS ${HYPERSCAN_INCLUDE_DIRS}
          AND ${root} STRLESS ${HYPERSCAN_LIBRARIES}))
    message(FATAL_ERROR
      "Located paths for Hyperscan are not all within ${root}")
  endif()
endmacro()

macro(add_dependency_Hyperscan target required)
  if(required)
    find_package(Hyperscan REQUIRED)
  else()
    find_package(Hyperscan)
  endif()
  if(Hyperscan_FOUND)
    target_include_directories(${target} PRIVATE ${HYPERSCAN_INCLUDE_DIRS})
    target_link_libraries(${target} PRIVATE ${HYPERSCAN_LIBRARIES})
  endif()
endmacro()
