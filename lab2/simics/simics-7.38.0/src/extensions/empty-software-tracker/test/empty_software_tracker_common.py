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
import conf
import simics
import stest

# SIMICS-20853
conf.sim.deprecation_level = 0

def setup_test_system():
    cli.run_command("load-module empty-software-tracker")
    qsp_script = simics.SIM_lookup_file(
        '%simics%/targets/qsp-x86/qsp-linux-common.simics')
    stest.expect_different(qsp_script, None,
                           "Unable to locate qsp start script")
    simics.SIM_run_command_file(qsp_script, False)
    cli.run_command("board.software.delete-tracker")

    cli.run_command("%s.insert-tracker tracker = empty_software_tracker_comp"
                    % get_sw().name)

    return conf.board.software

def get_checkpoint_file():
    return stest.scratch_file("checkpoint")

def get_sw():
    return conf.board.software
