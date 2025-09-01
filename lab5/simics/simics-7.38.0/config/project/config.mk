# -*- Makefile ; coding: utf-8 -*-

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

# The job of this makefile is to define a large set of variables, and
# to export some of them. The makefile assumes that a few variables
# (RAW_* and *INCLUDE_PATHS from config.mk and CC from compiler.mk),
# as well as the _shellquote and makequote functions, are predefined.

SIMICS_MAJOR_VERSION=7

_RAW_SIMICS_BASE:=$(RAW_SIMICS_BASE)
_RAW_EML_PACKAGE:=$(RAW_EML_PACKAGE)
SIMICS_PACKAGE_LIST:=$(call _shellquote,$(RAW_SIMICS_PACKAGE_LIST))

RAW_SIMICS_PROJECT:=$(shell pwd)
SHELL_SIMICS_PROJECT:=$(call _shellquote,$(RAW_SIMICS_PROJECT))
ifeq ($(SHELL_SIMICS_PROJECT),$(RAW_SIMICS_PROJECT))
# Project path looks normal, use absolute path.
SIMICS_PROJECT:=$(RAW_SIMICS_PROJECT)
else
# Found troublesome characters, duck using relative path. Command
# lines will sometimes be more difficult to follow, but at least
# everything will build.
SIMICS_PROJECT:=../../../..
endif

compiler:=$(shell $(PYTHON) $(SIMICS_BASE)/scripts/build/cctype.py --type $(CC))
CC_TYPE:=$(compiler)

_cc_version:=$(shell $(PYTHON) $(SIMICS_BASE)/scripts/build/cctype.py --version $(CC))

ifeq (default,$(origin CXX))
    CXX=
endif

OBJEXT=o
SO_SFX=so

LDFLAGS_PY3+=-L$(SIMICS_BASE)/$(HOST_TYPE)/bin
LDFLAGS += $(LDFLAGS_PY3)

LDFLAGS += -z noexecstack -z relro -z now
LIBS=-lsimics-common -lvtutils
LIBZ ?= -lz
_LIBSSP =

DEP_CC=$(CC)
DEP_CXX=$(CXX)

system:=$(HOST_TYPE)-$(compiler)

ifneq ($(SIMICS_PYTHON),)
PYTHON_INCLUDE := -I$(shell $(SIMICS_PYTHON) -c "import sysconfig; print(sysconfig.get_paths()['include'])")
PYTHON_LDFLAGS := -L$(shell $(SIMICS_PYTHON) -c "import sysconfig; print(sysconfig.get_paths()['stdlib'])")
PYTHON3_INCLUDE := $(PYTHON_INCLUDE)
PYTHON3_LDFLAGS := $(PYTHON_LDFLAGS)
else
PYTHON3_VERSION := 3.10
PYTHON3_INCLUDE:=-I$(PYTHON_PKG)/$(HOST_TYPE)/include/python$(PYTHON3_VERSION)
PYTHON_INCLUDE := $(PYTHON3_INCLUDE)
PYTHON3_LDFLAGS := -L$(PYTHON_PKG)/$(HOST_TYPE)/sys/lib
PYTHON_LDFLAGS := $(PYTHON3_LDFLAGS)
endif

ifeq ($(OBJDUMP),)
    OBJDUMP:=objdump
endif

DISAS=$(OBJDUMP) -dw

GCC_NO_OPT_CFLAGS=-O0

GCC_OPT_CFLAGS=-O2 -D_FORTIFY_SOURCE=2 -DNDEBUG

ifneq (,$(filter $(compiler),gcc3 gcc4 icc))
    EXPORTMAP := $(SIMICS_BASE)/config/project/exportmap.elf
    CCLDFLAGS_DYN=-shared -Wl,--version-script,$(EXPORTMAP),--build-id
    BLD_COMMON_CFLAGS:=-gdwarf-5 -Wall -Wwrite-strings -std=gnu11 -fPIC
    BLD_COMMON_CXXFLAGS:=-gdwarf-5 -Wall -Wwrite-strings -fPIC
    OPT_CFLAGS:=$(GCC_OPT_CFLAGS)
    NO_OPT_CFLAGS:=$(GCC_NO_OPT_CFLAGS)
    DEP_CFLAGS=-M -MP -std=gnu11
    DEP_CXXFLAGS=-M -MP
    DEP_DOUBLE_TARGETS=yes
    CPP=$(CC) -E
    ifeq ($(compiler),icc)
        CXX_NAME=icc
        BLD_COMMON_CFLAGS+=-wd271,593,869,1418,1419
    else
        CXX_NAME=g++
        GCC_VISIBILITY=-fvisibility=hidden
    endif

    # SDL flags
    BLD_COMMON_CFLAGS += -Wformat-security
    BLD_COMMON_CXXFLAGS += -Wformat-security
