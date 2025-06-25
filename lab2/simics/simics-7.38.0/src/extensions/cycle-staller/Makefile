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

# USER-TODO: specify the class(es) implemented in this module
MODULE_CLASSES = cycle_staller

# USER-TODO: set file-names
SRC_FILES = cycle-staller.c

PYTHON_FILES = module_load.py simics_start.py

MODULE_CFLAGS =

SIMICS_API := latest
THREAD_SAFE:=yes

EXTRA_MODULE_VPATH = simple-cache-tool

ifeq ($(MODULE_MAKEFILE),)
$(error Make sure you compile your module from the project directory)
else
include $(MODULE_MAKEFILE)
endif
