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

# Makefile for building SystemC kernel on Windows with Visual Studio

# Setup INTC_EXT defines from intc-config.mk.
# NOTE: "$() " is a SPACE. Only touch it if you know a better
# encoding of SPACE that works with recent GNU Make
defines := $(subst $() ,;,$(SYSTEMC_KERNEL_INTC_DEFINES))

# Export variables that are expected in MSVC Projects.
export BUILD_DIR = $(call W_PATH,$(CURDIR))
export CMD_LINE_DEFINES=$(defines)
export DisableSpecificWarnings=4838

ifneq ($(MSVC_VERSION),)
    solutions_file = \
        $(call W_PATH,$(kernel_src_dir)/$(MSVC_VERSION)/SystemC/SystemC.sln)
else
    ifeq ($(VisualStudioVersion),14.0)
        solutions_file = \
	    $(call W_PATH,$(kernel_src_dir)/msvc140/SystemC/SystemC.sln)
    else
        ifeq ($(VisualStudioVersion),17.0)
            solutions_file = \
	        $(call W_PATH,$(kernel_src_dir)/msvc170/SystemC/SystemC.sln)
        else
            $(error Unsupported Visual Studio version: $(VisualStudioVersion))
        endif
    endif
endif

log_file = $(call W_PATH,$(CURDIR))/build.log
msbuild_cmd := $(VCVARSPREFIX) msbuild $(solutions_file) \
               /m /fl /flp:logfile=$(log_file)
ifeq ($(D),1)
    msbuild_cmd += /p:Configuration=Debug /p:Platform=x64 /t:Build
else
    msbuild_cmd += /p:Configuration=Release /p:Platform=x64 /t:Build
endif
ifneq ($(SYSTEMC_CXX_STANDARD),)
    msbuild_cmd += /p:CppLanguageStandard=stdcpp$(SYSTEMC_CXX_STANDARD)
endif

ifeq ($(findstring win64,$(HOST_TYPE)),win64)
    arch := x64
else
    arch := Win32
endif

ifeq ($(D),1)
    solutions_build_dir := $(call W_PATH,$(arch)/Debug)
else
    solutions_build_dir := $(call W_PATH,$(arch)/Release)
endif

# Files to build and copy to install_dir
built_files = $(wildcard $(solutions_build_dir)/*.lib) \
              $(wildcard $(solutions_build_dir)/*/*.pdb)

# Rules
.PHONY: build_and_copy
build_and_copy: copy
	$(info Files have been copied to installation directory.)

.PHONY: copy
copy: build | $(install_dir)
	$(info Copying files to installation directory.)
	$(if $(strip $(built_files)),,\
	    $(error No files in $(solutions_build_dir)))
	$(foreach f,$(built_files),\
	    copy /y $(call W_PATH,$(f)) $(install_dir) &)

.PHONY: build
build:
	$(msbuild_cmd)

$(install_dir):
	-mkdir -p $@
