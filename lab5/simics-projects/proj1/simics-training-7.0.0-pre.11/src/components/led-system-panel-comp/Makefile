# © 2022 Intel Corporation
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
# Simics module makefile for the panel component
#
# List all the image files used
#

MODULE_COMPONENTS := led_system_panel_comp
SIMICS_API := latest
THREAD_SAFE:=yes

SRC_IMAGES := bg-board-1000x500.png  led-background.png  button-A.png  \
	button-B.png button-A-highlight.png  button-B-highlight.png \
	led-blue.png led-green.png led-red.png \
	led-black.png led-cyan.png  led-magenta.png  \
	led-white.png toggle-1.png led-yellow.png  \
	toggle-0.png toggle-change.png 

PYTHON_FILES = led_system_panel_comp.py module_load.py

COMPILE_PYC = 0

include $(MODULE_MAKEFILE)
