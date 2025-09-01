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

MODULE_CLASSES = sample-risc sample-risc-core
MODULE_CFLAGS += -DSAMPLE_RISC_CPU_CLASSNAME=sample-risc
MODULE_CFLAGS += -DSAMPLE_RISC_CORE_CLASSNAME=sample-risc-core

SRC_FILES += sample-risc.c            \
             sample-risc-core.c       \
             sample-risc-queue.c      \
             sample-risc-frequency.c  \
             sample-risc-step.c       \
             sample-risc-cycle.c      \
             sample-risc-memory.c     \
             sample-risc-exec.c       \
             event-queue.c            \
             toy-risc-core.c

SRC_FILES += sample-risc-direct-memory.c

PYTHON_FILES = module_load.py

EXTRA_MODULE_VPATH = cycle-common

# for simics/util/fp-init.h
MODULE_CFLAGS += -I$(SIMICS_REPO_ROOT)/core/src/include

SRC_FILES += generic-cycle-iface.c

SIMICS_API := latest
THREAD_SAFE:= yes

# The dist files generation script does not know that we run tests
# from another directory, so we manually specify the test files here
SAMPLE_RISC_TEST_FILES = \
  test/common.py \
  test/s-add.py \
  test/s-branch-direct.py \
  test/s-branch-indirect.py \
  test/s-config-sanity.py \
  test/s-exec-bp-logical.py \
  test/s-exec-bp-physical-nested.py \
  test/s-exec-bp-physical.py \
  test/s-load-stall.py \
  test/s-load.py \
  test/s-nop.py \
  test/s-read-bp-logical.py \
  test/s-read-bp-physical-nested.py \
  test/s-read-bp-physical.py \
  test/s-store.py \
  test/s-write-bp-logical.py \
  test/s-write-bp-physical-nested.py \
  test/s-write-bp-physical.py

EXTRA_DIST_SRC_FILES := $(SAMPLE_RISC_TEST_FILES)

include $(MODULE_MAKEFILE)
-include $(firstword $(filter %cycle-common,$(VPATH)))/generic-cycle-iface.mk
