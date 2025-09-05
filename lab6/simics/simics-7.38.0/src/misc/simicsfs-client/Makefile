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

ifneq ($(CROSSCOMPILE),)
	CC := $(CROSSCOMPILE)gcc
endif

OBJPRE ?=

TARGET_DIR ?= .
CFLAGS += -std=gnu99 -D_FILE_OFFSET_BITS=64
LIBS ?= -lmagicpipe -lfuse
CFLAGS += $(EXTRA_CFLAGS)
LDFLAGS += $(EXTRA_LDFLAGS)

CFLAGS += $(INCLUDES)

OBJS := simicsfs-client.o simicsfs-prot.o
OBJECTS=$(patsubst %,$(OBJPRE)%,$(OBJS))

TARGET := $(TARGET_DIR)/simicsfs-client

.PHONY: all clean
.DEFAULT: all

all: $(TARGET)

clean:
	${RM} ${TARGET} ${OBJECTS}

$(TARGET): $(OBJECTS)
	$(info Linking $@)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJPRE)%.o: %.c
	$(info Compiling $^)
	$(CC) $(CFLAGS) -c -o $@ $^
