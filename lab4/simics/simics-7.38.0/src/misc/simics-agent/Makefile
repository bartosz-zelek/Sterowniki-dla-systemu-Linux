# -*- Makefile ; coding: utf-8 -*-

# © 2010 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

OBJPRE ?=

TARGET_DIR ?= .
CFLAGS_LOCAL += -std=gnu99
OBJEXT=o

UNITS=agent util proto
LIBS=-L$(TARGET_DIR)

ifeq ($(OS),Windows_NT)
	RM=rm
	UNITS += util_win32 proto_win32
	EXE:=.exe
	CCLDFLAGS := $(filter-out -pie,$(CCLDFLAGS))
else
	UNAME_S := $(shell uname -s)
	UNITS += proto_unix util_linux
	ifeq ($(UNAME_S),Linux)
		LIBS += -lrt
		CCLDFLAGS := $(filter-out -pie,$(CCLDFLAGS))
	else
		$(error System not supported: $(UNAME_S))
	endif
	EXE:=
endif

ifneq ($(CROSSCOMPILE),)
	CC = $(CROSSCOMPILE)gcc$(EXE)
endif

TARGET=$(TARGET_DIR)/simics-agent$(EXE)
OBJECTS=$(patsubst %,$(OBJPRE)%.$(OBJEXT),$(UNITS))
DEPENDS=$(UNITS:=.d)

.PHONY: all clean
.DEFAULT: all

all: $(TARGET)

clean:
	$(RM) -rf $(TARGET) $(OBJECTS) $(DEPENDS)

-include $(DEPENDS)

$(TARGET): $(OBJECTS)
	$(info Linking $@)
	$(CC) $(CCLDFLAGS) $(LDFLAGS_LOCAL_FIRST) -o $@ $^ $(LIBS)

$(OBJPRE)%.$(OBJEXT): %.c
	$(info Compiling $*.c)
	$(CC) $(CFLAGS) $(CFLAGS_LOCAL) -c -o $@ $<

%.d: %.c
	$(info Creating dependencies: $(<F))
	$(CC) -MM -MP -MT $@ -MT $(@:.d=.$(OBJEXT)) -MF $@ $(CFLAGS_LOCAL) $<

util_win32.o: CFLAGS_LOCAL += -Wno-address
