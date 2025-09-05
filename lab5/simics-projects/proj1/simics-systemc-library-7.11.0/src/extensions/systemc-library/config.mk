# © 2019 Intel Corporation
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
# Generic configuration for all hosts and compilers
#

# Find out if host type is windows and which compiler to use
WINDOWS := $(if $(findstring win,$(HOST_TYPE)),yes,no)
IFEQ = $(and $(findstring $(1),$(2)),$(findstring $(2),$(1)))
MSVC := $(if $(call IFEQ,$(CXX),cl),yes,no)

# Setup paths
include $(MODULE_MAKEFILE:module.mk=systemc-find.mk)

# Setup SB_INCLUDE
NON_ESSENTIAL_PATTERN := src/include/simics/base/memory.h
P1 := $(NON_ESSENTIAL_PATTERN)
P3 := $(NON_ESSENTIAL_PATTERN)
MSG := Unable to locate: Simics Base
$(eval $(call SC_FIND,SB_PATH,P1,,P3,MSG,NON_ESSENTIAL_PATTERN))
SB_INCLUDE := $(SB_PATH)/src/include

# Setup CPP_API_INCLUDE
unique := /conf-class.cc
P1 := modules/c++-api$(unique)
P3 := src/devices/c++-api$(unique)
MSG := Unable to locate: Simics C++ API
$(eval $(call SC_FIND,CPP_API_INCLUDE,P1,,P3,MSG,unique))

# Setup SC_KERNEL_INCLUDE
ifeq ($(SYSTEMC_CORE_CFLAGS),)
    unique := /systemc.h
    P1 := external/systemc/kernel-$(SYSTEMC_KERNEL_VERSION)/src$(unique)
    P3 := src/external/systemc/kernel-$(SYSTEMC_KERNEL_VERSION)/src$(unique)
    MSG := Unable to locate: SystemC Kernel
    $(eval $(call SC_FIND,SC_KERNEL_INCLUDE,P1,,P3,MSG,unique))
else
    SC_KERNEL_INCLUDE := ./not/used
endif

# Setup version headers. Kernel is always the project, while library is
# always the one distributed by the package
GEN_KERNEL_SRC_PATH := $(SIMICS_PROJECT)/$(HOST_TYPE)/obj/modules/systemc-library

# Setup SC_CCI_INCLUDE
unique := /cci_configuration
P1 := external/systemc/cci/src$(unique)
P3 := src/external/systemc/cci/src$(unique)
MSG := Unable to locate: CCI
$(eval $(call SC_FIND,SC_CCI_INCLUDE,P1,,P3,MSG,unique))

# Setup SC_INTERFACES_INCLUDE
unique := /systemc-interfaces.h
P1 := modules/systemc-interfaces$(unique)
P3 := src/extensions/systemc-interfaces$(unique)
MSG := Unable to locate: systemc-interfaces.
$(eval $(call SC_FIND,SC_INTERFACES_INCLUDE,P1,,P3,MSG,unique))

# Find source path for systemc-checkpoint
unique:=/systemc-checkpoint.mk
P1:=modules/systemc-checkpoint$(unique)
P3:=src/extensions/systemc-checkpoint$(unique)
MSG:=Unable to locate: SystemC Checkpoint
$(eval $(call SC_FIND,SYSTEMC_CHECKPOINT_SRC,P1,,P3,MSG,unique))

# Setup defines for intel systemc-kernel extensions.
# Note that these flags must match between systemc-library and systemc-kernel,
# so if another set of flags are needed, both systemc-kernel and systemc-library
# must be rebuilt.
# If SYSTEMC_CORE_CFLAGS has been set, this is interpreted an attempt to build
# a custom systemc kernel, and the Intel systemc-kernel flags are not
# incorporated in the build automatically.
ifeq ($(SYSTEMC_CORE_CFLAGS),)
    intc_config := $(wildcard $(SC_KERNEL_INCLUDE)/../intc-config.mk)
    ifeq ($(intc_config),)
        msg := Missing intc-config.mk. Please report this.
        $(error $(msg))
    else
        include $(intc_config)
        intc_cflags := $(foreach d,$(SYSTEMC_KERNEL_INTC_DEFINES),-D$(d))
    endif
endif

# Generic Compiler flags
SC_CFLAGS :=
SC_CFLAGS += -DSC_INCLUDE_DYNAMIC_PROCESSES
SC_CFLAGS += $(intc_cflags)
ifneq ($(SYSTEMC_CORE_CFLAGS),)
    SC_CFLAGS += $(SYSTEMC_CORE_CFLAGS)
endif
ifneq ($(USE_SIMICS_CCI),no)
    SC_CFLAGS += -DUSE_SIMICS_CCI
endif

ifeq ($(EXTERNAL_BUILD),1)
    SC_CFLAGS += -DEXTERNAL_BUILD
endif
ifeq ($(SC_ELAB_AND_SIM_DISABLE),1)
    SC_CFLAGS += -DSC_ELAB_AND_SIM_DISABLE
endif
SC_CFLAGS += -I$(SB_INCLUDE)
SC_CFLAGS += -I$(CPP_API_INCLUDE)
SC_CFLAGS += -I$(SC_KERNEL_INCLUDE)
SC_CFLAGS += -I$(SYSTEMC_LIBRARY_SRC)
SC_CFLAGS += -I$(GEN_KERNEL_SRC_PATH)
SC_CFLAGS += -I$(TARGET_DIR)/systemc/$(SYSTEMC_KERNEL_VERSION)
SC_CFLAGS += -I$(SC_INTERFACES_INCLUDE)
SC_CFLAGS += -I$(SC_CCI_INCLUDE)
ifneq ($(INTC_EXT),0)
    ifneq ($(_CORE_PROJECT_BUILD),)
        include $(SYSTEMC_CHECKPOINT_SRC)/systemc-checkpoint.mk
        SC_CFLAGS += $(SYSTEMC_CHECKPOINT_CFLAGS_DEFAULT)
    else
        ifeq ($(USE_SIMICS_CHECKPOINTING),yes)
            include $(SYSTEMC_CHECKPOINT_SRC)/systemc-checkpoint.mk
            SC_CFLAGS += $(SYSTEMC_CHECKPOINT_CFLAGS_DEFAULT)
        endif
    endif
endif

# Convert all flags to Visual Studio format
ifeq ($(MSVC),yes)
    SC_CFLAGS := $(patsubst -D%,/D%,$(SC_CFLAGS))
    SC_CFLAGS := $(patsubst -I%,/I%,$(SC_CFLAGS))
endif

# TODO(ah): until we have written a new PCIe gasket that can be utilized in
#           Simics 7 we need to build the library with obsolete API
SC_CFLAGS += -DSHOW_OBSOLETE_API
