# -*- Makefile -*-

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

# Simics module makefile

# Needs to match list in simics_start.py.
MODULE_COMPONENTS := processor_core_i7 processor_core_i7_single \
	processor_core_i7_duo processor_core_i7_6c_2t \
	processor_xeon_5500 processor_xeon_5530 \
	processor_core_i7_8c_4t processor_x86_intel64 processor_x86QSP1

PYTHON_FILES = x86_nehalem.py module_load.py simics_start.py

THREAD_SAFE=yes
SIMICS_API := latest


include $(MODULE_MAKEFILE)
