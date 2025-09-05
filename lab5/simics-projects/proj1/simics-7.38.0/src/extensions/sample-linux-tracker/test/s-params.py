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


import filecmp
import cli
import stest
import sample_linux_tracker_common as common

(sw, _) = common.setup_test_system()

comp_name = "sample_linux_tracker_comp"


detected_file = stest.scratch_file("detect.params")
cli.run_command("%s.tracker.detect-parameters %s" % (sw.name, detected_file))

# Delete the tracker and test that it can be recreated by using the parameters
# file created above.
cli.run_command("%s.delete-tracker" % sw.name)
stest.expect_false(hasattr(sw, "tracker"))
cli.run_command("%s.load-parameters %s" % (sw.name, detected_file))
stest.expect_different(sw.tracker, None)
stest.expect_equal(sw.tracker.classname, comp_name)

# Save the parameters and test that it is equal to the detected parameters
saved_file = stest.scratch_file("saved.params")
cli.run_command("%s.save-parameters %s" % (sw.name, saved_file))

stest.expect_true(filecmp.cmp(saved_file, detected_file))

# Test name option to detect-parameters to see that this is saved in
# parameters and that root node gets this name for loaded parameters.
new_name = "New Root"
detected_with_name_file = stest.scratch_file("detect_with_name.params")
saved_with_name_file = stest.scratch_file("saved_with_name.params")
cli.run_command('%s.tracker.detect-parameters param-file=%s name="%s"'
                % (sw.name, detected_with_name_file, new_name))

cli.run_command("%s.load-parameters %s" % (sw.name, detected_with_name_file))

cli.run_command("%s.save-parameters %s" % (sw.name, saved_with_name_file))
stest.expect_true(filecmp.cmp(saved_with_name_file, detected_with_name_file))

(ok, rid_or_msg) = sw.iface.osa_control_v2.request("test")
stest.expect_true(ok, rid_or_msg)

nt_query = sw.iface.osa_node_tree_query
root_props = nt_query.get_node(common.root_node(sw))
stest.expect_equal(root_props.get("name"), new_name)
