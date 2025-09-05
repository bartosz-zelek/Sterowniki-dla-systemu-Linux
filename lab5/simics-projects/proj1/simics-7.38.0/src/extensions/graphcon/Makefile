# -*- Makefile -*-

# © 2018 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

# Simics module makefile

CONSOLE_CLASS = graphcon
WINSOME_CLASS = gfx-frontend-winsome
MODULE_CLASSES = $(CONSOLE_CLASS) $(WINSOME_CLASS)

SRC_FILES =					\
	gfx-console.c				\
	recorder.c				\
	gfx-break.c				\
	break-strings.c				\
	console-frontend.c			\
	output.c				\
	vnc.c					\
	rect.c					\
	save.c					\
	winsome-frontend.c			\
	input.c

EXTRA_MODULE_VPATH := console-common crypto-engine

MODULE_CFLAGS = -DGFX_CONSOLE=$(CONSOLE_CLASS)				  \
		-DWINSOME_FRONTEND=$(WINSOME_CLASS)			  \
		-DNOTIFY_OPEN=graphcon-open -DNOTIFY_CLOSE=graphcon-close

MODULE_CFLAGS += -I$(HYPERSCAN_ROOT)/include -I$(PNG_INCLUDE_DIR) $(LIBZ_INCLUDE) $(X_INCLUDES)
MODULE_LDFLAGS = $(HYPERSCAN_LIB) $(LIBPNG) $(LIBZ) $(LIBSOCKET)
# rpath for libhs, libpng
MODULE_LDFLAGS += -Wl,-rpath,'$$ORIGIN/../sys/lib'

# Make sure keysymdef.h can be found
ifeq ($(findstring win, $(HOST_TYPE)),win)
MODULE_CFLAGS += -I$(PACKAGE_DIR)/simics-pkgs-x11-keysymdef-include
endif

PYTHON_FILES := module_load.py simics_start.py

THREAD_SAFE:= yes
SIMICS_API := latest

include $(MODULE_MAKEFILE)
