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

import cli
import stest
cli.global_cmds.run_script(script="s-uart-example.py")

stest.expect_equal(cli.quiet_run_command("print-target-info")[1].split(),
"""
UART example
   ┌─────────┬─────────────────────────────────────────────────┐
   │Namespace│board                                            │
   │System   │root object of blueprint created object hierarchy│
   │Processor│No processor                                     │
   │Memory   │0 B                                              │
   │Ethernet │None                                             │
   │Storage  │No disks                                         │
   └─────────┴─────────────────────────────────────────────────┘
""".split())
