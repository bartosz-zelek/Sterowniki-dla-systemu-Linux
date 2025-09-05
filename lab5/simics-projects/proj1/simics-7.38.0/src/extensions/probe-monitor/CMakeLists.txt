# © 2024 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

simics_add_module(probe-monitor
  CLASSES probe_monitor probe_streamer probe_system_perfmeter
  SOURCES commands.py common.py html.py html_report.py logwindow.py module_load.py monitor.py plot.py presentation.py sampler.py simics_start.py sprobes.py streamer.py system_perfmeter.py probe_session.py html_graphs/__init__.py html_graphs/prop.py html_graphs/common.py html_graphs/graph.py html_graphs/report_graphs.py
  SIMICS_API latest
)
