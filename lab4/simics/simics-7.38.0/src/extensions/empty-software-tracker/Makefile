# -*- Makefile -*-

# © 2016 Intel Corporation

THREAD_SAFE := yes
SIMICS_API := 7
MODULE_CLASSES := empty_software_tracker empty_software_mapper \
	empty_software_tracker_comp
TRACKER_FILES = empty-software-tracker.c \
	empty-software-tracker-control-iface.c
MAPPER_FILES = empty-software-mapper.c empty-software-mapper-admin-iface.c \
	empty-software-mapper-control-iface.c \
	empty-software-mapper-query-iface.c
SRC_FILES = $(TRACKER_FILES) $(MAPPER_FILES)
PYTHON_FILES = module_load.py empty_software_tracker_comp.py

include $(MODULE_MAKEFILE)
