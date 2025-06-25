# -*- Makefile -*-

# © 2020 Intel Corporation
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
# Makefile for setting up build environment.

# We need SC_FIND to have been setup for searching for files and directories.
ifeq ($(SC_FIND),)
    $(error SC_FIND is undefined)
endif

# Find out if host type is windows and which compiler to use
WINDOWS := $(if $(findstring win,$(HOST_TYPE)),yes,no)
IFEQ = $(and $(findstring $(1),$(2)),$(findstring $(2),$(1)))
MSVC := $(if $(call IFEQ,$(CXX),cl),yes,no)

INCDIR := $(if $(call IFEQ,$(MSVC),yes),/I,-I)
DEF := $(if $(call IFEQ,$(MSVC),yes),/D,-D)

ifneq ($(SYSTEMC_CXX),)
    CXX = $(SYSTEMC_CXX)
    DEP_CXX = $(SYSTEMC_CXX)
endif

EXTRA_MODULE_VPATH += systemc-cci

# Setup cflags for CCI.
ifeq ($(SYSTEMC_CCI_CFLAGS),)
    unique := /src/cci_configuration
    P1 := external/systemc/cci$(unique)
    P3 := src/external/systemc/cci$(unique)
    MSG := Unable to locate: CCI
    $(eval $(call SC_FIND,CCI_INC,P1,,P3,MSG,unique))
    SYSTEMC_CCI_CFLAGS = $(INCDIR)$(CCI_INC)/src $(DEF)USE_SIMICS_CCI
endif

ifeq ($(SYSTEMC_CCI_LDFLAGS),)
    ifeq ($(MSVC),yes)
        SYSTEMC_CCI_LIB := cciapi.lib
    else
        SYSTEMC_CCI_LIB := libcciapi.a
    endif

    P1 := $(HOST_TYPE)/lib/systemc/$(SYSTEMC_KERNEL_VERSION)/$(SYSTEMC_CCI_LIB)
    P2 := $(SYSTEMC_DEPS_CACHE)/$(SYSTEMC_KERNEL_VERSION)/$(SYSTEMC_CCI_LIB)
    P3 := $(P1)
    MSG := Unable to locate: CCI libs
    $(eval $(call SC_FIND,SYSTEMC_CCI_LIB,P1,P2,P3,MSG))
    SYSTEMC_CCI_LDFLAGS = $(SYSTEMC_CCI_LIB)
endif
