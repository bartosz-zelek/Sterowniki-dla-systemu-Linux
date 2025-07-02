# © 2010 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import sys
import os
import testparams
import glob

def tests(suite):
    os.environ['SAMPLE_RISC_CLASS'] = 'sample-risc'
    os.environ['SAMPLE_RISC_CORE_CLASS'] = 'sample-risc-core'

    imported_test_path = os.path.join("..", "..", "sample-risc", "test")
    pattern = os.path.join(imported_test_path, 's-*.py')
    tests = sorted(glob.glob(pattern))
    if not tests:
        suite.add_test("no-subtests", no_subtests)

    quoted_path = imported_test_path.replace("\\", "\\\\")
    for script in sorted(tests):
        test = script.split(os.sep)[-1]
        suite.add_simics_test(
            script, name = test.replace(".py", ""),
            extra_args = [
                '-e', 'cd "%s"' % quoted_path,
                '-e', f'@sys.path.append(r"{imported_test_path}")'])
