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


import transfer
import simics
import stest

dma_image = simics.SIM_create_object('image', 'dma_image', size=1 << 20)
dma_ram = simics.SIM_create_object('ram', 'dma_ram', image=dma_image)
(dma, dma_space, mock, regs) = transfer.setup_test(dma_ram, transfer.on_log)

def test():
    transfer.reset_counters()
    transfer.run_test(dma, dma_space, mock, regs)
    stest.expect_equal(transfer.non_zero_counters(), [('dmi_access_count', 2),
                                                      ('dst_write_count', 1),
                                                      ('read_count', 2),
                                                      ('write_count', 2)])

def test_dmi_consistency():
    # Bug 22049:
    # Test that DMI pointers are not kept when changing the memory space
    dma_image = simics.SIM_create_object('image', 'dma_image2',
                                         [['size', 1 << 20]])
    dma_ram = simics.SIM_create_object('ram', 'dma_ram2',
                                       [['image', dma_image]])
    dma_space = SIM_create_object('memory-space', 'dma_space2')
    dma_space.map = [[0x0, dma_ram, 0, 0, dma_image.size]]
    dma.phys_mem = dma_space
    # Run the test again with a different memory. Since the test
    # copies the contents from one address to another using the DMA
    # device and then checks the result, it will fail if the DMA
    # device has stale host pointers (since it will copy the wrong
    # memory).
    transfer.reset_counters()
    transfer.run_test(dma, dma_space, mock, regs)

    # As a sanity check, the number of DMI and read/write accesses
    # should be the same (but we should never get here if DMI is
    # broken). The number of dest updates should *not* increase since it
    # is accessible via inbound DMI (which has not been invalidated).
    stest.expect_equal(transfer.non_zero_counters(), [('dmi_access_count', 2),
                                                      ('read_count', 2),
                                                      ('write_count', 2)])

test()
test_dmi_consistency()
