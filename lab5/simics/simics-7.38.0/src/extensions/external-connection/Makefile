# © 2021 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

MODULE_CLASSES := tcp-server

SIMICS_API := latest
THREAD_SAFE := yes

PYTHON_FILES := module_load.py
SRC_FILES := init.c tcp.c

ifneq ($(findstring win,$(HOST_TYPE)),win)
SRC_FILES += unix-socket.c
MODULE_CLASSES += unix-socket-server
else
SRC_FILES += named-pipe.c
MODULE_CLASSES += named-pipe-server
MODULE_CFLAGS += -D_WIN32_WINNT=0x0A00 -DWIN32_LEAN_AND_MEAN
MODULE_LDFLAGS := $(LIBWIN32) $(LIBSOCKET)
endif

include $(MODULE_MAKEFILE)
