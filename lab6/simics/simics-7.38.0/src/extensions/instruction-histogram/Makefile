# -*- Makefile -*-

# © 2018 Intel Corporation
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

PYTHON_FILES = module_load.py simics_start.py

MODULE_CLASSES := instruction_histogram                \
                  mnemonic_histogram_connection        \
                  size_histogram_connection            \
                  x86_normalized_histogram_connection  \
                  xed_iform_histogram_connection

SRC_FILES += instruction-histogram.c generic-view.c
SRC_FILES += mnemonic-view.c
SRC_FILES += size-view.c
SRC_FILES += x86-normalized-view.c
SRC_FILES += xed-iform-view.c
SIMICS_API := latest
THREAD_SAFE = yes

include $(MODULE_MAKEFILE)
