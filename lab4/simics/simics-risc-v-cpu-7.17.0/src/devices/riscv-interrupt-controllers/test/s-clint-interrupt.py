# © 2019 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


from riscv_intc_common import create_tb
import simics
import stest
import random
random.seed("Poppy")

tb = create_tb()

harts = [h.obj for h in tb.harts]

for (hart, msip) in zip(tb.harts, tb.clint.msip):
    stest.expect_false(hart.obj.port.MSIP.state)
    msip.write(1)
    stest.expect_true(hart.obj.port.MSIP.state)
    msip.write(0)
    stest.expect_false(hart.obj.port.MSIP.state)

for reg in tb.clint.mtimecmp:
    reg.write(-1)

for (hart, mtimecmp) in zip(tb.harts, tb.clint.mtimecmp):
    stest.expect_false(hart.obj.port.MTIP.state)
    count = random.randint(1, 1000)
    mtimecmp.write(tb.clint.mtime.read() + count)
    stest.expect_false(hart.obj.port.MTIP.state)
    simics.SIM_continue(count - 1)
    stest.expect_false(hart.obj.port.MTIP.state)
    simics.SIM_continue(1)
    stest.expect_true(hart.obj.port.MTIP.state)
