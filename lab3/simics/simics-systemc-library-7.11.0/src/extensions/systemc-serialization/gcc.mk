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

.PHONY: all
all:
ifeq ($(INTC_EXT),0)
	$(info Skipping serialization due to INTC_EXT=0)
else
	$(MAKE) -C $(SERIALIZATION)
endif


WINDOWS:=$(if $(findstring win,$(HOST_TYPE)),yes,no)

include $(MODULE_MAKEFILE:module.mk=systemc-find.mk)

UNIQUE:=/Makefile
P1:=external/systemc/foundation/checkpoint/serialization$(UNIQUE)
P3:=src/external/systemc/foundation/checkpoint/serialization$(UNIQUE)
MSG:=Unable to locate: Serialization
$(eval $(call SC_FIND,SERIALIZATION,P1,,P3,MSG,UNIQUE))

LOOKUP_DIR = $(strip $(dir $(shell $(SIMICS_PROJECT)/bin/lookup-file -f $(1))))
ifeq ($(WINDOWS),yes)
  BIN_DIR:=$(call LOOKUP_DIR,linux64/bin/libvtutils.lib)
else
  BIN_DIR:=$(call LOOKUP_DIR,linux64/bin/libvtutils.so)
  # sys/lib does not exist on Windows
  SYS_LIB_DIR:=$(call LOOKUP_DIR,linux64/sys/lib/libstdc++.so.6)
endif

unique:=/systemc-boost.mk
P1:=modules/systemc-library$(unique)
P3:=src/extensions/systemc-library$(unique)
MSG:=Unable to locate: SystemC Library
$(eval $(call SC_FIND,SYSTEMC_LIBRARY,P1,,P3,MSG,unique))
include $(call _MAKEQUOTE,$(SYSTEMC_LIBRARY)/systemc-boost.mk)

LDFLAGS:= -L$(BIN_DIR)
LIBRARY_PATHS:=$(BIN_DIR)
ifneq ($(WINDOWS),yes)
  LDFLAGS += -L$(SYS_LIB_DIR)
  LIBRARY_PATHS:=$(LIBRARY_PATHS):$(SYS_LIB_DIR)
endif

ifeq ($(WINDOWS),yes)
  SKIP_TESTS:=yes
endif
ifeq ($(SYSTEMC_NO_UNITTESTS),1)
  SKIP_TESTS:=yes
endif

ifneq ($(SYSTEMC_CXX),)
CXX = $(SYSTEMC_CXX)
endif

CXXFLAGS += \
  $(BLD_CXXFLAGS) \
  $(CFLAGS_NO_ATTRIBUTES) \
  $(CFLAGS_NO_DEPRECATED_DECLARATIONS) \
  $(CFLAGS_NO_UNDEF) \
  $(CFLAGS_NO_UNUSED_LOCAL_TYPEDEFS) \
  -Wno-uninitialized  # GCC 12 flags wchar_from_mb.hpp:102 as uninitialized

CXXFLAGS := $(filter-out -DNDEBUG,$(CXXFLAGS))
ifneq ($(SYSTEMC_CXX_STANDARD),)
    CXXFLAGS += -std=c++$(SYSTEMC_CXX_STANDARD)
endif

export BOOST_INC_DIR:=$(BOOST_INC_PATH)
export BOOST_LIB_DIR:=$(BOOST_LIB_PATH)
export CXX
export CXXFLAGS
export LDFLAGS
export LIBRARY_PATHS
export LIB_DIR:=$(TARGET_DIR)/systemc/$(SYSTEMC_KERNEL_VERSION)
export OBJ_DIR:=$(CURDIR)
export SKIP_TESTS
export WINDOWS
