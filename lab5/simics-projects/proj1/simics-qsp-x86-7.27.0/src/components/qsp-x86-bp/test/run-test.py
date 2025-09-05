# © 2025 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

from glob import glob
import testparams as tp

add_subtests = vars(tp).get("add_subtests", tp.run_subtests)
add_subtest = vars(tp).get("add_subtest", tp.run_subtest)

deprecation_tests = {'s-attach-usb-disk.simics'}
tests = set(glob("s-*.simics")) - deprecation_tests

for t in tests:
    add_subtest(t, timeout=1200)
for t in deprecation_tests:
    add_subtest(t, timeout=1200,
                extra_args=["-e", "@conf.sim.deprecation_level=0"])
add_subtests("s-*.py", timeout=1200)
