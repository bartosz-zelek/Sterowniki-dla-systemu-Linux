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

# Simics module makefile

MODULE_CLASSES = rn-eth-bridge-tap

MODULE_COMPONENTS = real_network_host_comp real_network_bridge_comp

SRC_FILES=					\
	eth-adapter.c				\
	eth-bridge-impl.c			\
	eth-bridge.c				\
	real-network.c				\
	rn-tun-tap-common.c			\
	net-utils.c				\
	ip-addr.c

PYTHON_FILES = module_load.py simics_start.py

MODULE_LDFLAGS= $(LIBSOCKET) $(LIBPTHREAD) $(LIBWIN32) $(LIBIPHLPAPI)

ifeq ($(findstring win, $(HOST_TYPE)),win)
SRC_FILES += lowlevel-win32.c lowlevel-tapwin32.c rn-tap-win32.c
EXTRA_DIST_SRC_FILES := lowlevel-unix.c rn-tun-tap-unix.c
else
SRC_FILES += lowlevel-unix.c rn-tun-tap-unix.c
EXTRA_DIST_SRC_FILES := lowlevel-win32.c \
			lowlevel-tapwin32.c rn-tap-win32.c
endif

EXTRA_MODULE_VPATH = network-common

SIMICS_API := latest
THREAD_SAFE:= yes

include $(MODULE_MAKEFILE)
