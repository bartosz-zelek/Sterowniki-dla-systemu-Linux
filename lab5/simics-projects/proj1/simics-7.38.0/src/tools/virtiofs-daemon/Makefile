# -*- Makefile ; coding: utf-8 -*-

# © 2023 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


DAEMON_SRC_DIR := $(SRC_BASE)/$(TARGET)
DAEMON_BIN_DIR=$(SIMICS_PROJECT)/$(HOST_TYPE)/bin

SRC := $(foreach x, $(DAEMON_SRC_DIR), $(wildcard $(addprefix $(x)/*,.c*)))

BLD_CXXFLAGS += -I$(LIBFUSE3_INCLUDE) -fPIE
# First rpath is for packaged daemon, second is for in-repo daemon
LDFLAGS += -L$(LIBFUSE3_LIB_PATH) \
	-Wl,-rpath,'$$ORIGIN/../sys/lib:$$ORIGIN/../../core/$(HOST_TYPE)/sys/lib' -lfuse3

EXTRA_DIST_SRC_FILES := GPL2_libfuse.txt MIT_cxxopts.txt README.md \
			cxxopts.hpp passthrough_hp_uds.cc

include $(MODULE_MAKEFILE)

$(TARGET):
	$(CXX) -o $(DAEMON_BIN_DIR)/$@ $(SRC) $(BLD_CXXFLAGS) $(LDFLAGS) -pie

.PHONY: clean
clean:
	rm -f $(DAEMON_BIN_DIR)/$(TARGET)

.PHONY: clobber
clobber: clean

.PHONY: all
all: $(TARGET)
