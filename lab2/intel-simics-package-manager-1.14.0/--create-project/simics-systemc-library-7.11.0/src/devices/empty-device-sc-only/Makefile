# -*- Makefile ; coding: utf-8 -*-

# © 2015 Intel Corporation

# Simics module makefile

MODULE_CLASSES = empty_device_sc_only
SRC_FILES = sc_factory.cc sc-device.cc simics-main.cc
PYTHON_FILES = module_load.py
EXTRA_DIST_SRC_FILES = sc-main.cc

SIMICS_API := 7
THREAD_SAFE = yes
SYSTEMC = yes
_SYSTEMC_GASKET_CLASSES = no

# Prefer Visual Studio on Windows
COMPILERS = cl gcc

include $(MODULE_MAKEFILE)
