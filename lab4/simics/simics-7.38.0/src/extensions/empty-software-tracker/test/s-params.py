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

import cli
import stest
import empty_software_tracker_common as common

sw = common.setup_test_system()

comp_name = "empty_software_tracker_comp"

params_file = stest.scratch_file("skeleton.params")
cli.run_command("%s.save-parameters %s" % (sw.name, params_file))

# Delete the tracker and test that it can be recreated by using the parameters
# file created above.
cli.run_command("%s.delete-tracker" % sw.name)
stest.expect_false(hasattr(sw, "tracker"))
cli.run_command("%s.load-parameters %s" % (sw.name, params_file))
stest.expect_different(sw.tracker, None)
stest.expect_equal(sw.tracker.classname, comp_name)
