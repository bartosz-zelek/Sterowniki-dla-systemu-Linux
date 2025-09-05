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

MODULE_COMPONENTS := x86_chassis motherboard_x86_simple motherboard_x86_simple_no_apic

PYTHON_FILES = x86_comp.py x86_comp_cmos.py x86_connector.py x86_motherboard.py \
	x86_northbridge.py x86_processor.py x86_southbridge.py module_load.py
SRC_IMAGES = intel-desktop.png intel-server.png intel-logo.png

THREAD_SAFE=yes
SIMICS_API := latest

include $(MODULE_MAKEFILE)
