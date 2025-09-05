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

#
# Makefile that supports building SystemC Library devices in simics projects.

ifeq ($(SYSTEMC_LIBRARY_SRC),)
    $(error Expects SYSTEMC_LIBRARY_SRC to point out path for systemc-library.)
endif

EXTRA_MODULE_VPATH += systemc-library

# Adding interfaces to support trace/break on SystemC objects
EXTRA_MODULE_VPATH += systemc-interfaces

# Setup for systemc-iosf-gaskets.
ifeq ($(SYSTEMC_IOSF),yes)
    unique := /systemc-config.mk
    P1 := modules/systemc-iosf-gaskets$(unique)
    P3 := src/extensions/systemc-iosf-gaskets$(unique)
    MSG := Unable to locate: IOSF Gaskets
    $(eval $(call SC_FIND,IOSF_GASKET_SRC,P1,,P3,MSG,unique))
    include $(call _MAKEQUOTE,$(IOSF_GASKET_SRC)/systemc-config.mk)
endif

# Find src path for isctlm awareness and include configuration makefile.
ifeq ($(SYSTEMC_ISCTLM),yes)
    include $(call _MAKEQUOTE,$(SYSTEMC_LIBRARY_SRC)/systemc-boost.mk)
    # Workaround for boost use of #pragma to tell MSVC to link with a lib that
    # is clearly not needed. We must set the LIBPATH
    ifeq ($(MSVC),yes)
        SYSTEMC_MODULE_LDFLAGS_DEFAULT += /LIBPATH:$(SIMICS_PROJECT)/$(HOST_TYPE)/lib
    endif

    unique := /systemc-config.mk
    P1 := modules/systemc-isctlm-awareness$(unique)
    P3 := src/extensions/systemc-isctlm-awareness$(unique)
    MSG := Unable to locate: ISCTLM Awareness
    $(eval $(call SC_FIND,ISCTLM_SRC_DIR,P1,,P3,MSG,unique))
    include $(call _MAKEQUOTE,$(ISCTLM_SRC_DIR)/systemc-config.mk)
endif

# Find out if host type is windows and what compiler to use
WINDOWS := $(if $(findstring win,$(HOST_TYPE)),yes,no)
IFEQ = $(and $(findstring $(1),$(2)),$(findstring $(2),$(1)))
MSVC := $(if $(call IFEQ,$(CXX),cl),yes,no)

INCDIR := $(if $(call IFEQ,$(MSVC),yes),/I,-I)
DEF := $(if $(call IFEQ,$(MSVC),yes),/D,-D)

# Internal builds must setup VCVARSPREFIX to point to Visual Studio version
# The version is set in config/env-linux64.mk
ifeq ($(MSVC),yes)
ifneq ($(_CORE_PROJECT_BUILD),)
ifneq ($(__VCVARS),)
export VCVARSPREFIX="$(__VCVARS)" > NUL &
else
$(error Invalid config; __VCVARS not set)
endif
endif
endif

ifeq ($(SYSTEMC_KERNEL_VERSION),)
    $(error SYSTEMC_KERNEL_VERSION not set; See Programming Guide for how to set it)
endif

# Find systemc kernel src and also add include flags and Intel extensions flags.
ifeq ($(SYSTEMC_CORE_CFLAGS),)
    unique := /systemc.h
    P1 := external/systemc/kernel-$(SYSTEMC_KERNEL_VERSION)/src$(unique)
    P3 := src/external/systemc/kernel-$(SYSTEMC_KERNEL_VERSION)/src$(unique)
    MSG := Unable to locate: SystemC Kernel
    $(eval $(call SC_FIND,KERNEL_SRC,P1,,P3,MSG,unique))
    SYSTEMC_CORE_CFLAGS_DEFAULT += $(INCDIR)$(KERNEL_SRC)

    SYSTEMC_CORE_CFLAGS_DEFAULT += $(SYSTEMC_CCI_CFLAGS)
    SYSTEMC_CORE_CFLAGS_DEFAULT += $(SYSTEMC_CHECKPOINT_CFLAGS)

    # Setup defines for intel systemc-kernel extensions.
    # Note that these flags must match when building systemc-kernel,
    # systemc-library and systemc devices. For information on how to disable
    # Intel extensions, see the file intc-config.mk in systemc kernel package.
    intc_config := $(wildcard $(KERNEL_SRC)/../intc-config.mk)
    ifeq ($(intc_config),)
        msg := Missing intc-config.mk. Please report this.
        $(error $(msg))
    else
        include $(intc_config)
        intc_cflags := $(foreach d,$(SYSTEMC_KERNEL_INTC_DEFINES),$(DEF)$(d))
        SYSTEMC_CORE_CFLAGS_DEFAULT += $(intc_cflags)
    endif
