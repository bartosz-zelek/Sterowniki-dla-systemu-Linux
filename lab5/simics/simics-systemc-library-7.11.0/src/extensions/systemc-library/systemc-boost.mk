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

# If BOOST_INC_PATH or BOOST_LIB_PATH is not set, get it from BOOST_PREFIX
ifneq ($(BOOST_PREFIX),)
    ifeq ($(BOOST_INC_PATH),)
        export BOOST_INC_PATH := $(BOOST_PREFIX)/include
    endif
    ifeq ($(BOOST_LIB_PATH),)
        export BOOST_LIB_PATH := $(BOOST_PREFIX)/lib
    endif
    BOOST_PREFIX :=
endif

ifeq ($(BOOST_INC_PATH),)
    $(error BOOST_INC_PATH not set. See SystemC Library Programming Guide for details.)
endif

# Find out if host type is windows and what compiler to use
WINDOWS := $(if $(findstring win,$(HOST_TYPE)),yes,no)
IFEQ = $(and $(findstring $(1),$(2)),$(findstring $(2),$(1)))
MSVC := $(if $(call IFEQ,$(CXX),cl),yes,no)

ifeq ($(SYSTEMC_BOOST_CFLAGS_DEFAULT),)
    ifeq ($(WINDOWS),yes)
        ifeq ($(MSVC),yes)
            SYSTEMC_BOOST_CFLAGS_DEFAULT := /I$(BOOST_INC_PATH)
        else
            SYSTEMC_BOOST_CFLAGS_DEFAULT := -I$(BOOST_INC_PATH)
        endif
    else
        SYSTEMC_BOOST_CFLAGS_DEFAULT := -isystem $(BOOST_INC_PATH)
    endif
endif
