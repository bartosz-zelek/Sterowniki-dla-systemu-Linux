# -*- Makefile -*-

# © 2015 Intel Corporation
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

MODULE_CLASSES = at_1_phase_example

PYTHON_FILES = module_load.py

COMMON_SRC=traffic_generator.cpp \
           memory.cpp \
           report.cpp \
           at_target_1_phase.cpp \
           select_initiator.cpp
SRC_FILES = sc_factory.cc init.cc $(COMMON_SRC) \
	at_1_phase_top.cpp initiator_top.cpp

COMMON_INCLUDE := src/external/systemc/kernel-$(SYSTEMC_KERNEL_VERSION)/examples/tlm
EXTRA_VPATH_IN_SIMICS += $(COMMON_INCLUDE)/common/include	\
                         $(COMMON_INCLUDE)/common/src		\
                         $(COMMON_INCLUDE)/at_1_phase/include	\
                         $(COMMON_INCLUDE)/at_1_phase/src	\
                         $(COMMON_INCLUDE)

SIMICS_API := latest
THREAD_SAFE = yes
SYSTEMC = yes
_SYSTEMC_GASKET_CLASSES = no

# Prefer Visual Studio on Windows
COMPILERS = cl gcc

include $(MODULE_MAKEFILE)