endif

# Find systemc checkpoint and setup compiler/linker flags
_USE_SIMICS_CHECKPOINTING = no
ifneq ($(_CORE_PROJECT_BUILD),)
    _USE_SIMICS_CHECKPOINTING := yes
else
    ifeq ($(USE_SIMICS_CHECKPOINTING),yes)
        _USE_SIMICS_CHECKPOINTING := yes
    endif
endif
ifeq ($(_USE_SIMICS_CHECKPOINTING),yes)
    ifneq ($(INTC_EXT),0)
        # systemc-boost.mk cannot be included twice
        ifneq ($(SYSTEMC_ISCTLM),yes)
            include $(call _MAKEQUOTE,$(SYSTEMC_LIBRARY_SRC)/systemc-boost.mk)
            # Workaround for boost use of #pragma to tell MSVC to link with a
            # lib that is clearly not needed. We must set the LIBPATH
            ifeq ($(MSVC),yes)
                SYSTEMC_MODULE_LDFLAGS_DEFAULT += /LIBPATH:$(SIMICS_PROJECT)/$(HOST_TYPE)/lib
            endif
        endif

        unique := /systemc-config.mk
        P1 := modules/systemc-checkpoint$(unique)
        P3 := src/extensions/systemc-checkpoint$(unique)
        MSG := Unable to locate: SystemC Checkpoint
        $(eval $(call SC_FIND,CHECKPOINT_SRC_DIR,P1,,P3,MSG,unique))
        include $(call _MAKEQUOTE,$(CHECKPOINT_SRC_DIR)/systemc-config.mk)
    endif
endif

# Find systemc cci and setup compiler/linker flags
ifneq ($(USE_SIMICS_CCI),no)
    ifneq ($(INTC_EXT),0)
        unique := /systemc-config.mk
        P1 := modules/systemc-cci$(unique)
        P3 := src/extensions/systemc-cci$(unique)
        MSG := Unable to locate: CCI
        $(eval $(call SC_FIND,CCI_SRC_DIR,P1,,P3,MSG,unique))
        include $(call _MAKEQUOTE,$(CCI_SRC_DIR)/systemc-config.mk)
    endif
endif

