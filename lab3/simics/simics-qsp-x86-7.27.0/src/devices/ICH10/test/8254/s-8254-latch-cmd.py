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


# s-8254-latch-cmd.py
# test the latch command of the 8254 timer
# in the LPC bridge in the ICH9 chip

from timer_tb import *

def do_test(counters, lsb, msb):
    initial_cnt = 0xFF
    cont_len    = 100
    for i in range(lpc_counter_cnt):
        tb.signal[i].level = 0

    for c in counters:
        ctrl_val = (c << 6) + ((msb * 2 + lsb) << 4) + 0x6
        tb.write_io_le(0x43, 8, ctrl_val)

        io_addr = 0x40 + c
        tb.write_io_le(io_addr, 8, initial_cnt)

        if msb == 1 and lsb == 1:
            tb.write_io_le(io_addr, 8, initial_cnt)

        expect(tb.signal[c].level, 1, "initial output from the counter %d" % c)

    SIM_continue(cont_len)

    # Latch the count of tested counters
    for c in counters:
        ctrl_val = (c << 6) + (0 << 5)
        tb.write_io_le(0x43, 8, ctrl_val)

    # Compute the expected value
    if lsb == 1 and msb == 0:
        exp_lsb = initial_cnt - cont_len
        exp_msb = 0
    elif lsb == 0 and msb == 1:
        exp_lsb = 0
        exp_msb = initial_cnt - 1 # 100 cycles will borrow one from MSB
    elif lsb == 1 and msb == 1:
        exp_lsb = initial_cnt - cont_len
        exp_msb = initial_cnt

    for c in counters:
        io_addr = 0x40 + c
        first_val = tb.read_io_le(io_addr, 8)
        if lsb == 1:
            expect_hex(
                first_val, exp_lsb, "least significant byte of counter %d" % c)
        else:
            expect_hex(
                first_val, exp_msb, "most significant byte of counter %d" % c)

        if (lsb == 1 and msb == 1):
            sec_val = tb.read_io_le(io_addr, 8)
            expect_hex(
                sec_val, exp_msb, "most significant byte of counter %d" % c)

do_test([0, 1, 2], 1, 1)
do_test([0, 1, 2], 1, 0)
do_test([0, 1, 2], 0, 1)

do_test([2], 1, 1)
do_test([2], 1, 0)
do_test([2], 0, 1)
