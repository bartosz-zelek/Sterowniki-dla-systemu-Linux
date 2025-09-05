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


# Test that it is possible to disable and delete a tracker and mapper which has
# some state.

import cli
import simics
import stest
import sample_linux_tracker_common as common

(sw, console) = common.setup_test_system()

(ok, rid_or_msg) = sw.iface.osa_control_v2.request("test")
stest.expect_true(ok, rid_or_msg)

common.boot_system(console)

sw.iface.osa_control_v2.release(rid_or_msg)

cli.run_command("board.software.delete-tracker")
