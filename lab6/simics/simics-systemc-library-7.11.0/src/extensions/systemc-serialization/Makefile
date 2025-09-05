# -*- Makefile -*-

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

MODULE_DIR:=$(SRC_BASE)/$(TARGET)

# Setup compiler preference
include $(MODULE_MAKEFILE:module.mk=systemc-find.mk)
unique := /compiler.mk
P1 := modules/systemc-library$(unique)
P3 := src/extensions/systemc-library$(unique)
MSG := Unable to locate: SystemC Library
$(eval $(call SC_FIND,_systemc_library,P1,,P3,MSG,unique))
include $(_systemc_library)/compiler.mk

ifeq ($(MSVC),yes)
  export SHELL=cmd.exe
  include $(MODULE_DIR)/msvc.mk
else
  include $(MODULE_DIR)/gcc.mk
endif
