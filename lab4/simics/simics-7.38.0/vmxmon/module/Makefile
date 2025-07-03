# -*- Makefile ; coding: utf-8 -*-

# © 2006 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

QUIET			:= @
ifdef V
QUIET			:=
endif

ifeq ($(KERNELRELEASE),)
$(error This Makefile is *only* for building the vmxmon kernel module)
endif

# Versioning of RHEL kernels is not always simple. RHEL_RELEASE can be a number
# - X, two numbers - X.Y (so called "early y+1 numbering"), or even three
# numbers - X.Y.Z (so called zstream numbering). Please see Makefile.rhelver and
# https://www.redhat.com/en/blog/code-path-how-does-new-code-enter-red-hat-products
# for more information. Below we extract the X part of RHEL_RELEASE as a
# number(!) into RHEL_RELEASE_AS_NUM that is then used for conditional
# compilation in the vmxmon's source code. Since RHEL_RELEASE variable is not
# exported to this Makefile we use tricks to get RHEL_RELEASE_AS_NUM. Please
# simplify/rewrite the code below if you can.
RHEL_RELEASE_AS_NUM := $(shell echo RHEL_RELEASE \
	| $(CC) -nostdinc $(LINUXINCLUDE) -include linux/version.h -E -P -x c - 2>/dev/null)

ifneq ($(RHEL_RELEASE_AS_NUM),RHEL_RELEASE)
RHEL_RELEASE_AS_NUM := $(subst $\",,$(RHEL_RELEASE_AS_NUM))  # strip quotes
# Handle X.Y and X.Y.Z:
RHEL_RELEASE_AS_NUM := $(word 1, $(subst ., ,$(RHEL_RELEASE_AS_NUM)))
ccflags-y       += -DRHEL_RELEASE_AS_NUM=$(RHEL_RELEASE_AS_NUM)
else
# We get here if there is no RHEL_RELEASE define in linux/version.h which means
# it is not a RHEL kernel. In this case RHEL_RELEASE_AS_NUM is left undefined.
endif


ccflags-y	+= -I$(src)/common -I$(src)/include -I$(src)/linux
ccflags-y	+= -std=gnu11 -Wno-declaration-after-statement

obj-m		:= vmxmon.o
vmxmon-y	:= linux/module.o linux/api.o \
               linux/page.o linux/nmi.o \
               common/dbg.o common/pctr.o common/tlb.o \
               common/mm.o common/vm.o common/tphys.o common/nested-ept.o \
               common/vmx.o common/dev.o common/entry.o common/fw-dbg.o \
               common/ept.o common/host-instructions.o

$(obj)/common/entry.o: $(obj)/common/asm-offsets.h

$(obj)/common/asm-offsets.h: $(obj)/common/tmp-offsets.s
	$(QUIET)grep "\.set " $< > $@

$(obj)/common/tmp-offsets.s: $(obj)/common/asm-offsets.c $(obj)/common/vm.h \
		$(obj)/common/vmx.h $(obj)/include/vmxmon.h
	$(QUIET)echo "  GEN	  $@"
	$(QUIET)$(CC) -D__KERNEL__ $(LINUXINCLUDE) -I$(obj)/include \
                -S -o $@ $<
