# -*- Makefile -*-

# © 2025 Intel Corporation
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

MODULE_CLASSES := x58-mb

QSP_X86_FILES := __init__.py board.py disks.py ich10.py state.py x58.py
X86_CPU_FILES := __init__.py cpu.py system.py state.py
TEST_FILES := network_setup.py qsp_platform.py pcie.py disks.py __init__.py

CLASSES_FILES := __init__.py x86_classes.py

PYTHON_FILES := module_load.py \
	$(addprefix qsp_x86/,$(QSP_X86_FILES)) \
	$(addprefix x86_cpu/,$(X86_CPU_FILES)) \
	$(addprefix classes/,$(CLASSES_FILES)) \
	$(addprefix tests/,$(TEST_FILES))

THREAD_SAFE = yes
SIMICS_API := latest

include $(MODULE_MAKEFILE)