# Setup systemc module cflags.
ifeq ($(SYSTEMC_MODULE_CFLAGS),)
    ifeq ($(MSVC),yes)
        SYSTEMC_MODULE_CFLAGS_DEFAULT += /D_WINSOCKAPI_
        SYSTEMC_MODULE_CFLAGS_DEFAULT += /DWIN32
        SYSTEMC_MODULE_CFLAGS_DEFAULT += /wd4005
        # defining wd4996, _SC_SECURE_NO_WARNINGS and _CRT_SECURE_NO_DEPRECATE
        # gets rid of warning C4996
        SYSTEMC_MODULE_CFLAGS_DEFAULT += /wd4996
        SYSTEMC_MODULE_CFLAGS_DEFAULT += /D_SCL_SECURE_NO_WARNINGS
        SYSTEMC_MODULE_CFLAGS_DEFAULT += /D_CRT_SECURE_NO_DEPRECATE
        SYSTEMC_MODULE_CFLAGS_DEFAULT += /GR
        SYSTEMC_MODULE_CFLAGS_DEFAULT += /vmg /MP
        SYSTEMC_MODULE_CFLAGS_DEFAULT += /vmv
        SYSTEMC_MODULE_CFLAGS_DEFAULT += /FS
        SYSTEMC_MODULE_CFLAGS_DEFAULT += /D_NO_CRT_STDIO_INLINE
        # VS 2022 use C++14 by default and filesystem is experimental
        SYSTEMC_MODULE_CFLAGS_DEFAULT += /D_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
        ifneq ($(SYSTEMC_CXX_STANDARD),)
            SYSTEMC_MODULE_CFLAGS_DEFAULT += /std:c++$(SYSTEMC_CXX_STANDARD)
        endif
    else
        # Suppress some warnings we cannot fix (because they originate from the
        # kernel or from Boost)
        BLD_CXXFLAGS += -Wno-undef -Wno-unused-variable -Wno-maybe-uninitialized

        # Don't use NDEBUG in SystemC builds for now. SystemC code within some
        # Intel IPs end up with unused variables when asserts are turned off,
        # leading to warnings leading to errors in their sanity checking. The
        # code should be fixed; but it was easier for us to just filter it out
        # here for now.
        BLD_CXXFLAGS := $(filter-out -DNDEBUG,$(BLD_CXXFLAGS))

        # To avoid -Wvla to contaminate python trampolines (SIMICS-13121) we
        # only add -Wvla to the BLD_CXXFLAGS, and unconditionally. This should
        # be safe and desireable for anyone wanting to build platform (windows
        # vs. linux) agnostic device modules.
        BLD_CXXFLAGS += -Wvla

        # As of GCC 6.x C++14 is the default, but we set it explicitly for clarity.
        # Need GNU extensions to support variadic macros used by Simics
        ifneq ($(SYSTEMC_CXX_STANDARD),)
            BLD_CXXFLAGS += -std=gnu++$(SYSTEMC_CXX_STANDARD)
            DEP_CXXFLAGS += -std=gnu++$(SYSTEMC_CXX_STANDARD)
        endif
    endif

    # Awareness needs sc_spawn
    SYSTEMC_MODULE_CFLAGS_DEFAULT += $(DEF)SC_INCLUDE_DYNAMIC_PROCESSES

    # Compile-time headers
    NON_ESSENTIAL_PATTERN := /systemc-library-version-checks.h
    P1 := $(HOST_TYPE)/lib/systemc/$(SYSTEMC_KERNEL_VERSION)$(NON_ESSENTIAL_PATTERN)
    P2 := $(SYSTEMC_DEPS_CACHE)/$(SYSTEMC_KERNEL_VERSION)$(NON_ESSENTIAL_PATTERN)
    P3 := $(HOST_TYPE)/lib/systemc/$(SYSTEMC_KERNEL_VERSION)$(NON_ESSENTIAL_PATTERN)
    MSG := Unable to locate: compile-time version file
    $(eval $(call SC_FIND,COMPILE_TIME_FILE_PATH,P1,P2,P3,MSG,NON_ESSENTIAL_PATTERN))
    SYSTEMC_MODULE_CFLAGS_DEFAULT += $(INCDIR)$(COMPILE_TIME_FILE_PATH)
    include $(call _MAKEQUOTE,$(SYSTEMC_LIBRARY_SRC)/systemc-version.mk)
    SYSTEMC_MODULE_CFLAGS_DEFAULT += $(SCL_VERSION_DEFINES)

    # Add attributes and interfaces for adapter
    ifeq ($(MSVC),yes)
        SYSTEMC_MODULE_CFLAGS_DEFAULT += /FI $(SYSTEMC_LIBRARY_SRC)/simics/systemc/class_decorator_include.h
    else
        SYSTEMC_MODULE_CFLAGS_DEFAULT += -include $(SYSTEMC_LIBRARY_SRC)/simics/systemc/class_decorator_include.h
    endif
endif

ifeq ($(SYSTEMC_MODULE_LDFLAGS),)
    ifeq ($(findstring linux, $(HOST_TYPE)), linux)
        SYSTEMC_MODULE_LDFLAGS_DEFAULT += -Wl,-rpath-link,$(SIMICS_BASE)/$(HOST_TYPE)/sys/lib -Wl,-rpath-link,$(SIMICS_BASE)/$(HOST_TYPE)/bin
        ifneq ($(BINUTILS),)
            SYSTEMC_MODULE_LDFLAGS_DEFAULT += -B$(BINUTILS)
        endif
    endif
endif

