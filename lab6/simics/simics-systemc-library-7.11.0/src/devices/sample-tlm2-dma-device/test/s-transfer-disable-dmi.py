# © 2015 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import transfer
import simics
import stest

def clear_dma_counters():
    # Verify and clear DMA counters not related to this subtest
    stest.expect_equal(transfer.counters['read_count'], 2)
    transfer.counters['read_count'] = 0
    stest.expect_equal(transfer.counters['dmi_access_count'], 2)
    transfer.counters['dmi_access_count'] = 0
    stest.expect_equal(transfer.counters['write_count'], 2)
    transfer.counters['write_count'] = 0

dma_image = simics.SIM_create_object('image', 'dma_image', size=1 << 20)
dma_ram = simics.SIM_create_object('ram', 'dma_ram', image=dma_image)
(dma, dma_space, mock, regs) = transfer.setup_test(dma_ram, transfer.on_log)

# initial transfer, sets up DMI table _in the gasket_ after first access to DMA
# device => only initial access to DST/SRC registers logged
transfer.reset_counters()
transfer.run_test(dma, dma_space, mock, regs)
clear_dma_counters()
stest.expect_equal(transfer.non_zero_counters(), [('dst_write_count', 1)])

# 2nd transfer, everything is DMI now => no output logged
transfer.reset_counters()
transfer.run_test(dma, dma_space, mock, regs)
clear_dma_counters()
stest.expect_equal(transfer.non_zero_counters(), [])


# 3rd transfer, disable DMI => all accesses are logged
run_command('dma.gasket_DMADevice_mmio.disable-dmi')
transfer.reset_counters()
transfer.run_test(dma, dma_space, mock, regs)
clear_dma_counters()
stest.expect_equal(transfer.non_zero_counters(), [('dst_read_count', 1),
                                                  ('dst_write_count', 1),
                                                  ('src_read_count', 1),
                                                  ('src_write_count', 1)])
