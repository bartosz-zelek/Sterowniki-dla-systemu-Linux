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

.PHONY: all
all:
ifeq ($(INTC_EXT),0)
	$(info Skipping systemc-checkpoint due to INTC_EXT=0)
else
	$(MAKE) -C $(CHECKPOINT_DIR)
endif

WINDOWS:=$(if $(findstring win,$(HOST_TYPE)),yes,no)

include $(MODULE_MAKEFILE:module.mk=systemc-find.mk)

UNIQUE:=/Makefile
P1:=external/systemc/foundation/checkpoint/systemc-checkpoint$(UNIQUE)
P3:=src/external/systemc/foundation/checkpoint/systemc-checkpoint$(UNIQUE)
MSG:=Unable to locate: SystemC Checkpoint
$(eval $(call SC_FIND,CHECKPOINT_DIR,P1,,P3,MSG,UNIQUE))

UNIQUE:=/Makefile
P1:=external/systemc/foundation/checkpoint/serialization$(UNIQUE)
P3:=src/external/systemc/foundation/checkpoint/serialization$(UNIQUE)
MSG:=Unable to locate: Serialization
$(eval $(call SC_FIND,SERIALIZATION_DIR,P1,,P3,MSG,UNIQUE))

unique:=/Makefile.am
P1:=external/systemc/kernel-$(SYSTEMC_KERNEL_VERSION)/$(unique)
P3:=src/external/systemc/kernel-$(SYSTEMC_KERNEL_VERSION)/$(unique)
MSG:=Unable to locate: SystemC Kernel
$(eval $(call SC_FIND,KERNEL_DIR,P1,,P3,MSG,unique))

unique:=/systemc-boost.mk
P1:=modules/systemc-library$(unique)
P3:=src/extensions/systemc-library$(unique)
MSG:=Unable to locate: SystemC Library
$(eval $(call SC_FIND,SYSTEMC_LIBRARY_DIR,P1,,P3,MSG,unique))

P1:=$(HOST_TYPE)/lib/systemc/$(SYSTEMC_KERNEL_VERSION)/libcci_serialization.a
P2:=$(SYSTEMC_DEPS_CACHE)/$(SYSTEMC_KERNEL_VERSION)/libcci_serialization.a
P3:=$(P1)
MSG:=Unable to locate: Serialization libs
ifneq ($(INTC_EXT),0)
$(eval $(call SC_FIND,SERIALIZATION_LIB,P1,P2,P3,MSG))
endif

# Linked by systemc-checkpoint/unittest
P1:=$(HOST_TYPE)/lib/systemc/$(SYSTEMC_KERNEL_VERSION)/libsystemc.a
P2:=$(SYSTEMC_DEPS_CACHE)/$(SYSTEMC_KERNEL_VERSION)/libsystemc.a
P3:=$(P1)
MSG:=Unable to locate: SystemC Kernel libs
$(eval $(call SC_FIND,KERNEL_LIB,P1,P2,P3,MSG))

SYSTEMC_DIR:=$(strip $(dir $(wildcard $(SIMICS_PROJECT)/$(HOST_TYPE)/lib/systemc/$(SYSTEMC_KERNEL_VERSION)/libsystemc.a)))
ifeq ($(SYSTEMC_DIR),)
  SYSTEMC_DIR:=$(strip $(dir $(shell $(SIMICS_PROJECT)/bin/lookup-file -f $(HOST_TYPE)/lib/systemc/$(SYSTEMC_KERNEL_VERSION)/libsystemc.a)))
endif

SYS_LIB_DIR:=$(strip $(dir $(shell $(SIMICS_PROJECT)/bin/lookup-file -f $(HOST_TYPE)/sys/lib/libstdc++.so.6)))

include $(call _MAKEQUOTE,$(SYSTEMC_LIBRARY_DIR)/systemc-boost.mk)
include $(call _MAKEQUOTE,$(KERNEL_DIR)/intc-config.mk)

ifneq ($(SYSTEMC_CXX),)
CXX = $(SYSTEMC_CXX)
endif

CXXFLAGS += $(foreach d,$(SYSTEMC_KERNEL_INTC_DEFINES),-D$(d))
ifneq ($(SYSTEMC_CXX_STANDARD),)
    CXXFLAGS += -std=c++$(SYSTEMC_CXX_STANDARD)
endif

LDFLAGS += \
  -L$(SYSTEMC_DIR) \
  -L$(SYS_LIB_DIR)

ifeq ($(SYSTEMC_NO_UNITTESTS),1)
  SKIP_TESTS:=yes
endif
ifeq ($(INTC_EXT),0)
  SKIP_TESTS:=yes
endif
ifeq ($(WINDOWS),yes)
  SKIP_TESTS:=yes
endif

export AR
export BOOST_INC_DIR:=$(BOOST_INC_PATH)
export BOOST_LIB_DIR:=$(BOOST_LIB_PATH)
export CXX
export CXXFLAGS
export KERNEL_DIR
export KERNEL_LIB
export LDFLAGS
export LIBRARY_PATHS:=$(SYS_LIB_DIR)
export LIB_DIR:=$(TARGET_DIR)/systemc/$(SYSTEMC_KERNEL_VERSION)
export OBJ_DIR:=$(CURDIR)
export SERIALIZATION_DIR
export SERIALIZATION_LIB
export SKIP_TESTS
export WINDOWS