# Adding Core deps: SystemC Library > CCI & Checkpointing > Kernel
ifeq ($(SYSTEMC_CORE_LDFLAGS),)
    ifeq ($(MSVC),yes)
         LIBRARY_SYSTEMC_LIBRARY := systemc_library.lib
    else
         LIBRARY_SYSTEMC_LIBRARY := libsystemc-library.a
    endif
    P1 := $(HOST_TYPE)/lib/systemc/$(SYSTEMC_KERNEL_VERSION)/$(LIBRARY_SYSTEMC_LIBRARY)
    P2 := $(SYSTEMC_DEPS_CACHE)/$(SYSTEMC_KERNEL_VERSION)/$(LIBRARY_SYSTEMC_LIBRARY)
    P3 := $(HOST_TYPE)/lib/systemc/$(SYSTEMC_KERNEL_VERSION)/$(LIBRARY_SYSTEMC_LIBRARY)
    MSG := Unable to locate: SystemC Library libs
    $(eval $(call SC_FIND,SYSTEMC_LIBRARY_LIB,P1,P2,P3,MSG))
    SYSTEMC_CORE_LDFLAGS_DEFAULT += $(SYSTEMC_LIBRARY_LIB)

    SYSTEMC_CORE_LDFLAGS_DEFAULT += $(SYSTEMC_CCI_LDFLAGS)
    SYSTEMC_CORE_LDFLAGS_DEFAULT += $(SYSTEMC_CHECKPOINT_LDFLAGS)

    ifeq ($(MSVC),yes)
        SYSTEMC_CORE_LIB := SystemC.lib
        ifeq ($(D),1)
	    SYSTEMC_CORE_LDFLAGS_DEFAULT += /DEBUG
        endif
    else
        SYSTEMC_CORE_LIB := libsystemc.a
    endif
    P1 := $(HOST_TYPE)/lib/systemc/$(SYSTEMC_KERNEL_VERSION)/$(SYSTEMC_CORE_LIB)
    P2 := $(SYSTEMC_DEPS_CACHE)/$(SYSTEMC_KERNEL_VERSION)/$(SYSTEMC_CORE_LIB)
    P3 := $(HOST_TYPE)/lib/systemc/$(SYSTEMC_KERNEL_VERSION)/$(SYSTEMC_CORE_LIB)
    MSG := Unable to locate: SystemC Kernel libs
    $(eval $(call SC_FIND,SYSTEMC_CORE_LIB,P1,P2,P3,MSG))
    SYSTEMC_CORE_LDFLAGS_DEFAULT += $(SYSTEMC_CORE_LIB)
endif

# Adding additional dependencies without deps on SystemC Kernel
ifeq ($(SYSTEMC_CORE_LDFLAGS),)
    # On linux, librt is also needed (INTC_EXT process profiling)
    ifeq ($(WINDOWS),no)
        SYSTEMC_CORE_LDFLAGS_DEFAULT += -lrt
    endif

    # Quickthreads support for GDB (linux only)
    ifeq ($(WINDOWS),no)
        SYSTEMC_CORE_LDFLAGS_DEFAULT += -ldl
        QT_DB_LIB=libsystemc-quickthreads-db.so
        P1 := $(HOST_TYPE)/lib/$(QT_DB_LIB)
        P2 := $(SYSTEMC_DEPS_CACHE)/$(QT_DB_LIB)
        P3 := $(HOST_TYPE)/lib/$(QT_DB_LIB)
        # No error message for external users
        $(eval $(call SC_FIND,QT_DB_PATH,P1,P2,P3,,QT_DB_LIB))
        ifneq ($(QT_DB_PATH),)
            SYSTEMC_CORE_LDFLAGS_DEFAULT += -Wl,-rpath,$(QT_DB_PATH)
        endif
    endif
endif

# Set systemc makefile variables to default values, unless already set.
ifeq ($(SYSTEMC_CORE_CFLAGS),)
    SYSTEMC_CORE_CFLAGS := $(SYSTEMC_CORE_CFLAGS_DEFAULT)
endif
ifeq ($(SYSTEMC_CORE_LDFLAGS),)
    SYSTEMC_CORE_LDFLAGS := $(SYSTEMC_CORE_LDFLAGS_DEFAULT)
endif

ifeq ($(SYSTEMC_MODULE_CFLAGS),)
    SYSTEMC_MODULE_CFLAGS := $(SYSTEMC_MODULE_CFLAGS_DEFAULT)
endif
ifeq ($(SYSTEMC_MODULE_LDFLAGS),)
    SYSTEMC_MODULE_LDFLAGS := $(SYSTEMC_MODULE_LDFLAGS_DEFAULT)
endif

ifeq ($(SYSTEMC_ISCTLM_CFLAGS),)
    SYSTEMC_ISCTLM_CFLAGS := $(SYSTEMC_ISCTLM_CFLAGS_DEFAULT)
endif
ifeq ($(SYSTEMC_ISCTLM_LDFLAGS),)
    SYSTEMC_ISCTLM_LDFLAGS := $(SYSTEMC_ISCTLM_LDFLAGS_DEFAULT)
endif

ifeq ($(SYSTEMC_BOOST_CFLAGS),)
    SYSTEMC_BOOST_CFLAGS := $(SYSTEMC_BOOST_CFLAGS_DEFAULT)
