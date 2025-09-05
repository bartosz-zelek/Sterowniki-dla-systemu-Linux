# © 2014 Intel Corporation
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
import dev_util

import conf
# SIMICS-21634
conf.sim.deprecation_level = 0

# Load checkpoint in old format, see that the load works
run_command("read-configuration qsp-x86-booted")

new_conf = stest.scratch_file("new-conf")
print("Writing checkpoint as %s" % new_conf)
SIM_run_command('write-configuration %s' % new_conf)
