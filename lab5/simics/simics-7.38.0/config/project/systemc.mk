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

# All we need in this file is to utlize the systemc-find.mk to find
# systemc-library.mk from either the project or a package; then include that to
# get the rest of the rules.
#
# NOTE: please don't add any CFLAGS, LDFLAGS, ... to this file

include $(MODULE_MAKEFILE:module.mk=systemc-find.mk)
# Find source path for systemc-library and include more Makefile rules
unique := /systemc-library.mk
P1 := modules/systemc-library$(unique)
P3 := src/extensions/systemc-library$(unique)
MSG := Unable to locate: SystemC Library
$(eval $(call SC_FIND,SYSTEMC_LIBRARY_SRC,P1,,P3,MSG,unique))
include $(call _MAKEQUOTE,$(SYSTEMC_LIBRARY_SRC)/systemc-library.mk)
