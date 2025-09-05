# -*- Makefile -*-

# © 2021 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

# Simics interface module makefile

MODULE_CLASSES = probe_monitor probe_streamer probe_system_perfmeter

PYTHON_FILES = simics_start.py module_load.py sampler.py \
               monitor.py streamer.py system_perfmeter.py \
               logwindow.py commands.py sprobes.py common.py plot.py \
               presentation.py html_report.py html.py \
               probe_session.py \
               html_graphs/__init__.py \
               html_graphs/prop.py \
               html_graphs/common.py \
               html_graphs/graph.py \
               html_graphs/report_graphs.py

SIMICS_API := latest
THREAD_SAFE := yes

ifeq ($(MODULE_MAKEFILE),)
$(error Make sure you compile your module from the project directory)
endif

include $(MODULE_MAKEFILE)

