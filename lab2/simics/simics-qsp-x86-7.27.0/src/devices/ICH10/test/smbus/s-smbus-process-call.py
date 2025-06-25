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


# s-smbus-process-call.py
# tests the process call transfer command of ICH9 SMBus controller module

from smbus_tb import *

tb.enable_smbus_host(1)

def do_test():
    cmd_code = 0x66
    cmd_data1 = 0x88
    cmd_data2 = 0x99
    cmd_data3 = 0x33
    cmd_data4 = 0x44

    slave = tb.slaves[process_call_slave_addr]
    slave.caps_lock = 1
    slave.num_lock = 1
    slave.cmd_code = ~cmd_code
    slave.cmd_data1 = ~cmd_data1
    slave.cmd_data2 = ~cmd_data2
    slave.cmd_data3 = cmd_data3
    slave.cmd_data4 = cmd_data4

    tb.map_smbus_func(smbus_mapped_addr)
    # Set the slave address and the write
    tb.configure_transfer_paras(process_call_slave_addr, SmbusConst.smb_write,
                                cmd_code, cmd_data1, cmd_data2)
    # Set and start the smb command
    tb.start_smb_cmd(SmbusConst.cmd["process-call"])
    # Check the slave has toggled its capital lock state
    simics.SIM_continue(100)
    expect(slave.caps_lock, 0,
            "the capital lock state toggled in the process call SMBus slave device")
    expect(slave.num_lock, 0,
            "the number lock state toggled in the process call SMBus slave device")
    expect(slave.cmd_code, cmd_code,
            "the command code written into the process call SMBus slave device")
    expect(slave.cmd_data1, cmd_data1,
            "the command data 1 written into the process call SMBus slave device")
    expect(slave.cmd_data2, cmd_data2,
            "the command data 2 written into the process call SMBus slave device")
    reg_val = tb.read_io_le(tb.smbus_func_mapped_addr + 0x5, 8)
    expect(reg_val, cmd_data3,
            "the command data 1 read from the process call SMBus slave device")
    reg_val = tb.read_io_le(tb.smbus_func_mapped_addr + 0x6, 8)
    expect(reg_val, cmd_data4,
            "the command data 2 read from the process call SMBus slave device")
    expect(slave.stop_called, 1,
           "the count of stop called by the i2c link")
    expect(slave.stop_condition, [0],
            "the conditions of stops called by the i2c link")

    slave.cmd_code = ~cmd_code
    slave.cmd_data1 = ~cmd_data1
    slave.cmd_data2 = ~cmd_data2
    slave.cmd_data3 = cmd_data3
    slave.cmd_data4 = cmd_data4

    # Set the slave address and the read
    tb.configure_transfer_paras(process_call_slave_addr, SmbusConst.smb_read, cmd_code, cmd_data1, cmd_data2)
    # Set and start the smb command
    tb.start_smb_cmd(SmbusConst.cmd["process-call"])
    # Check the slave has toggled its number lock state
    simics.SIM_continue(100)
    expect(slave.caps_lock, 1,
            "the capital lock state toggled in the process call SMBus slave device")
    expect(slave.num_lock, 1,
            "the number lock state toggled in the process call SMBus slave device")
    expect(slave.cmd_code, cmd_code,
            "the command code written into the process call SMBus slave device")
    expect(slave.cmd_data1, cmd_data1,
            "the command data 1 written into the process call SMBus slave device")
    expect(slave.cmd_data2, cmd_data2,
            "the command data 2 written into the process call SMBus slave device")
    reg_val = tb.read_io_le(tb.smbus_func_mapped_addr + 0x5, 8)
    expect(reg_val, cmd_data3,
            "the command data 1 read from the process call SMBus slave device")
    reg_val = tb.read_io_le(tb.smbus_func_mapped_addr + 0x6, 8)
    expect(reg_val, cmd_data4,
            "the command data 2 read from the process call SMBus slave device")
    expect(slave.stop_called, 2,
           "the count of stop called by the i2c link")
    expect(slave.stop_condition, [0, 0],
            "the conditions of stops called by the i2c link")

do_test()
