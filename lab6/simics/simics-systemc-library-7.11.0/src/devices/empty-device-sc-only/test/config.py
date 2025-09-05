# © 2015 Intel Corporation

from simics import SIM_create_object

def create_test_device(num_transactions = 10):
    dut = SIM_create_object('empty_device_sc_only', 'dut',
                            [['argv', [str(num_transactions)]]])
    return dut
