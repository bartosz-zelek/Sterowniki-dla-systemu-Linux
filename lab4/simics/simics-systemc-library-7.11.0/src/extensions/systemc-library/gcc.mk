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

SC_CFLAGS += -Wno-undef -Wvla
SC_CFLAGS += -Wno-unused-variable  # Boost & GCC 5.x
ifneq ($(SYSTEMC_CXX_STANDARD),)
    SC_CFLAGS += -std=gnu++$(SYSTEMC_CXX_STANDARD)  # need GNU extensions to support variadic macros used by Simics
endif

# Extra MinGW flag needed on windows (produces warning if not used)
ifeq ($(WINDOWS),yes)
    SC_CFLAGS += -DWIN32_LEAN_AND_MEAN
endif

# Add defines for static asserts of systemc-library-version-checks.h
SC_CFLAGS += $(SCL_VERSION_DEFINES)

CXXFLAGS := $(filter-out -DNDEBUG,$(CXXFLAGS))
DEP_CXXFLAGS := $(filter-out -DNDEBUG,$(DEP_CXXFLAGS))
SC_CFLAGS := $(filter-out -DNDEBUG,$(SC_CFLAGS))
BLD_CXXFLAGS := $(filter-out -DNDEBUG,$(BLD_CXXFLAGS))

# Compiler and library archiver commands.
# CXXFLAGS is set from config/host-flags.common (build tree)
# BLD_CXXFLAGS is set from config/project/config.mk (project)
SC_COMPILE_CMD = $(CXX) $(CXXFLAGS) $(BLD_CXXFLAGS) $(SC_CFLAGS) -c $< -o $@
SC_DEP_CMD = $(DEP_CXX) $< $(DEP_CXXFLAGS) $(DEP_CFLAGS_TARGETS) \
$(SC_CFLAGS) -MF $@ -MT $(patsubst %.d,%.$(OBJEXT),$@)
AR ?= ar
SC_ARCHIVE_CMD = $(AR) rcs $@ $^

SC_LIBRARY := libsystemc-library.a
OBJEXT := o
INVOKE := ./

# Simics modules can be built using a much wider range of compiler versions,
# while SystemC modules are typically much more limited due to a lot of 3rd
# party IP and external libraries. Make it possible for the user to run with a
# custom version for SystemC if needed. This is used internally by Simics team
# in Stockholm.
ifneq ($(SYSTEMC_CXX),)
CXX = $(SYSTEMC_CXX)
DEP_CXX = $(SYSTEMC_CXX)
endif

# See SC_CFLAGS above for adding -std=gnu++XX

ifeq ($(INTC_EXT),0)
SYSTEMC_NO_UNITTESTS := 1
endif

ifneq ($(SYSTEMC_NO_UNITTESTS),1)
    ifneq ($(EXTERNAL_BUILD),1)
        # Run unittest and integration only on linux64 and when build inside Simics.
        unittest_rule := $(if $(call IFEQ,$(HOST_TYPE),linux64),unittest integration)
        # Make sure we setup boost, required by unittests
        include $(SYSTEMC_LIBRARY_SRC)/systemc-boost.mk
    endif
endif

# Dependency rules (does not work on windows with MSVC compiler)
# (taken from module.mk, so these should work in project too.
depfiles=$(strip $(SC_LIBRARY_OBJS:.$(OBJEXT)=.d))
-include $(depfiles)

$(SIMICS2TLM_PREF)%.$(OBJEXT) : $(SYSTEMC_LIBRARY_SRC)/simics2tlm/%.cc $(_MAKEFILES)
	$(SC_COMPILE)
$(SIMICS2TLM_PREF)%.d : $(SYSTEMC_LIBRARY_SRC)/simics2tlm/%.cc $(GEN_HEADERS)
	$(SC_DEP)

$(TLM2SIMICS_PREF)%.$(OBJEXT) : $(SYSTEMC_LIBRARY_SRC)/tlm2simics/%.cc $(_MAKEFILES)
	$(SC_COMPILE)
$(TLM2SIMICS_PREF)%.d : $(SYSTEMC_LIBRARY_SRC)/tlm2simics/%.cc $(GEN_HEADERS)
	$(SC_DEP)

$(TOOLS_PREF)%.$(OBJEXT) : $(SYSTEMC_LIBRARY_SRC)/tools/%.cc $(_MAKEFILES)
	$(SC_COMPILE)
$(TOOLS_PREF)%.d : $(SYSTEMC_LIBRARY_SRC)/tools/%.cc $(GEN_HEADERS)
	$(SC_DEP)

$(SYSTEMC2SIMICS_PREF)%.$(OBJEXT) : $(SYSTEMC_LIBRARY_SRC)/systemc2simics/%.cc $(_MAKEFILES)
	$(SC_COMPILE)
$(SYSTEMC2SIMICS_PREF)%.d : $(SYSTEMC_LIBRARY_SRC)/systemc2simics/%.cc $(GEN_HEADERS)
	$(SC_DEP)

$(SIMICS2SYSTEMC_PREF)%.$(OBJEXT) : $(SYSTEMC_LIBRARY_SRC)/simics2systemc/%.cc $(_MAKEFILES)
	$(SC_COMPILE)
$(SIMICS2SYSTEMC_PREF)%.d : $(SYSTEMC_LIBRARY_SRC)/simics2systemc/%.cc $(GEN_HEADERS)
	$(SC_DEP)

$(AWARENESS_PREF)%.$(OBJEXT) : $(SYSTEMC_LIBRARY_SRC)/awareness/%.cc $(_MAKEFILES)
	$(SC_COMPILE)
$(AWARENESS_PREF)%.d : $(SYSTEMC_LIBRARY_SRC)/awareness/%.cc $(GEN_HEADERS)
	$(SC_DEP)

$(INJECTION_PREF)%.$(OBJEXT) : $(SYSTEMC_LIBRARY_SRC)/injection/%.cc $(_MAKEFILES)
	$(SC_COMPILE)
$(INJECTION_PREF)%.d : $(SYSTEMC_LIBRARY_SRC)/injection/%.cc $(GEN_HEADERS)
	$(SC_DEP)

$(INSTRUMENTATION_PREF)%.$(OBJEXT) : $(SYSTEMC_LIBRARY_SRC)/instrumentation/%.cc $(_MAKEFILES)
	$(SC_COMPILE)
$(INSTRUMENTATION_PREF)%.d : $(SYSTEMC_LIBRARY_SRC)/instrumentation/%.cc $(GEN_HEADERS)
	$(SC_DEP)

%.$(OBJEXT) : %.cc $(_MAKEFILES)
	$(SC_COMPILE)
%.d: %.cc $(_MAKEFILES) $(GEN_HEADERS)
	$(SC_DEP)
