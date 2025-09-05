# -*- Makefile -*-

# © 2016 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

# Try to find include paths from the project, otherwise from another
# default location that might have to be configured.
ifeq ($(wildcard ../../../config.mk),)
    # Location to <simics base>/src/include
    INCLUDE_PATHS="../../../../src/include"
else
    # From project
    include ../../../config.mk
endif
CFLAGS = -std=c99 -I $(INCLUDE_PATHS) -O0 -g -static

all:

active: active.c
	$(CC) $(CFLAGS) $< -o $@

all: active

clean:
	rm -f active
