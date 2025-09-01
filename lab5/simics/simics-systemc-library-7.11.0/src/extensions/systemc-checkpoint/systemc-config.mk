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

# Find source path for systemc-checkpoint
unique:=/systemc-checkpoint.mk
P1:=modules/systemc-checkpoint$(unique)
P3:=src/extensions/systemc-checkpoint$(unique)
MSG:=Unable to locate: SystemC Checkpoint
$(eval $(call SC_FIND,SYSTEMC_CHECKPOINT_SRC,P1,,P3,MSG,unique))
include $(call _MAKEQUOTE,$(SYSTEMC_CHECKPOINT_SRC)/systemc-checkpoint.mk)

SYSTEMC_CHECKPOINT_CFLAGS += \
  $(SYSTEMC_CHECKPOINT_CFLAGS_DEFAULT) \
  $(CFLAGS_NO_DEPRECATED_DECLARATIONS) \
  $(CFLAGS_NO_MAYBE_UNINITIALIZED)

ifeq ($(MSVC),yes)
  SERIALIZATION_LIB_NAME:=libcci_serialization.lib
  CHECKPOINT_LIB_NAME:=systemc_checkpoint.lib
else
  SERIALIZATION_LIB_NAME:=libcci_serialization.a
  CHECKPOINT_LIB_NAME:=libsystemc-checkpoint.a
endif

P1:=$(HOST_TYPE)/lib/systemc/$(SYSTEMC_KERNEL_VERSION)/$(SERIALIZATION_LIB_NAME)
P2:=$(SYSTEMC_DEPS_CACHE)/$(SYSTEMC_KERNEL_VERSION)/$(SERIALIZATION_LIB_NAME)
P3:=$(P1)
MSG:=Unable to locate: Serialization libs
$(eval $(call SC_FIND,SERIALIZATION_LIB,P1,P2,P3,MSG))

P1:=$(HOST_TYPE)/lib/systemc/$(SYSTEMC_KERNEL_VERSION)/$(CHECKPOINT_LIB_NAME)
P2:=$(SYSTEMC_DEPS_CACHE)/$(SYSTEMC_KERNEL_VERSION)/$(CHECKPOINT_LIB_NAME)
P3:=$(P1)
MSG:=Unable to locate: SystemC Checkpoint libs
$(eval $(call SC_FIND,CHECKPOINT_LIB,P1,P2,P3,MSG))

ifeq ($(WINDOWS),yes)
  ifeq ($(MSVC),yes)
    SYSTEMC_CHECKPOINT_LDFLAGS += /LIBPATH:$(dir $(SERIALIZATION_LIB)) \
                                  $(CHECKPOINT_LIB) \
                                  $(SERIALIZATION_LIB)

    # What Boost libraries to link are provided automatically using
    # pragmas for MSVC, including the Simics serialization library
  else # MinGW
    SYSTEMC_CHECKPOINT_LDFLAGS += -L$(SIMICS_BASE)/$(HOST_TYPE)/lib/systemc/$(SYSTEMC_KERNEL_VERSION) \
                                  $(CHECKPOINT_LIB) \
                                  $(SERIALIZATION_LIB) \
                                  $(LIBZ) \
                                  -lstdc++fs
  endif
else
  SYSTEMC_CHECKPOINT_LDFLAGS += -L$(dir $(SERIALIZATION_LIB)) \
                                -L$(SIMICS_BASE)/$(HOST_TYPE)/lib \
                                $(CHECKPOINT_LIB) \
                                $(SERIALIZATION_LIB) \
                                -lstdc++fs
endif
