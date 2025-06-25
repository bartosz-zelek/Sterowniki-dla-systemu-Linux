# © 2023 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

# Find out if host type is windows and which compiler to use
WINDOWS ?= $(if $(findstring win,$(HOST_TYPE)),yes,no)
IFEQ ?= $(and $(findstring $(1),$(2)),$(findstring $(2),$(1)))
MSVC ?= $(if $(call IFEQ,$(CXX),cl),yes,no)

# Setup paths
include $(MODULE_MAKEFILE:module.mk=systemc-find.mk)

# Setup SERIALIZATION_INCLUDE
unique := /cci/serialization/serialization.hpp
P1 := external/systemc/foundation/checkpoint/serialization$(unique)
P3 := src/external/systemc/foundation/checkpoint/serialization$(unique)
MSG := Unable to locate: Serialization
$(eval $(call SC_FIND,SERIALIZATION_INCLUDE,P1,,P3,MSG,unique))

# Setup SC_CHECKPOINT_INCLUDE
unique := /hierarchical_name.cc
P1 := external/systemc/foundation/checkpoint/systemc-checkpoint$(unique)
P3 := src/external/systemc/foundation/checkpoint/systemc-checkpoint$(unique)
MSG := Unable to locate: SystemC Checkpoint
$(eval $(call SC_FIND,SC_CHECKPOINT_INCLUDE,P1,,P3,MSG,unique))

# Setup SYSTEMC_LIBRARY_DIR
unique:=/systemc-boost.mk
P1:=modules/systemc-library$(unique)
P3:=src/extensions/systemc-library$(unique)
MSG:=Unable to locate: SystemC Library
$(eval $(call SC_FIND,SYSTEMC_LIBRARY_DIR,P1,,P3,MSG,unique))

ifeq ($(SYSTEMC_CHECKPOINT_CFLAGS_DEFAULT),)
    include $(call _MAKEQUOTE,$(SYSTEMC_LIBRARY_DIR)/systemc-boost.mk)
    SYSTEMC_CHECKPOINT_CFLAGS_DEFAULT += $(SYSTEMC_BOOST_CFLAGS_DEFAULT)

    ifeq ($(WINDOWS),yes)
        ifeq ($(MSVC),yes)
            SYSTEMC_CHECKPOINT_CFLAGS_DEFAULT += /I$(SERIALIZATION_INCLUDE)
            SYSTEMC_CHECKPOINT_CFLAGS_DEFAULT += /I$(SC_CHECKPOINT_INCLUDE)
            SYSTEMC_CHECKPOINT_CFLAGS_DEFAULT += /DUSE_SIMICS_CHECKPOINTING
        endif
        SYSTEMC_CHECKPOINT_CFLAGS_DEFAULT += -I$(SERIALIZATION_INCLUDE)
        SYSTEMC_CHECKPOINT_CFLAGS_DEFAULT += -I$(SC_CHECKPOINT_INCLUDE)
        SYSTEMC_CHECKPOINT_CFLAGS_DEFAULT += -DUSE_SIMICS_CHECKPOINTING
    else
        SYSTEMC_CHECKPOINT_CFLAGS_DEFAULT += -I$(SERIALIZATION_INCLUDE)
        SYSTEMC_CHECKPOINT_CFLAGS_DEFAULT += -I$(SC_CHECKPOINT_INCLUDE)
        SYSTEMC_CHECKPOINT_CFLAGS_DEFAULT += -DUSE_SIMICS_CHECKPOINTING
    endif
endif
