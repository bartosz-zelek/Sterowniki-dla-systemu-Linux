# © 2013 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import config
import stest

def send_reset_signal(dev):
    dev.port.reset.iface.signal.signal_raise()
    SIM_continue(10**6)
    dev.port.reset.iface.signal.signal_lower()

dma, regs = config.create_dma_device('dma')
regs.dma_source.write(1)
regs.dma_destination.write(2)
regs.dma_control.write(3)
send_reset_signal(dma)
stest.expect_equal(regs.dma_source.read(), 0)
stest.expect_equal(regs.dma_destination.read(), 0)
stest.expect_equal(regs.dma_control.read(), 0)
