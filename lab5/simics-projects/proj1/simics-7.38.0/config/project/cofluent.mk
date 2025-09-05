# -*- Makefile ; coding: utf-8 -*-

# © 2021 Intel Corporation
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
# Makefile include needed by CoFluent modules
#

# CoFluent is using a similar trampoline as SystemC and is re-using the SC_FIND
# macro for convenience
include $(MODULE_MAKEFILE:module.mk=systemc-find.mk)
unique := /cofluent.mk
P1 := modules/cofluent$(unique)
P2 := $(COF_ISIM_BASE)/config/project$(unique)
P3 := src/extensions/cofluent$(unique)
MSG := Unable to locate: cofluent.mk file
$(eval $(call SC_FIND,COF_SRC,P1,P2,P3,MSG,unique))
include $(call _MAKEQUOTE,$(COF_SRC)/cofluent.mk)
