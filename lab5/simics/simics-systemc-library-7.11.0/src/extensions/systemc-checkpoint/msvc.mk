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

CHECKPOINT_LIB:=$(TARGET_DIR)/systemc/$(SYSTEMC_KERNEL_VERSION)/systemc_checkpoint.lib

include $(MODULE_MAKEFILE:module.mk=systemc-find.mk)

unique:=/Makefile.am
P1:=external/systemc/kernel-$(SYSTEMC_KERNEL_VERSION)$(unique)
P3:=src/external/systemc/kernel-$(SYSTEMC_KERNEL_VERSION)$(unique)
MSG:=Unable to locate: SystemC Kernel
$(eval $(call SC_FIND,KERNEL_DIR,P1,,P3,MSG,unique))

# Find source path for systemc-checkpoint
unique:=/systemc-checkpoint.mk
P1:=modules/systemc-checkpoint$(unique)
P3:=src/extensions/systemc-checkpoint$(unique)
MSG:=Unable to locate: SystemC Checkpoint
$(eval $(call SC_FIND,SYSTEMC_CHECKPOINT_SRC,P1,,P3,MSG,unique))

include $(call _MAKEQUOTE,$(KERNEL_DIR)/intc-config.mk)
include $(call _MAKEQUOTE,$(SYSTEMC_CHECKPOINT_SRC)/systemc-checkpoint.mk)

CXXFLAGS += \
  $(SYSTEMC_CHECKPOINT_CFLAGS_DEFAULT) \
  /I$(KERNEL_DIR)/src \
  $(foreach d,$(SYSTEMC_KERNEL_INTC_DEFINES),/D$(d)) \
  /DHAVE_MODULE_DATE \
  /DSC_INCLUDE_DYNAMIC_PROCESSES \
  /DWIN32 \
  /DWIN32_LEAN_AND_MEAN \
  /D_CRT_SECURE_NO_DEPRECATE \
  /D_SCL_SECURE_NO_WARNINGS \
  /vmg /vmv /EHsc /Zc:wchar_t /WX \
  /D_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING \
  /wd4834

ifeq ($(D),1)
  CXXFLAGS += /Od /MDd /Zi /FS
else
  CXXFLAGS += /Ox /MD
endif
ifneq ($(SYSTEMC_CXX_STANDARD),)
    CXXFLAGS += /std:c++$(SYSTEMC_CXX_STANDARD)
endif

.PHONY: all
all: $(CHECKPOINT_LIB)

SRCS:= \
  hierarchical_name.cc \
  iarchive.cc \
  in_memory_state.cc \
  istate_tree.cc \
  notify_callbacks.cc \
  kernel.cc \
  oarchive.cc \
  on_disk_state.cc \
  on_disk_systemc_state.cc \
  ostate_tree.cc \
  payload_update_registry.cc \
  serializer_registry.cc \
  sim_context_proxy_checkpoint.cc \
  state_keeper.cc \
  state_tree.cc

OBJS:=$(SRCS:cc=obj)

RM_OR_MOVE = $(PYTHON) $(W_SIMICS_BASE)\scripts\build\rm_or_move.py $(subst /,\,$@)
$(CHECKPOINT_LIB): $(OBJS)
	$(info Linking $@)
	$(RM_OR_MOVE)
	$(VCVARSPREFIX) lib /out:$@ $^

marker: $(addprefix $(SC_CHECKPOINT_INCLUDE)/,$(SRCS))
	$(info Compiling $(notdir $^))
	$(VCVARSPREFIX) cl /MP $(CXXFLAGS) /c $^ /Fo$(CURDIR)/
	type nul > $@

$(OBJS): marker ;
