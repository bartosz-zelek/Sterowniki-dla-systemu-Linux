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


import stest
import config

dma, regs = config.create_dma_device()

stest.expect_equal(regs.dma_control.read(), 0)
stest.expect_equal(regs.dma_source.read(), 0)
stest.expect_equal(regs.dma_destination.read(), 0)
