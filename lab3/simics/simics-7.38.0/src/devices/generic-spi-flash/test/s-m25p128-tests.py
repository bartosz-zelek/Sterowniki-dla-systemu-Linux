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


import test_bench

#run_command('log-level 4')
testbench = test_bench.TestBench(#bp_mask         = 0x7,
                                 sector_size     = 0x40000,
                                 sector_number   = 64,
                                 elec_signature  = 0x16,
                                 JEDEC_signature = [0x20, 0x20, 0x18])

testbench.test_elec_signature()
testbench.test_elec_signature(expected=0x11, result=False)
testbench.test_flash_status()
testbench.test_read_write_flash()
testbench.test_sector_erase()
testbench.test_bulk_erase()
testbench.test_jedec_id()
testbench.test_jedec_id([0x20, 0x20, 0x17], result=False)
testbench.test_sector_protection(2, 62)
testbench.test_program_protection(0, 64)
testbench.test_program_protection(1, 63)
testbench.test_program_protection(2, 62)
testbench.test_program_protection(3, 60)
testbench.test_program_protection(4, 56)
testbench.test_program_protection(5, 48)
testbench.test_program_protection(6, 32)
testbench.test_operation_with_wrong_state(
    4 * 0x40000, [x & 0xFF for x in range(128)]) #unprotected sector
testbench.test_program_protection(7, -1)
