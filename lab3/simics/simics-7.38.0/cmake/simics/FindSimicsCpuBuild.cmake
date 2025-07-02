# © 2024 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

if(NOT SIMICS_CPU_ADDON_DIR)
  _simics_load_packages()
  foreach(package ${packages})
    if(EXISTS ${package}/src/cpu/cpu.cmake)
      set(SIMICS_CPU_ADDON_DIR ${package})
      break()
    endif()
  endforeach()
endif()

message(TRACE "Found Simics CPU Build Support Package in"
  " ${SIMICS_CPU_ADDON_DIR}/src/cpu/cpu.cmake")
include(${SIMICS_CPU_ADDON_DIR}/src/cpu/cpu.cmake)
