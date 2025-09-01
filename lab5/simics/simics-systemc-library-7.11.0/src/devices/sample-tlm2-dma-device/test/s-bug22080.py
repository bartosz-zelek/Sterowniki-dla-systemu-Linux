# © 2010 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import dev_util
import config
import stest

dma_space = SIM_create_object('memory-space', 'dma_space')

dma, regs = config.create_dma_device('dma', dma_space)
simple_device, _ = config.create_dma_device('simple_device')

dma_space.map = [[0x1000, dma, 0, 0, 8], [0x2000, simple_device, 0, 0, 8]]
# Update the align_size to 4
dma_space.map[0][7] = 4
dma_space.map[1][7] = 4

# Do dma to/from the simple_device registers
regs.dma_source.write(0x2000)
regs.dma_destination.write(0x2000)

# TS=2 set dma size to 8 bytes
with stest.expect_log_mgr(log_type="error"):
    # The align_size based splitting of transactions only applies to targets
    # implementing the generic_transaction_t based interfaces port_space,
    # io_memory, translate, and bridge
    regs.dma_control.write(0, EN=1, SWT=1, TS=2)
