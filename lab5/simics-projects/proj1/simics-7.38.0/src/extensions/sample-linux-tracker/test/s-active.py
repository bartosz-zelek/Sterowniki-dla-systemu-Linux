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


import stest
import sample_linux_tracker_common as common

(sw, console) = common.setup_test_system()
tracker = sw.tracker.tracker_obj
mapper = sw.tracker.mapper_obj
active_ok = False

common.boot_system(console)

console.command("simics-agent&")
common.upload_binary(".", "active", "/usr/bin")

(rc, rid_or_msg) = sw.iface.osa_control_v2.request("Test")
stest.expect_true(rc, rid_or_msg)

common.test_active(sw, console)
