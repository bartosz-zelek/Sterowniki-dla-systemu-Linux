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

OBJPRE ?=

TARGET_DIR ?= .
CFLAGS_LOCAL += -std=gnu99 -fPIC
OBJEXT=o

ifneq ($(CROSSCOMPILE),)
	CC = $(CROSSCOMPILE)gcc$(EXEEXT)
	AR := ar
endif

UNITS=magic-pipe pipe-header
CFLAGS_LOCAL += -static
ARFLAGS_LOCAL = rcs

ifeq ($(OS),Windows_NT)
	UNITS += pipe-win32
	CFLAGS_LOCAL += -D__USE_MINGW_ANSI_STDIO
	RM=rm -f
	ifeq ($(SIMICS_BASE),)
		EXEEXT=.exe
	endif
else
	UNITS += pipe-unix
endif

TARGET := $(TARGET_DIR)/libmagicpipe.a
OBJECTS=$(patsubst %,$(OBJPRE)%.$(OBJEXT),$(UNITS))
DEPENDS=$(UNITS:=.d)

.PHONY: all clean
.DEFAULT: all

all: $(TARGET)

clean:
	${RM} $(TARGET) $(OBJECTS) $(DEPENDS)

-include $(DEPENDS)

$(TARGET): $(OBJECTS)
	$(info Creating $@)
	rm -f $@
	$(AR) $(ARFLAGS_LOCAL) $@ $^

$(OBJPRE)%.$(OBJEXT): %.c
	$(info Compiling $*.c)
	$(CC) $(CFLAGS) $(CFLAGS_LOCAL) -c -o $@ $<

%.d: %.c
	$(info Creating dependencies: $(<F))
	$(CC) -MM -MP -MT $@ -MT $(@:.d=.$(OBJEXT)) -MF $@ $(CFLAGS_LOCAL) $<
