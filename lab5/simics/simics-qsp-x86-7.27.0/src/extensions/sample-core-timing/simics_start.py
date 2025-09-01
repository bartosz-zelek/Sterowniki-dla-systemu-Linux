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


import instrumentation

instrumentation.make_tool_commands(
    "sample_core_timing_tool",
    object_prefix = "sample_core_timing",
    provider_requirements = "x86_cstate & x86_instrumentation_subscribe",
    make_info_cmd = True,
    make_status_cmd = False,
    make_filter_cmds = False,
    new_cmd_doc = """Creates a new sample-core-timing tool which
    can be connected to x86 cores. """)
