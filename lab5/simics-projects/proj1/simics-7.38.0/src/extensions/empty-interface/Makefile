# -*- Makefile -*-

# © 2010 Intel Corporation

# Simics interface module makefile

# IFACE_FILES should list all .h files that should be used to produce
# Python support for its interfaces and structs. If you do not want
# any Python support, you should remove the entire Makefile.
IFACE_FILES := empty-interface-interface.h

SIMICS_API := 7
THREAD_SAFE := yes

ifeq ($(MODULE_MAKEFILE),)
$(error Make sure you compile your module from the project directory)
endif

include $(MODULE_MAKEFILE)
