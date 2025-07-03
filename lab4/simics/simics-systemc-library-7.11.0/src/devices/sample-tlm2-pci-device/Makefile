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

MODULE_CLASSES = sample_tlm2_pci_device \
                 sample_tlm2_pci_device_external
SRC_FILES = pci-device.cc simics-init.cc
PYTHON_FILES = module_load.py utility.py
EXTRA_MODULE_VPATH = systemc-commands

SIMICS_API := 6
THREAD_SAFE = yes
SYSTEMC = yes

# Prefer Visual Studio on Windows
COMPILERS = cl gcc

include $(MODULE_MAKEFILE)
