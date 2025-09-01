# -*- Makefile -*-

# © 2015 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

# Makefile for building systemc kernel.

# Setup compiler preference
include $(MODULE_MAKEFILE:module.mk=systemc-find.mk)
unique := /compiler.mk
P1 := modules/systemc-library$(unique)
P3 := src/extensions/systemc-library$(unique)
MSG := Unable to locate: SystemC Library
$(eval $(call SC_FIND,_systemc_library,P1,,P3,MSG,unique))
include $(_systemc_library)/compiler.mk

ifndef SYSTEMC_KERNEL_VERSION
    $(error Variable SYSTEMC_KERNEL_VERSION is not defined)
else
    ifeq ($(SYSTEMC_KERNEL_VERSION), 2.3.3)
        $(info SYSTEMC_KERNEL_VERSION=2.3.3)
    else ifeq ($(SYSTEMC_KERNEL_VERSION), 2.3.4)
        $(info SYSTEMC_KERNEL_VERSION=2.3.4)
    else ifeq ($(SYSTEMC_KERNEL_VERSION), 3.0.0)
        $(info SYSTEMC_KERNEL_VERSION=3.0.0)
    else
        $(error Variable SYSTEMC_KERNEL_VERSION must be either '2.3.3', '2.3.4' or '3.0.0', was: $(SYSTEMC_KERNEL_VERSION))
    endif
endif

# Header file containing static assert of compiler and kernel version
COMPILER_VERSION_CHECK_H = $(TARGET_DIR)/systemc/$(SYSTEMC_KERNEL_VERSION)/systemc-kernel-compiler-version-check.h

# NOTE: must put 'all:' here on top, as setup.mk also defines make targets
# The build_and_copy rule is compiler specific and defined by gcc.mk or msvc.mk
.PHONY: all
all: build_and_copy $(COMPILER_VERSION_CHECK_H) ;

# Setup Makefiles and configure script for non-project builds
kernel_make_dir := $(SRC_BASE)/$(TARGET)
ifneq ($(_CORE_PROJECT_BUILD),)
    ifeq ($(MSVC),no)
        include $(kernel_make_dir)/setup.mk
    else
.PHONY: configure-$(SYSTEMC_KERNEL_VERSION)
configure-$(SYSTEMC_KERNEL_VERSION): ;
    endif
else
.PHONY: configure-$(SYSTEMC_KERNEL_VERSION)
configure-$(SYSTEMC_KERNEL_VERSION): ;
endif

# Setup directories used when building.
unique := /intc-config.mk
P1 := external/systemc/kernel-$(SYSTEMC_KERNEL_VERSION)/$(unique)
P3 := src/external/systemc/kernel-$(SYSTEMC_KERNEL_VERSION)/$(unique)
MSG := Unable to locate: SystemC Kernel
$(eval $(call SC_FIND,kernel_src_dir,P1,,P3,MSG,unique))
install_dir := $(TARGET_DIR)/systemc/$(SYSTEMC_KERNEL_VERSION)

$(kernel_src_dir)/intc-config.mk: configure-$(SYSTEMC_KERNEL_VERSION) ;

# Because we don't include MODULE_MAKEFILE we need to setup host specific rules here
ifeq ($(WINDOWS),no)
    _CP:=cp
    W_PATH=$(1)
    INVOKE := ./
else
    _CP:=copy /Y
    W_PATH=$(subst /,\,$(1))
    export SHELL:=cmd.exe
    EXEEXT := .exe
    install_dir := $(call W_PATH,$(install_dir))
endif
intc-config.mk: $(kernel_src_dir)/intc-config.mk
	$(info Making kernel intc-config.mk)
	$(_CP) $(call W_PATH,$<) $@

gcc.mk: $(kernel_make_dir)/gcc.mk intc-config.mk
	$(info Making gcc.mk)
	$(_CP) $(call W_PATH,$<) $@

msvc.mk: $(kernel_make_dir)/msvc.mk intc-config.mk
	$(info Making msvc.mk)
	$(_CP) $(call W_PATH,$<) $@

version.mk: $(kernel_make_dir)/version.mk intc-config.mk
	$(info Making version.mk)
	$(_CP) $(call W_PATH,$<) $@

ifeq ($(WINDOWS),no)
    ifeq ($(_CORE_PROJECT_BUILD),yes)
        INTC_EXT_VALGRIND = 1
    endif
    ifneq ($(SYSTEMC_VALGRIND_CFLAGS),)
        INTC_EXT_VALGRIND = 1
    endif
    ifeq ($(INTC_EXT_VALGRIND),1)
        SYSTEMC_VALGRIND_CFLAGS ?= $(VALGRIND_INCLUDE)
    endif
endif

# Include build configuration makefile that controls default build options
# for Intel specific extensions, contained in SYSTEMC_KERNEL_INTC_DEFINES.
-include intc-config.mk

# Host/compiler specific makefiles.
ifeq ($(WINDOWS),yes)
    ifeq ($(MSVC),yes)
        -include msvc.mk
    else
        -include gcc.mk
    endif
else
    -include gcc.mk
endif

-include version.mk
