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

# Makefile for building SystemC CCI on Windows with Visual Studio

# Use correct slashes in install_dir.
W_PATH=$(subst /,\,$(1))
install_dir := $(call W_PATH,$(install_dir))

# Export variables that are expected in MSVC Projects.
export BUILD_DIR = $(call W_PATH,$(CURDIR))
export SYSTEMC_HOME = $(call W_PATH,$(kernel_src_dir))
export BOOST_HOME = $(call W_PATH,$(BOOST_INC_PATH))
export CCI_HOME = $(call W_PATH,$(cci_src_dir))

ifneq ($(MSVC_VERSION),)
    solutions_file = $(call W_PATH,$(cci_src_dir)/$(MSVC_VERSION)/cci/cci.sln)
else
    ifeq ($(VisualStudioVersion),14.0)
        solutions_file = $(call W_PATH,$(cci_src_dir)/msvc140/cci/cci.sln)
    else
        ifeq ($(VisualStudioVersion),17.0)
            solutions_file = $(call W_PATH,$(cci_src_dir)/msvc170/cci/cci.sln)
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

arch := x64
ifeq ($(D),1)
    solutions_build_dir := $(call W_PATH,$(arch)/Debug)
else
    solutions_build_dir := $(call W_PATH,$(arch)/Release)
endif

# Files to build and copy to install_dir
built_files = $(wildcard $(solutions_build_dir)/*.lib)

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
	-mkdir $@
