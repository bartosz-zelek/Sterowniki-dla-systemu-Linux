# -*- Makefile -*-

# © 2016 Intel Corporation
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

MODULE_CLASSES := lt_temporal_decouple_chkpt_example

PYTHON_FILES := module_load.py

MODULE_CFLAGS := -DINTC_CHKPT

COMMON_SRC := initiator_top.cpp \
              td_initiator_top.cpp \
              lt_initiator.cpp \
              lt_td_initiator.cpp \
              lt_target.cpp \
              lt_synch_target.cpp \
              memory.cpp \
              report.cpp \
              traffic_generator.cpp

SERIALIZER_SRC := serializer_initiator_top.cc \
                  serializer_traffic_generator.cc \
                  serializer_lt_initiator.cc \
                  serializer_lt_synch_target.cc \
                  serializer_lt_target.cc \
                  serializer_lt_td_initiator.cc \
                  serializer_td_initiator_top.cc

SRC_FILES := sc_factory.cc init.cc $(COMMON_SRC) lt_temporal_decouple_top.cpp $(SERIALIZER_SRC)

COMMON_INCLUDE := src/external/systemc/kernel-$(SYSTEMC_KERNEL_VERSION)/examples/tlm

EXTRA_VPATH_IN_SIMICS += $(COMMON_INCLUDE)/common/include \
                         $(COMMON_INCLUDE)/common/src \
                         $(COMMON_INCLUDE) \
                         $(COMMON_INCLUDE)/lt_temporal_decouple/include	\
                         $(COMMON_INCLUDE)/lt_temporal_decouple/src

EXTRA_MODULE_VPATH := accellera-at-1-phase

SIMICS_API := latest
THREAD_SAFE := yes
SYSTEMC := yes
_SYSTEMC_GASKET_CLASSES = no

# Prefer Visual Studio on Windows
COMPILERS = cl gcc

include $(MODULE_MAKEFILE)
