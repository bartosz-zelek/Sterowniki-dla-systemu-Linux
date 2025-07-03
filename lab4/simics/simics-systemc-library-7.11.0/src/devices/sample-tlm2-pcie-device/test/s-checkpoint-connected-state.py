# © 2024 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

import common
import simics
import stest

dev = common.create_sample_pcie_device()
up = common.create_upstream_target(fake=False, name="up")
up.devices = [[0x40, conf.dev]]
stest.expect_equal(up.cfg_space.map,
                   [[0x400000, dev.port.cfg0, 0, 0, 65536, None, 0, 0, 0],
                    [0x450000, dev.port.cfg5, 0, 0, 65536, None, 0, 0, 0]])

common.save_checkpoint("connected-state")
simics.SIM_run_command("restart-simics read-configuration-connected-state.py")