endif

ifneq ($(_CORE_PROJECT_BUILD),)
        WARN_CFLAGS += -Werror -Wundef -Wformat -Wformat-nonliteral
        BLD_COMMON_CFLAGS += -march=haswell -mtune=intel -std=gnu23
        BLD_COMMON_CFLAGS += -B $(BINUTILS)
        BLD_COMMON_CXXFLAGS += -B $(BINUTILS)
        NO_OPT_CFLAGS += $(WARN_CFLAGS)
        OPT_CFLAGS += $(WARN_CFLAGS)
	LDFLAGS += -B $(BINUTILS)

        # SDL flags
	WARN_CFLAGS += -Wformat -Wformat-security -Werror=format-security
	BLD_COMMON_CFLAGS += -fcf-protection=full -fstack-protector \
		-fstack-protector-strong
	BLD_COMMON_CXXFLAGS += -fcf-protection=full -fstack-protector \
		-fstack-protector-strong
	LDFLAGS += -z noexecstack -z relro -z now
endif

BLD_OPT_CFLAGS:=$(BLD_COMMON_CFLAGS) $(OPT_CFLAGS)

# For compatibility, we accept DEBUG=yes instead of D=1.
ifeq ($(DEBUG),yes)
    $(warning DEBUG=yes is deprecated, use D=1 instead)
    D=1
endif
# For compatibility, we accept VERBOSE=yes instead of V=1.
ifeq ($(VERBOSE),yes)
    $(warning VERBOSE=yes is deprecated, use V=1 instead)
    V=1
endif

ifeq ($(D),1)
    BLD_CFLAGS := $(BLD_COMMON_CFLAGS) $(NO_OPT_CFLAGS) -fno-inline
    BLD_CXXFLAGS:=$(BLD_COMMON_CXXFLAGS) $(NO_OPT_CFLAGS)
    DMLC_OPT_FLAGS=-g
else
    BLD_CFLAGS := $(BLD_COMMON_CFLAGS) $(OPT_CFLAGS)
    BLD_CXXFLAGS:=$(BLD_COMMON_CXXFLAGS) $(OPT_CFLAGS)
    DMLC_OPT_FLAGS=
endif

ifeq (,$(CXX))
    ifeq (,$(CXX_NAME))
        CXX=g++
    else
	CC_NAME=$(notdir $(CC))
	CC_DIR=$(patsubst %$(CC_NAME),%,$(CC))
	ifeq (,$(CC_DIR))
	    CXX=$(CXX_NAME)
	else
	    CXX=$(CC_DIR)$(CXX_NAME)
	endif
    endif
endif

PYTHON_BLD_CFLAGS := $(BLD_CFLAGS) $(PYTHON_INCLUDE) \
                     -Wno-write-strings -Wno-undef \
                     -DPy_LIMITED_API=0x030A0000 -DPY_MAJOR_VERSION=3

CCLD=$(CC)
MKDIRS = mkdir -p
CAT = cat

_IS_WINDOWS=

export AR
export BLD_CFLAGS
export BLD_CXXFLAGS
export BLD_OPT_CFLAGS
export CAT
export CC
export CC_TYPE
export CCLD
export CCLDFLAGS_DYN
export CFLAGS
export COMPILER
export CPP
export CXX
export CXXFLAGS
export D
export DEP_CC
export DEP_CFLAGS
export DEP_CXX
export DEP_CXXFLAGS
export DEP_DOUBLE_TARGETS
export DISAS
export DMLC
export DMLC_DIR
export DMLC_OPT_FLAGS
export GCC_VISIBILITY
export INCLUDE_PATHS
export CXX_INCLUDE_PATHS
export DML_INCLUDE_PATHS
export LDFLAGS
export LDFLAGS_PY3
export LIBS
export LIBZ
export MKDIRS
export OBJEXT
export PYTHON
export PYTHON3
export PYTHON_BLD_CFLAGS
export PYTHON_INCLUDE
export PYTHON_LDFLAGS
export PYTHON3_INCLUDE
export PYTHON3_LDFLAGS
export SIMICS_MAJOR_VERSION
export SIMICS_BASE
export SIMICS_MODEL_BUILDER
export SIMICS_PACKAGE_LIST
export SIMICS_PROJECT
export SO_SFX
export SYSTEMC_KERNEL_VERSION
export USER_BUILD_ID
export V
export _IS_WINDOWS
export _RAW_EML_PACKAGE
export _RAW_SIMICS_BASE
export _LIBSSP
