# -*- Makefile -*-

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

MODULE_CLASSES = tcp_server connected_device

SRC_FILES= tcp-server.c sock.c notifier-example.c connected-device.dml

SIMICS_API := latest
THREAD_SAFE := yes

MODULE_LDFLAGS = $(LIBSOCKET)

# Files not automatically included in dist.list
EXTRA_DIST_SRC_FILES:= demo_config.py

include $(MODULE_MAKEFILE)
