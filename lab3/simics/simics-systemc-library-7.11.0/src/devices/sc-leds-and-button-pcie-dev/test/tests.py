# © 2015 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import glob
import simicsutils

is_windows = simicsutils.host.is_windows()
# The following subtests have been disabled due to intermittent failures on
# Windows (SIMICS-16249)
disabled_tests = ["s-trace.py"]

def tests(suite):
    for test in glob.glob("s-*.py"):
        if is_windows and test in disabled_tests:
            continue
        suite.add_simics_test(test)
