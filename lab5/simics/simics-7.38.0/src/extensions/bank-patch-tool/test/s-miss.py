# © 2018 Intel Corporation
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
import stest

from dev_util import MemoryError
from simics import SIM_get_object

import conf
# SIMICS-21543
conf.sim.deprecation_level = 0

# This test is more or less a sanity check to make sure that the
# devices we use to test the tool don't change unexpectedly
((_, device), (_, dma_device)) = (common.create_sample_device(),
                                  common.create_dma_device())

for (obj, (hit, miss)) in ((device, common.sample_registers(device)),
                           (dma_device, common.dma_registers(dma_device))):
    hit.read()
    with stest.expect_log_mgr(obj.bank.regs, 'spec-viol'):
        stest.expect_exception(miss.read, [], MemoryError)
