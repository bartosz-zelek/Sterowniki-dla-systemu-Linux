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
import stest

dev = common.create_sample_pcie_device()

# the trigger will initiate an size bytes read from src_addr and write that
# data back to dst_addr, so that we can test that our mem object now
# contains both the original and the copied data
size = 4
src_addr = 0x1000
dst_addr = 0x2000

up = common.create_upstream_target()
dev.upstream_target = up
dev.dma_trigger = [src_addr, dst_addr, size]

stest.expect_equal(up.object_data.issue,
                   [(src_addr, simics.PCIE_Type_Mem, 4, True), # Read
                    (dst_addr, simics.PCIE_Type_Mem, 4, False)]) # Write
