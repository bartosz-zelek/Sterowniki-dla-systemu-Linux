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

common.create_sample_pcie_device()
conf.dev.base_address = [0xc, 0x10, 0, 0, 0, 0, 0, 0xc, 0, 0, 0, 0, 0, 0]
stest.expect_equal(conf.dev.base_address,
                   [0xc, 0x10, 0, 0, 0, 0, 0, 0xc, 0, 0, 0, 0, 0, 0])
common.save_checkpoint("init-state")
simics.SIM_run_command("restart-simics read-configuration-init-state.py")
