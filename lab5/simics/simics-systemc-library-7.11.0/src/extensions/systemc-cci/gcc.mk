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

# Makefile for building systemc cci on Linux

ifneq ($(SYSTEMC_CXX),)
    CXX = $(SYSTEMC_CXX)
endif

CXXFLAGS := -fPIC
ifneq ($(SYSTEMC_CXX_STANDARD),)
    CXXFLAGS += -std=c++$(SYSTEMC_CXX_STANDARD)
endif

cci_libs := libcciapi.a
INSTALL = $(CURDIR)/installation

.PHONY: build_and_copy
build_and_copy: $(install_dir)/$(cci_libs)

$(install_dir):
	mkdir -p $@

$(INSTALL):
	cp -r $(cci_src_dir) $@
	chmod +w $@

$(INSTALL)/lib/$(cci_libs): | $(INSTALL)
	CXX="$(CXX)" CXXFLAGS="$(CXXFLAGS)" \
        SYSTEMC_HOME="$(kernel_src_dir)" \
        CCI_HOME="$(INSTALL)" \
        $(MAKE) -C $(INSTALL)/src all

$(install_dir)/$(cci_libs): $(INSTALL)/lib/$(cci_libs) | $(install_dir)
	$(info Creating $@)
	cp -p $(INSTALL)/lib/$(cci_libs) $@
