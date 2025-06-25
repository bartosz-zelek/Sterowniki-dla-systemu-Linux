# © 2012 Intel Corporation
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
                                 sector_size     = 0x10000,
                                 sector_number   = 8,
                                 elec_signature  = 0x12,
                                 JEDEC_signature = [0x01, 0x20, 0x18])

testbench.test_elec_signature()
testbench.test_elec_signature(expected=0x11, result=False)
testbench.test_flash_status()
testbench.test_read_write_flash()
testbench.test_page_write_flash()
testbench.test_page_erase()
testbench.test_subsector_erase()
testbench.test_sector_erase()
testbench.test_bulk_erase()
testbench.test_jedec_id()
testbench.test_sector_protection(2, 6)
testbench.test_program_protection(0, 8)
testbench.test_program_protection(1, 7)
testbench.test_program_protection(2, 6)
testbench.test_operation_with_wrong_state(
    4 * 0x10000, [x & 0xFF for x in range(128)]) #unprotected sector
testbench.test_program_protection(3, 4)
testbench.test_program_protection(4, -1)
testbench.test_program_protection(5, -1)
testbench.test_program_protection(6, -1)
testbench.test_program_protection(7, -1)
testbench.test_spm1_protection(6)
