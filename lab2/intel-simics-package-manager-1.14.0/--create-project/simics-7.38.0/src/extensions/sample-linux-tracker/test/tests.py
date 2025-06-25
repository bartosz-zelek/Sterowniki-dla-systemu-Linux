# © 2018 Intel Corporation
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

# Tests that boot Linux to prompt, can take some time.
slow_tests = (
    "s-checkpoint-tracker-0.py",
    "s-delete.py",
    "s-reverse-execution.py",
    "s-setup-system.py",
)

def tests(suite):
    for test in sorted(glob.glob("s-*.py")):
        if test in slow_tests:
            timeout = 300
        else:
            timeout = 120
        suite.add_simics_test(test, timeout=timeout)
