# © 2014 Intel Corporation
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
import simics
import conf

def test_common():
    # Check that commands do not crash
    simics.SIM_run_command("board.mb.cpu0.tlb[0][0].info")
    simics.SIM_run_command("board.mb.cpu0.tlb[0][0].status")

    def test_transaction(tlb, la, supr, write, expect_hit):
        trans = simics.x86_memory_transaction_t()
        trans.linear_address = la
        if supr:
            trans.mode = simics.Sim_CPU_Mode_Supervisor
        else:
            trans.mode = simics.Sim_CPU_Mode_User
        trans.fault_as_if_write = False
        if write:
            simics.SIM_set_mem_op_type(trans.s, simics.Sim_Trans_Store)
        else:
            simics.SIM_set_mem_op_type(trans.s, simics.Sim_Trans_Load)
        hit = tlb.iface.x86_tlb.lookup(trans)
        stest.expect_equal(hit, expect_hit)
        print("Test 0x%x success" % la)

    tlb = conf.board.mb.cpu0.tlb[0][0]

    test_transaction(tlb, 0xffffffff81200000, True, False, True)
    test_transaction(tlb, 0xffffffff81200000, True, True, False)
    test_transaction(tlb, 0xffffffff81200000, False, False, False)
    test_transaction(tlb, 0xffffffff81200000, False, True, False)
    test_transaction(tlb, 0xffff8881f5800000, True, False, True)
    test_transaction(tlb, 0xffff8881f5800000, True, True, False)
    test_transaction(tlb, 0xffff8881f5800000, False, False, False)
    test_transaction(tlb, 0xffff8881f5800000, False, True, False)
    test_transaction(tlb, 0xffff8881f6c00000, True, False, True)
    test_transaction(tlb, 0xffff8881f6c00000, True, True, True)
    test_transaction(tlb, 0xffff8881f6c00000, False, False, False)
    test_transaction(tlb, 0xffff8881f6c00000, False, True, False)
