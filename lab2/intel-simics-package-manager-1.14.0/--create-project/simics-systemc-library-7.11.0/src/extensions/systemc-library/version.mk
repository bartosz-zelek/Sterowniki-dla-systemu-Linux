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

# Version checking:
#
# 1) Kernel Version Check
#
# Extract the SystemC Kernel version and put it into a header file. This file
# is generated both when compiling packages (MP rig) and inside user's project.
#
# 2) Library Version Check
#
# Extract the Simics build-id and store it in a header file. This file is
# distributed and used when compiling from project.
#
# 3) Compile-time version checks
#
# Generate a header file with the compiler version and the kernel used when
# building the library. This file is generated in the project only, it is not
# distributed. The generated file runs static asserts when compiling the
# module to verify that the module and library is built using the same versions.

# Add defines for systemc-library-version-checks.h
include $(systemc_library_make_dir)/systemc-version.mk

#### 1) Kernel Version Check
LIB_SYSTEMC_PATH := $(TARGET_DIR)/../lib/systemc/$(SYSTEMC_KERNEL_VERSION)
VERSION_LDFLAGS := -Wl,-Bstatic -lsystemc -Wl,-Bdynamic -lpthread -static-libstdc++
VERSION_LDFLAGS += $(SYSTEMC_CORE_LDFLAGS)
VERSION_CMD = $(CXX) $(SC_CFLAGS) -L$(LIB_SYSTEMC_PATH) $(VERSION_LDFLAGS) $< -o $@
ifeq ($(WINDOWS),yes)
    ifeq ($(MSVC),yes)
        VERSION_LDFLAGS := /SUBSYSTEM:CONSOLE /LIBPATH:$(LIB_SYSTEMC_PATH) SystemC.lib
        VERSION_CMD = set LINK= & $(CL) $(SC_CFLAGS) $< /link $(VERSION_LDFLAGS)
    endif
else
    VERSION_LDFLAGS += -ldl -lrt
endif

ifneq ($(LD_LIBRARY_PATH),)
    override LD_LIBRARY_PATH := $(LD_LIBRARY_PATH):
    ifneq ($(WINDOWS),yes)
        GEN_KERNEL_LD_LIBRARY_PATH := LD_LIBRARY_PATH=$(LD_LIBRARY_PATH)
    endif
endif
ifeq ($(WINDOWS),yes)
    DISABLE_COPYRIGHT := set SYSTEMC_DISABLE_COPYRIGHT_MESSAGE=1 &
else
    DISABLE_COPYRIGHT := SYSTEMC_DISABLE_COPYRIGHT_MESSAGE=1
endif

GEN_VERSION_UTIL_CMD := $(PYTHON) $(SYSTEMC_LIBRARY_SRC)/output_kernel_version.py
sc_version_util.cc:
	$(GEN_VERSION_UTIL_CMD) > $@

sc_version_util$(EXEEXT): sc_version_util.cc
	$(VERSION_CMD)

ifneq ($(EXTERNAL_BUILD),1)
    SYSTEMC_KERNEL_VERSION_H:= $(GEN_KERNEL_SRC_PATH)/systemc-kernel-version.h
endif
$(SYSTEMC_KERNEL_VERSION_H): sc_version_util$(EXEEXT)
	$(info Generating $@)
	$(DISABLE_COPYRIGHT) $(GEN_KERNEL_LD_LIBRARY_PATH) $(INVOKE)$< > $@

#### 3) Compile-time version checks
VERSION_CHECKS_H = $(TARGET_DIR)/systemc/$(SYSTEMC_KERNEL_VERSION)/systemc-library-version-checks.h
$(VERSION_CHECKS_H): generate_version_checks$(EXEEXT)
	$(info Generating $@)
	$(INVOKE)$< > $@

generate_version_checks$(EXEEXT): generate_version_checks.cc
ifeq ($(MSVC),yes)
	$(VERSION_CMD)
else
	$(CXX) $(SC_CFLAGS) $(SCL_VERSION_DEFINES) -DSC_DISABLE_API_VERSION_CHECK -static-libstdc++ $< -o $@
endif

#### *) dependencies. Add all generated headers to GEN_HEADERS

GEN_HEADERS:=$(SYSTEMC_KERNEL_VERSION_H) $(VERSION_CHECKS_H)
