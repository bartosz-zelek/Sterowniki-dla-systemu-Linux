# © 2018 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

# NOTE: This setup is used by all Makefile.standalone files built by the
# internal systemc-standalone target which is a linux-only target

HOST_DIR ?= $(SIMICS_PROJECT)/$(HOST_TYPE)
SYSTEMC_KERNEL_VERSION ?= 2.3.4

# We need SC_FIND to have been setup for searching for files and directories.
ifeq ($(SC_FIND),)
    include $(shell $(SIMICS_PROJECT)/bin/lookup-file -f config/project/systemc-find.mk)
endif

# When building inside Simics, we don't allow using a different
# SystemC kernel. Build in a project and set SYSTEMC_CORE_CFLAGS etc
# to use a different kernel.
# Setup directories used when building.
unique := /intc-config.mk
P1 := external/systemc/kernel-$(SYSTEMC_KERNEL_VERSION)$(unique)
P3 := src/external/systemc/kernel-$(SYSTEMC_KERNEL_VERSION)$(unique)
MSG := Unable to locate: SystemC Kernel
$(eval $(call SC_FIND,kernel_src_dir,P1,,P3,MSG,unique))
SYSTEMC_CORE_CFLAGS_DEFAULT += -I$(kernel_src_dir)/src

# Build with intel extensions.
include $(kernel_src_dir)/intc-config.mk
intc_cflags := $(foreach d,$(SYSTEMC_KERNEL_INTC_DEFINES),-D$(d))
SYSTEMC_CORE_CFLAGS_DEFAULT += $(intc_cflags)

# No warning flags.
SYSTEMC_MODULE_CFLAGS_DEFAULT += -Wno-unused-variable
SYSTEMC_MODULE_CFLAGS_DEFAULT += -Wno-undef
SYSTEMC_MODULE_CFLAGS_DEFAULT += -Wdeprecated-declarations

# Adding SystemC kernel
SYSTEMC_LIB_DIR = $(HOST_DIR)/lib/systemc/$(SYSTEMC_KERNEL_VERSION)
SYSTEMC_CORE_LDFLAGS_DEFAULT += $(SYSTEMC_LIB_DIR)/libsystemc.a

# Time Library and QuickThreads support
SYSTEMC_CORE_LDFLAGS_DEFAULT += -lrt -ldl

# Add default flags to the actually documented flag variables.
# If the outside makefile has set any of the flags, this indicates that this
# makefile assumes complete control of the flags, so the default flags won't
# be added.
ifeq ($(SYSTEMC_CORE_CFLAGS),)
    SYSTEMC_CORE_CFLAGS := $(SYSTEMC_CORE_CFLAGS_DEFAULT)
endif
ifeq ($(SYSTEMC_MODULE_CFLAGS),)
    SYSTEMC_MODULE_CFLAGS := $(SYSTEMC_MODULE_CFLAGS_DEFAULT)
endif
ifeq ($(SYSTEMC_CORE_LDFLAGS),)
    SYSTEMC_CORE_LDFLAGS := $(SYSTEMC_CORE_LDFLAGS_DEFAULT)
endif
