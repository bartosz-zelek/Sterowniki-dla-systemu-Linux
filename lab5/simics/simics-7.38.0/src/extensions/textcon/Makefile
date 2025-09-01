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

MODULE_CLASSES = textcon text-frontend-winsome

SRC_FILES =					\
	text-console.c				\
	driver.c				\
	recorder.c				\
	break-strings.c				\
	console-frontend.c			\
	screen.c				\
	winsome-frontend.c			\
	vt100.c					\
	telnet.c				\
	host-serial.c

EXTRA_MODULE_VPATH := console-common

MODULE_CFLAGS = -DNOTIFY_OPEN=textcon-open -DNOTIFY_CLOSE=textcon-close \
		-I$(HYPERSCAN_ROOT)/include

MODULE_LDFLAGS += $(HYPERSCAN_LIB) $(LIBICONV) $(LIBSOCKET)
# rpath for libhs
MODULE_LDFLAGS += -Wl,-rpath,'$$ORIGIN/../sys/lib'

ifeq ($(findstring win, $(HOST_TYPE)),win)
SRC_FILES += pty-win32.c
EXTRA_DIST_SRC_FILES := pty-unix.c
else
SRC_FILES += pty-unix.c
EXTRA_DIST_SRC_FILES := pty-win32.c
endif

PYTHON_FILES := module_load.py simics_start.py

THREAD_SAFE:= yes
SIMICS_API := latest

include $(MODULE_MAKEFILE)
