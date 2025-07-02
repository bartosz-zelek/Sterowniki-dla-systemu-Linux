# © 2023 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

simics_add_module(simple-cache-tool
  CLASSES simple_cache_tool simple_cache_connection simple_cache simple_directory
  INTERFACES simple_cache
  SOURCES cache-connection.c cache-tool.c simple-cache.c simple-directory.c module_load.py simics_start.py simple-cache-interface.h
  INIT_LOCAL
  SIMICS_API latest
)
