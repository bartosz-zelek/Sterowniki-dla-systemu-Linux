# -*- Makefile -*-

# © 2023 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

MODULE_CLASSES = c_test_probe py_test_probe py_array_test_probe
SRC_FILES = probe-c-impl.c
PYTHON_FILES = module_load.py probe_py_impl.py
SIMICS_API := 6
THREAD_SAFE = yes

include $(MODULE_MAKEFILE)
