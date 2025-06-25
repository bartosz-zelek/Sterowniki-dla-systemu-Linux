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

from cli import run_command
from simics import SIM_get_object
from stest import expect_equal

import conf
# SIMICS-21543
conf.sim.deprecation_level = 0

(sample_device_name, sample_device) = common.create_sample_device()
(dma_device_name, dma_device) = common.create_dma_device()

sample_device_bank = common.bank_name(sample_device_name)
dma_device_bank = '%s.bank.regs' % dma_device_name
run_command('new-bank-patch-tool banks=%s %s offset=0 size=0' % (
    sample_device_bank, dma_device_bank))

for (_, miss) in (common.sample_registers(sample_device),
                  common.dma_registers(dma_device)):
    miss.read()
