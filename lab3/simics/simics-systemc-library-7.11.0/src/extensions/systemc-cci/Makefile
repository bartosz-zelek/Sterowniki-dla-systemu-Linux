# -*- Makefile -*-

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

# Makefile for building systemc CCI

# Setup compiler preference
include $(MODULE_MAKEFILE:module.mk=systemc-find.mk)
unique := /compiler.mk
P1 := modules/systemc-library$(unique)
P3 := src/extensions/systemc-library$(unique)
MSG := Unable to locate: SystemC Library
$(eval $(call SC_FIND,_systemc_library,P1,,P3,MSG,unique))
include $(_systemc_library)/compiler.mk

# The copy_and_build rule is compiler specific and defined by gcc.mk or msvc.mk
.PHONY: all
all: build_and_copy ;

# Setup directories used when building.
# Setup lookup-file.
ifeq ($(WINDOWS),yes)
    _lookup_file=$(SIMICS_PROJECT)\bin\lookup-file.bat -s
else
    _lookup_file=$(SIMICS_PROJECT)/bin/lookup-file -s
endif

include $(MODULE_MAKEFILE:module.mk=systemc-find.mk)

install_dir := $(TARGET_DIR)/systemc/$(SYSTEMC_KERNEL_VERSION)

# Setup CCI source dir.
unique := /src/cci_configuration
P1 := external/systemc/cci$(unique)
P3 := src/external/systemc/cci$(unique)
MSG := Unable to locate: CCI
$(eval $(call SC_FIND,cci_src_dir,P1,,P3,MSG,unique))

# Setup SystemC kernel source dir.
unique := /systemc.h
P1 := external/systemc/kernel-$(SYSTEMC_KERNEL_VERSION)/src$(unique)
P3 := src/external/systemc/kernel-$(SYSTEMC_KERNEL_VERSION)/src$(unique)
MSG := Unable to locate: SystemC Kernel
$(eval $(call SC_FIND,kernel_src_dir,P1,,P3,MSG,unique))

# Host/compiler specific makefiles.
MODULE_DIR:=$(SRC_BASE)/$(TARGET)
ifeq ($(MSVC),yes)
    export SHELL=cmd.exe
    include $(MODULE_DIR)/msvc.mk
else
    include $(MODULE_DIR)/gcc.mk
endif
