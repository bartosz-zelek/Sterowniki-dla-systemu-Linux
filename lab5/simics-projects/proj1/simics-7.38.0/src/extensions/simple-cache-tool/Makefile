# -*- Makefile -*-

# © 2020 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

# Simics module makefile
# empty-instrumentation-tool.c
#

PYTHON_FILES = module_load.py simics_start.py

MODULE_CLASSES = simple_cache_tool simple_cache_connection simple_cache \
                 simple_directory
SRC_FILES += cache-tool.c cache-connection.c simple-cache.c simple-directory.c
IFACE_FILES := simple-cache-interface.h

SIMICS_API := latest
THREAD_SAFE = yes

include $(MODULE_MAKEFILE)


