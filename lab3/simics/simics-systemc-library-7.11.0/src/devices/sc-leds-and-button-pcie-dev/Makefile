# -*- Makefile -*-

# © 2017 Intel Corporation
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

MODULE_CLASSES = sc_leds_and_button_pcie_device
MODULE_COMPONENTS = sc_leds_and_button_pcie_comp leds_and_button_panel

SRC_FILES = pci-device.cc pci-io-mem.cc simics-init.cc
PYTHON_FILES = module_load.py utility.py
EXTRA_MODULE_VPATH = systemc-commands
SRC_IMAGES = \
	led-background-plate.png \
	red-on-32.png \
	red-off-32.png \
	blue-off-32.png \
	blue-on-32.png \
	yellow-off-32.png \
	yellow-on-32.png \
	green-off-32.png \
	green-on-32.png

SIMICS_API := 6
THREAD_SAFE = yes
SYSTEMC = yes
USE_SIMICS_CHECKPOINTING = yes

# Prefer Visual Studio on Windows
COMPILERS = cl gcc


include $(MODULE_MAKEFILE)
