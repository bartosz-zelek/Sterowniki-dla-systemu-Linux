## ****************************************************************************
##
##   INTEL CORPORATION PROPRIETARY INFORMATION
##
##   This software is supplied under the terms of a license
##   agreement or nondisclosure agreement with Intel Corporation
##   and may not be copied or disclosed except in accordance with
##   the terms of that agreement.
##   Copyright 2015-2021 Intel Corporation.
##
## ****************************************************************************
#
# Configuration makefile for building systemc kernel with Intel extensions.
#
# This makefile contains the configuration used when building systemc kernel
# for Intel. The purpose is to provide configuration to software layers above
# and to make it possible for a user to reference this file and understand how
# the systemc kernel was built with regards to Intel specific extensions.
#
# This makefile will setup the makefile variable SYSTEMC_KERNEL_INTC_DEFINES
# by defining combination key-value tuples that can be used to set compiler
# flags accordingly:
#
#     INTC_EXT
#     Intel extensions that are compatible with standard SystemC kernel ABI.
#     Allowed values are 0, 1.
#
#     INTC_EXT_ABI_BREAK
#     Intel extensions that are not compatible with standard SystemC kernel
#     ABI. Allowed values are 0, 1.
#
#     INTC_EXT_VALGRIND
#     Intel extensions that supports running Valgrind with the Memcheck tool.
#     Allowed values are 0, 1.
#
# If changes to the default flags are required, not only the SystemC kernel,
# but also related software such as systemc-library must be rebuilt with
# consistent flags set.
#
# To override default values, set the the makefile variables INTC_EXT or
# INTC_EXT_ABI_BREAK to 0 or 1 to disable or enable before including this file.
# For example, to enable INTC_EXT and INTC_EXT_ABI_BREAK:
# > INTC_EXT=1 INTC_EXT_ABI_BREAK=1 make

# Default values
_intc_ext := 1
_intc_ext_abi_break := 0
_intc_ext_valgrind := 0

# Handle overrides of INTC_EXT
ifneq ($(INTC_EXT),)
    _illegal := $(filter-out 0 1,$(INTC_EXT))
    ifneq ($(_illegal),)
        $(warning Illegal value for INTC_EXT: $(INTC_EXT))
        $(error Accepted values are 0 or 1)
    else
        _intc_ext := $(INTC_EXT)
    endif
else
    INTC_EXT := $(_intc_ext)
endif

# Handle overrides of INTC_EXT_ABI_BREAK
ifneq ($(INTC_EXT_ABI_BREAK),)
    _illegal := $(filter-out 0 1,$(INTC_EXT_ABI_BREAK))
    ifneq ($(_illegal),)
        $(warning Illegal value for INTC_EXT_ABI_BREAK: $(INTC_EXT_ABI_BREAK))
        $(error Accepted values are 0 or 1)
    else
        _intc_ext_abi_break := $(INTC_EXT_ABI_BREAK)
    endif
else
    INTC_EXT_ABI_BREAK := $(_intc_ext_abi_break)
endif

# Handle overrides of INTC_EXT_VALGRIND
ifneq ($(INTC_EXT_VALGRIND),)
    _illegal := $(filter-out 0 1,$(INTC_EXT_VALGRIND))
    ifneq ($(_illegal),)
        $(warning Illegal value for INTC_EXT_VALGRIND: $(INTC_EXT_VALGRIND))
        $(error Accepted values are 0 or 1)
    else
        _intc_ext_valgrind := $(INTC_EXT_VALGRIND)
    endif
else
    INTC_EXT_VALGRIND := $(_intc_ext_valgrind)
endif

# Setting the resulting values to exported SYSTEMC_KERNEL_INTC_DEFINES.
SYSTEMC_KERNEL_INTC_DEFINES += INTC_EXT=$(_intc_ext)
SYSTEMC_KERNEL_INTC_DEFINES += INTC_EXT_ABI_BREAK=$(_intc_ext_abi_break)
SYSTEMC_KERNEL_INTC_DEFINES += INTC_EXT_VALGRIND=$(_intc_ext_valgrind)
