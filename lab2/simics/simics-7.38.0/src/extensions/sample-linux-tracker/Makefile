# -*- Makefile -*-

# © 2016 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

THREAD_SAFE := yes
SIMICS_API := latest
MODULE_CLASSES := sample_linux_tracker sample_linux_mapper \
	sample_linux_tracker_comp
TRACKER_FILES = sample-linux-tracker.c \
	sample-linux-tracker-control-iface.c
MAPPER_FILES = sample-linux-mapper.c sample-linux-mapper-admin-iface.c \
	sample-linux-mapper-control-iface.c \
	sample-linux-mapper-query-iface.c sample-linux-tracker-monitor.c \
	sample-linux-tracker-task-struct.c sample-linux-tracker-params.c
SRC_FILES = $(TRACKER_FILES) $(MAPPER_FILES)
PYTHON_FILES = module_load.py sample_linux_tracker_comp.py

include $(MODULE_MAKEFILE)
