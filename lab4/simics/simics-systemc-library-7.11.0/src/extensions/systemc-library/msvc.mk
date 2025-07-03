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

SC_CFLAGS += /DWIN32_LEAN_AND_MEAN
SC_CFLAGS += /DHAVE_MODULE_DATE
SC_CFLAGS += /DWIN32 /D_SCL_SECURE_NO_WARNINGS
SC_CFLAGS += /D_CRT_SECURE_NO_DEPRECATE
SC_CFLAGS += /wd4996
SC_CFLAGS += /wd4005
SC_CFLAGS += /wd4819
SC_CFLAGS += /GR /vmg /vmv
SC_CFLAGS += /nologo /EHsc /Zc:wchar_t /WX /Gd
# VS 2022 use C++14 by default and filesystem is experimental
SC_CFLAGS += /D_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
ifneq ($(SYSTEMC_CXX_STANDARD),)
    SC_CFLAGS += /std:c++$(SYSTEMC_CXX_STANDARD) /D_ENFORCE_MATCHING_ALLOCATORS=0
endif

# TODO(ah): until we have written a new PCIe gasket that can be utilized in
#           Simics 7 we need to build the library with obsolete API
SC_CFLAGS += /DSHOW_OBSOLETE_API

# Add defines for static asserts of systemc-library-version-checks.h
SC_CFLAGS += $(SCL_VERSION_DEFINES)

# Add correct runtime library, DLL (shared) with/without debug
ifeq ($(D),1)
    SC_CFLAGS += /Od /MDd /Zi /FS
else
    SC_CFLAGS += /Ox /MD
endif

# Compiler command
# NOTE: Visual Studio is passed _all_ files at once and linker must find
# these files by custom rules
CL := $(VCVARSPREFIX) cl
SC_COMPILE_CMD = $(CL) $(SC_CFLAGS) /c /MP $^
SC_ARCHIVE_CMD = $(VCVARSPREFIX) lib /out:$@

SC_LIBRARY := systemc_library.lib
OBJEXT := obj
EXEEXT := .exe

export SHELL := cmd.exe

# build it all with single invocation of 'cl'
SIMICS2TLM_SRCS:=$(subst $(SIMICS2TLM_PREF),,$(SIMICS2TLM_OBJS:obj=cc))
mdir1:
	mkdir $@
marker1: $(addprefix $(SYSTEMC_LIBRARY_SRC)/simics2tlm/,$(SIMICS2TLM_SRCS)) | mdir1 $(GEN_HEADERS)
	$(SC_COMPILE) /Fomdir1/
	type nul > $@

$(SIMICS2TLM_OBJS): marker1 ;

TLM2SIMICS_SRCS:=$(subst $(TLM2SIMICS_PREF),,$(TLM2SIMICS_OBJS:obj=cc))
mdir2:
	mkdir $@
marker2: $(addprefix $(SYSTEMC_LIBRARY_SRC)/tlm2simics/,$(TLM2SIMICS_SRCS)) | mdir2 $(GEN_HEADERS)
	$(SC_COMPILE) /Fomdir2/
	type nul > $@

$(TLM2SIMICS_OBJS): marker2 ;

TOOLS_SRCS:=$(subst $(TOOLS_PREF),,$(TOOLS_OBJS:obj=cc))
mdir3:
	mkdir $@
marker3: $(addprefix $(SYSTEMC_LIBRARY_SRC)/tools/,$(TOOLS_SRCS)) | mdir3 $(GEN_HEADERS)
	$(SC_COMPILE) /Fomdir3/
	type nul > $@

$(TOOLS_OBJS): marker3 ;

SYSTEMC2SIMICS_SRCS:=$(subst $(SYSTEMC2SIMICS_PREF),,$(SYSTEMC2SIMICS_OBJS:obj=cc))
mdir4:
	mkdir $@
marker4: $(addprefix $(SYSTEMC_LIBRARY_SRC)/systemc2simics/,$(SYSTEMC2SIMICS_SRCS)) | mdir4 $(GEN_HEADERS)
	$(SC_COMPILE) /Fomdir4/
	type nul > $@

$(SYSTEMC2SIMICS_OBJS): marker4 ;

SIMICS2SYSTEMC_SRCS:=$(subst $(SIMICS2SYSTEMC_PREF),,$(SIMICS2SYSTEMC_OBJS:obj=cc))
mdir5:
	mkdir $@
marker5: $(addprefix $(SYSTEMC_LIBRARY_SRC)/simics2systemc/,$(SIMICS2SYSTEMC_SRCS)) | mdir5 $(GEN_HEADERS)
	$(SC_COMPILE) /Fomdir5/
	type nul > $@

$(SIMICS2SYSTEMC_OBJS): marker5 ;

AWARENESS_SRCS:=$(subst $(AWARENESS_PREF),,$(AWARENESS_OBJS:obj=cc))
mdir6:
	mkdir $@
marker6: $(addprefix $(SYSTEMC_LIBRARY_SRC)/awareness/,$(AWARENESS_SRCS)) | mdir6 $(GEN_HEADERS)
	$(SC_COMPILE) /Fomdir6/
	type nul > $@

$(AWARENESS_OBJS): marker6 ;

INJECTION_SRCS:=$(subst $(INJECTION_PREF),,$(INJECTION_OBJS:obj=cc))
mdir7:
	mkdir $@
marker7: $(addprefix $(SYSTEMC_LIBRARY_SRC)/injection/,$(INJECTION_SRCS)) | mdir7 $(GEN_HEADERS)
	$(SC_COMPILE) /Fomdir7/
	type nul > $@

$(INJECTION_OBJS): marker7 ;

INSTRUMENTATION_SRCS:=$(subst $(INSTRUMENTATION_PREF),,$(INSTRUMENTATION_OBJS:obj=cc))
mdir8:
	mkdir $@
marker8: $(addprefix $(SYSTEMC_LIBRARY_SRC)/instrumentation/,$(INSTRUMENTATION_SRCS)) | mdir8 $(GEN_HEADERS)
	$(SC_COMPILE) /Fomdir8/
	type nul > $@

$(INSTRUMENTATION_OBJS): marker8 ;

COMMON_SRCS:=$(COMMON_OBJS:obj=cc)
mdir9:
	mkdir $@
marker9: $(addprefix $(SYSTEMC_LIBRARY_SRC)/,$(COMMON_SRCS)) | mdir9 $(GEN_HEADERS)
	$(SC_COMPILE) /Fomdir9/
	type nul > $@

$(COMMON_OBJS): marker9 ;

