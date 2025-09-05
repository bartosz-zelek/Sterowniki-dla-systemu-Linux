# © 2010 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.
#

# Valgrind is Linux-only; it depends on the Linux kernel
if(NOT WIN32)
  # Builds a module required to run Simics through Valgrind,
  # does not expose any classes
  simics_add_module(valgrind-support
    SOURCES valgrind-support.c
    INIT_LOCAL)

  # NOTE: put the path to Valgrind headers into VALGRIND_INCLUDE_DIR
  target_include_directories(valgrind-support PRIVATE ${VALGRIND_INCLUDE_DIR})
endif()
