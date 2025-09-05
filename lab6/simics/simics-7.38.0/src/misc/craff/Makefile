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

# Makefile for the craff utility

ifeq ($(HOST_TYPE),win64)
SIMICS_BASE := $(subst \,/,$(SIMICS_BASE))
endif

include $(HOST_DIR)/host-config

HERE := $(SIMICS_BASE)/src/misc/craff

CFLAGS += -I$(SIMICS_BASE)/src/include -I$(HERE)
CFLAGS += $(LIBUUID_INCLUDE)

ifeq ($(findstring linux,$(HOST_TYPE)),linux)
CFLAGS += -fPIE
endif

SOURCES=craff.c vhd.c
OBJECTS=$(SOURCES:.c=.$(OBJEXT))
TARGET=$(TARGET_DIR)/craff$(_EXE_SUFFIX)

LIBPATH_FLAGS = $(patsubst %, -L%, $(subst :, ,$(LIBPATH)))
LIBS += $(TARGET_DIR)/vtutils.a $(LIBPTHREAD) $(LIBDL) \
	$(VTUTILS_REQUIRED_LIBS) $(LIBUUID)

RPATH_OPT := $(RPATH_OPTS)

.PHONY: craff

$(TARGET): $(OBJECTS)
	$(info Linking $@)
	$(CC) $(CCLDFLAGS) $(LDFLAGS_LOCAL_FIRST) -o $@		\
		$^ $(LIBPATH_FLAGS) $(LIBS) $(LIBZ) $(LIBWIN32)	\
		$(LIBSOCKET) $(RPATH_OPT)

%.$(OBJEXT): $(HERE)/%.c
	$(info Compiling $<)
	$(CC) $(CFLAGS) $(CFLAGS_LOCAL) -c -o $@ $<

clean:
	rm -f $(TARGET_DIR)/$(TARGET) $(OBJECTS)

clobber: clean
