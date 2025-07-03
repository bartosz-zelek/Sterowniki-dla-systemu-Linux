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

MODULE_COMPONENTS := northbridge_x58 southbridge_ich10_panel southbridge_ich10 motherboard_x58_ich10 chassis_x58_ich10 chassis_qsp_x86
MODULE_CLASSES := x86_reset_signal_conv auto_apic_bus


PYTHON_FILES = x58_ich10.py simics_start.py module_load.py
SRC_IMAGES = x58-ich10.png

THREAD_SAFE=yes
SIMICS_API := latest


include $(MODULE_MAKEFILE)
