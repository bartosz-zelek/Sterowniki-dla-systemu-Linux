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

# Setup pkg path
include $(MODULE_MAKEFILE:module.mk=systemc-find.mk)
unique := SystemC-Library-$(HOST_TYPE)
P3 := packageinfo/SystemC-Library-$(HOST_TYPE)
MSG := Unable to locate: SystemC Library Package
$(eval $(call SC_FIND,_systemc_library_info,,,P3,MSG,unique))

EXTRACT_FROM := $(_systemc_library_info)/SystemC-Library-$(HOST_TYPE)
ifeq ($(WINDOWS),no)
    ifneq ($(SYSTEMC_CXX),)
        CXX = $(SYSTEMC_CXX)
    endif

    AS_VERSION := $(shell $(CXX) $(SYSTEMC_MODULE_LDFLAGS) $(MODULE_LDFLAGS) -xc -c /dev/null -Wa,-v -o /dev/null 2>&1)
    AS_NUMBERS := $(subst ., ,$(word 1, $(shell echo -E '$(AS_VERSION)' | grep -oP '\d+\.\d+')))
    ifeq ($(V),1)
        $(info AS_VERSION: $(AS_VERSION))
        $(info AS_NUMBERS: $(AS_NUMBERS))
    endif
    AS_MAJOR := $(or $(strip $(word 1, $(AS_NUMBERS))), 0)
    AS_MINOR := $(or $(strip $(word 2, $(AS_NUMBERS))), 0)
    AS_PATCH := $(or $(strip $(word 3, $(AS_NUMBERS))), 0)
    SCL_AS_VERSION := $(AS_MAJOR),$(AS_MINOR),$(AS_PATCH)

    LD_VERSION := $(shell $(CXX) $(SYSTEMC_MODULE_LDFLAGS) $(MODULE_LDFLAGS) -xc /dev/null -Wl,-v -o /dev/null 2>/dev/null)
    LD_NUMBERS := $(subst ., ,$(word 1, $(shell echo -E '$(LD_VERSION)' | grep -oP '\d+\.\d+')))
    ifeq ($(V),1)
        $(info LD_VERSION: $(LD_VERSION))
        $(info LD_NUMBERS: $(LD_NUMBERS))
    endif
    LD_MAJOR := $(or $(strip $(word 1, $(LD_NUMBERS))), 0)
    LD_MINOR := $(or $(strip $(word 2, $(LD_NUMBERS))), 0)
    LD_PATCH := $(or $(strip $(word 3, $(LD_NUMBERS))), 0)
    SCL_LD_VERSION := $(LD_MAJOR),$(LD_MINOR),$(LD_PATCH)

    SCL_VERSION := $(shell grep -oP --regex='build-id: \K\d{4}' $(EXTRACT_FROM) | tail -1)

    SCL_VERSION_DEFINES := -DSCL_AS_VERSION=$(SCL_AS_VERSION) \
                           -DSCL_LD_VERSION=$(SCL_LD_VERSION) \
                           -DSCL_VERSION=$(SCL_VERSION)
endif

ifeq ($(MSVC),yes)
	SCL_VERSION := $(shell powershell -Command "(Select-String -Path '$(EXTRACT_FROM)' -Pattern 'build-id: (\d{4})').Matches.Groups[-1].Value")
	SCL_VERSION_DEFINES := /DSCL_VERSION=$(SCL_VERSION)
endif
