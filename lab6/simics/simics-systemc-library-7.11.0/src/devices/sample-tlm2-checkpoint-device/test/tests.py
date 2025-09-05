# © 2016 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


def tests(suite):
    suite.add_simics_tests('callbacks.py')
    suite.add_simics_tests('save_states.py')
    suite.add_simics_tests('load_state_0.py')
    suite.add_simics_tests('load_state_1.py')
    suite.add_simics_tests('load_state_2.py')
    suite.add_simics_tests('load_state_3.py')
    suite.add_simics_tests('snapshots.py')
    suite.add_simics_tests('save_fail.py')
    suite.add_simics_tests('checkpoint_chdir.py')
    suite.add_simics_tests('write_multiple.py')
    suite.add_simics_tests('load_multiple.py')
