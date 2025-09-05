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

MODULE_CLASSES = g-cache trans-splitter trans-sorter

SRC_FILES = gc-common.c gc-common-attributes.c			\
	    gc-random-repl.c gc-lru-repl.c gc-cyclic-repl.c	\
	    gc-attributes.c gc-interface.c gc.c			\
	    splitter.c sorter.c scalar-time.c

PYTHON_FILES := gcache_common.py module_load.py simics_start.py

MODULE_CFLAGS = -DGC_USE_STC # -DGC_DEBUG
EXTRA_MODULE_VPATH = scalar-time-common

THREAD_SAFE:= yes
SIMICS_API := latest

EXTRA_DIST_SRC_FILES:=gc-lru2-repl.c gc-lru2-repl.h

include $(MODULE_MAKEFILE)
