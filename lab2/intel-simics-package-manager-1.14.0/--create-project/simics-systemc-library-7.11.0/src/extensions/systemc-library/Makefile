# -*- Makefile -*-

# © 2010 Intel Corporation
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
# Makefile for building SystemC Library.
#

.PHONY: all
all: lib

# Setup compiler preference
include $(MODULE_MAKEFILE:module.mk=systemc-find.mk)
unique := /compiler.mk
P1 := modules/systemc-library$(unique)
P3 := src/extensions/systemc-library$(unique)
MSG := Unable to locate: SystemC Library
$(eval $(call SC_FIND,_systemc_library,P1,,P3,MSG,unique))
include $(_systemc_library)/compiler.mk

# Setup SYSTEMC_LIBRARY_SRC (used by modules)
SYSTEMC_LIBRARY_SRC := $(SRC_BASE)/$(TARGET)
# Extra indirection to keep the same look-n-feel as other systemc-* Makefiles
systemc_library_make_dir = $(SYSTEMC_LIBRARY_SRC)

# Bring in _all_ the external .mk files here, except the compiler specific ones below
include $(systemc_library_make_dir)/config.mk
include $(systemc_library_make_dir)/objects.mk
include $(systemc_library_make_dir)/version.mk
include $(systemc_library_make_dir)/misc.mk
# Add all .mk to the list of makefiles so things are properly re-built when updated
_MAKEFILES:=$(wildcard $(systemc_library_make_dir)/*.mk)

# Because we don't include MODULE_MAKEFILE we need to setup host specific rules here
ifeq ($(WINDOWS),no)
    _CP:=cp
    W_PATH=$(1)
else
    _CP:=copy /Y
    W_PATH=$(subst /,\,$(1))
    export SHELL:=cmd.exe
endif

gcc.mk: $(systemc_library_make_dir)/gcc.mk
	$(info Making gcc.mk)
	$(_CP) $(call W_PATH,$<) $@

msvc.mk: $(systemc_library_make_dir)/msvc.mk
	$(info Making msvc.mk)
	$(_CP) $(call W_PATH,$<) $@

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

# Objects, vpath and rules
VPATH += $(SYSTEMC_LIBRARY_SRC)

.PHONY : sanity_checks
sanity_checks:
ifeq ($(WINDOWS),no)
	grep -e "SIM_log_\(info\|spec\|unimp\|error\|critical\)" \
             -rn $(SYSTEMC_LIBRARY_SRC)/* | grep -v unittest && exit 1 || true
	grep -Ee ' \.[a-zA-Z0-9_]+ *=' -rn $(SYSTEMC_LIBRARY_SRC)/* && exit 1 || true
endif

.PHONY: unittest
unittest: $(TARGET_DIR)/systemc/$(SYSTEMC_KERNEL_VERSION)/$(SC_LIBRARY)
	$(MAKE) -f $(SYSTEMC_LIBRARY_SRC)/unittest/Makefile unittest

.PHONY: unittest-clean
unittest-clean:
	$(MAKE) -f $(SYSTEMC_LIBRARY_SRC)/unittest/Makefile unittest-clean

.PHONY: integration
integration: $(TARGET_DIR)/systemc/$(SYSTEMC_KERNEL_VERSION)/$(SC_LIBRARY)
	$(MAKE) -f $(SYSTEMC_LIBRARY_SRC)/integration/Makefile integration

.PHONY: integration-clean
integration-clean:
	$(MAKE) -f $(SYSTEMC_LIBRARY_SRC)/integration/Makefile integration-clean

# import useful macros from module.mk.win32
# NOTE: this only works from a project, where W_SIMICS_BASE
#       has been set. In the build-tree, we have cygwin and can do rm
RM_OR_MOVE=rm -f $@
ifeq ($(WINDOWS),yes)
    W_PATH = $(subst /,\,$(1))
    WDST=$(call W_PATH,$@)
    RM_OR_MOVE=$(PYTHON) $(W_SIMICS_BASE)\scripts\build\rm_or_move.py
endif

.PHONY: lib
lib: sanity_checks $(TARGET_DIR)/systemc/$(SYSTEMC_KERNEL_VERSION)/$(SC_LIBRARY) $(unittest_rule)

# Any .cc could include adapter.h which includes the generated header
$(SC_LIBRARY_OBJS) : $(GEN_HEADERS)

# TODO(ah): uggly! but with CMake we can make it pretty again
ifeq ($(MSVC),yes)
MDIRS=mdir1 mdir2 mdir3 mdir4 mdir5 mdir6 mdir7 mdir8 mdir9
$(TARGET_DIR)/systemc/$(SYSTEMC_KERNEL_VERSION)/$(SC_LIBRARY): $(SC_LIBRARY_OBJS)
	$(RM_OR_MOVE) $(WDST)
	$(SC_ARCHIVE) $(foreach d,$(MDIRS),$(wildcard $(d)/*))
else
$(TARGET_DIR)/systemc/$(SYSTEMC_KERNEL_VERSION)/$(SC_LIBRARY): $(SC_LIBRARY_OBJS)
	$(RM_OR_MOVE) $(WDST)
	$(SC_ARCHIVE)
endif

# exports needed for unittests
export BOOST_INC_PATH
export BOOST_LIB_PATH
export CFLAGS_NO_UNUSED_LOCAL_TYPEDEFS
export CXX
export CXXFLAGS
export INTC_EXT
export SC_CFLAGS
export SYSTEMC_LIBRARY_SRC
export SYSTEMC_KERNEL_VERSION
export SYSTEMC_CXX_STANDARD