endif
ifeq ($(SYSTEMC_BOOST_LDFLAGS),)
    SYSTEMC_BOOST_LDFLAGS := $(SYSTEMC_BOOST_LDFLAGS_DEFAULT)
endif

ifneq ($(TARGET),)
    # TODO(ah): Workaround for ASE modules that for some reason don't trigger
    #           the registration of tools and gasket classes. Which is a bug in
    #           the module that should be fixed. This is just a "temporary"
    #           workaround to silence the warnings until bug is fixed. For this
    #           reason we don't want to indent the code.
    ifeq ($(_WORKAROUND_FOR_ASE),)
    MODULE_CFLAGS += $(DEF)MODULE_NAME=\"$(TARGET)\"
    PYTHON_MODULE_NAME = $(subst -,_,$(TARGET))
    # Add gasket classes registered by the SCL to the module
    ifneq ($(_SYSTEMC_GASKET_CLASSES),no)
        MODULE_CLASSES += $(PYTHON_MODULE_NAME)_gasket_composite_Pci
        MODULE_CLASSES += $(PYTHON_MODULE_NAME)_gasket_composite_Pcie
        MODULE_CLASSES += $(PYTHON_MODULE_NAME)_gasket_simics2systemc_Signal
        MODULE_CLASSES += $(PYTHON_MODULE_NAME)_gasket_systemc2simics_Signal
        MODULE_CLASSES += $(addprefix $(PYTHON_MODULE_NAME)_gasket_simics2tlm_, EthernetCommon Packet I2cSlaveV2 IoMemory)
        MODULE_CLASSES += $(addprefix $(PYTHON_MODULE_NAME)_gasket_simics2tlm_, PciDevice PciExpress SerialDevice)
        MODULE_CLASSES += $(addprefix $(PYTHON_MODULE_NAME)_gasket_tlm2simics_, EthernetCommon Packet I2cMasterV2 MemorySpace)
        MODULE_CLASSES += $(addprefix $(PYTHON_MODULE_NAME)_gasket_tlm2simics_, PciBus SerialDevice)
	MODULE_CFLAGS += $(DEF)REGISTER_SYSTEMC_GASKET_CLASSES
    endif
    # Add tool classes
    MODULE_CLASSES += $(addprefix $(PYTHON_MODULE_NAME)_, sc_break_tool sc_protocol_checker_tool sc_trace_tool)
    MODULE_CLASSES += $(addprefix $(PYTHON_MODULE_NAME)_, sc_transaction_tracker_tool sc_vcd_trace_tool tool_connection)
    ifeq ($(SYSTEMC_ISCTLM),yes)
        MODULE_CLASSES += $(addprefix $(PYTHON_MODULE_NAME)_, isctlm_break_tool isctlm_trace_tool)
    endif
    endif  # _WORKAROUND_FOR_ASE
endif

ifneq ($(SYSTEMC_SERIAL_TD),)
    MODULE_CFLAGS += $(DEF)SYSTEMC_SERIAL_TD
endif

# Add SYSTEMC_* flags to MODULE_* to be used when building.
ifeq ($(SYSTEMC_ISCTLM),yes)
    MODULE_CFLAGS += $(SYSTEMC_ISCTLM_CFLAGS)
    MODULE_LDFLAGS += $(SYSTEMC_ISCTLM_LDFLAGS)
endif

# Deps order: module > core > boost
MODULE_CFLAGS += $(SYSTEMC_MODULE_CFLAGS)
MODULE_CFLAGS += $(SYSTEMC_CORE_CFLAGS)
MODULE_CFLAGS += $(SYSTEMC_BOOST_CFLAGS)
MODULE_LDFLAGS += $(SYSTEMC_MODULE_LDFLAGS)
MODULE_LDFLAGS += $(SYSTEMC_CORE_LDFLAGS)
MODULE_LDFLAGS += $(SYSTEMC_BOOST_LDFLAGS)

ifneq ($(SYSTEMC_CXX),)
CXX = $(SYSTEMC_CXX)
CCLD = $(SYSTEMC_CXX)  # should link using C++ linker
DEP_CXX = $(SYSTEMC_CXX)
# TODO(ah): assume CXX means GCC for now
CC = $(subst g++,gcc,$(SYSTEMC_CXX))
DEP_CC = $(CC)
# See BLD_CXXFLAGS for setting -std=gnu++XX
endif
