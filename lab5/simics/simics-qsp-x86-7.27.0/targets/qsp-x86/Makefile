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

CC ?= /opt/itm/apps/gcc/4.9.2/bin/gcc
CFLAGS ?= -g -Wall -Werror -I ../../core/src/include -std=c99
LDFLAGS ?= -lpthread

DEBUG_EXAMPLE = images/debug_example
WORKER = images/worker
WORKER_OPT = images/worker-opt

all:

/tmp/%.c: %.c
	cp $< $@

$(DEBUG_EXAMPLE): /tmp/debug_example.c
	$(CC) $(CFLAGS) -O0 $< -o $@
	rm $<

$(WORKER): /tmp/worker.c
	$(CC) $(LDFLAGS) $(CFLAGS) $< -o $@
	rm $<

$(WORKER_OPT): /tmp/worker_opt.c
	$(CC) $(LDFLAGS) $(CFLAGS) $< -o $@
	rm $<

all: $(DEBUG_EXAMPLE) $(WORKER) $(WORKER_OPT)

clean-worker-opt:
	rm -f $(WORKER_OPT)

clean-worker:
	rm -f $(WORKER)

clean-debug-example:
	rm -f $(DEBUG_EXAMPLE)

clean: clean-worker clean-worker-opt clean-debug-example
