# © 2018 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import dev_util
import simics
import stest

from cli import run_command

def create_clock(freq_mhz = 1):
    if not create_clock.clock:
        create_clock.clock = simics.SIM_create_object(
            'clock', 'clk', [['freq_mhz', 1]])
    return create_clock.clock
create_clock.clock = None

class MockSignal(dev_util.Signal):
    pass

def device_name():
    device_name.num += 1
    return 'sample_device%s' % device_name.num
device_name.num = -1

def create_sample_device():
    name = device_name()
    return (name, simics.SIM_create_object('sample_device_dml', name,
                                           [['queue', create_clock()]]))

def create_dma_device():
    clock = create_clock()

    mock_mem = dev_util.Memory()
    mock_tgt = dev_util.Dev([MockSignal])

    name = device_name()
    device = simics.pre_conf_object(name, 'sample_dma_device')

    device.target_mem = mock_mem.obj
    device.intr_target = mock_tgt.obj
    device.queue = clock

    simics.SIM_add_configuration([device,], None)
    return (name, simics.SIM_get_object(name))

def create_patch_tool(banks, offset = 0, size = 0, value = 0,
                      inject = False):
    return run_command(
        'new-bank-patch-tool banks=%s offset=%s size=%s value=%s %s' % (
            banks, offset, size, value,'-inject' if inject else ''))

def sample_registers(device):
    hit = dev_util.Register_LE(device.bank.regs, 0, 4)
    miss = dev_util.Register_LE(device.bank.regs, 4, 4)
    return (hit, miss)

def dma_registers(device):
    hit = dev_util.Register_LE(device.bank.regs, 0, 4)
    miss = dev_util.Register_LE(device.bank.regs, 12, 4)
    return (hit, miss)

def bank_name(device_name):
    return '%s.bank.regs' % device_name

def expect_miss(device, fun, args = []):
    with stest.expect_log_mgr(device, 'spec-viol'):
        stest.expect_exception(fun, args, dev_util.MemoryError)
