# © 2020 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

# Prefer Visual Studio on Windows
COMPILERS = cl gcc

# Find out if host type is windows and which compiler to use
WINDOWS := $(if $(findstring win,$(HOST_TYPE)),yes,no)

# === begin code borrowed from module.mk ===
_SUPPORTED_COMPILERS := $(if $(_IS_WINDOWS),gcc cl,gcc)

ifneq ($(COMPILERS),)
  _COMPILERS := $(filter $(_SUPPORTED_COMPILERS),$(COMPILERS))
  $(if $(_COMPILERS),,$(error No supported compiler '$(_SUPPORTED_COMPILERS)' among given compilers '$(COMPILERS)'))
  _COMPILER := $(if $(findstring $(COMPILER), $(_COMPILERS)),$(COMPILER),$(firstword $(_COMPILERS)))
else
  _COMPILER:=gcc
endif
ifneq ($(findstring cl, $(_COMPILER)),)
  CC:=cl
endif

# Backwards compatibility: honour legacy MSVC choice at top level
ifeq ($(CC),cl)
MSVC := yes
CXX := $(CC)
else
MSVC := no
endif
# === end code borrowed from module.mk ===
