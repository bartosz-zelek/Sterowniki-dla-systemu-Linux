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


import simics
import stest
import dev_util as du
import cli
import config

counters = {
    'dmi_access_count' : 0,
    'dst_read_count' : 0,
    'dst_write_count' : 0,
    'src_read_count' : 0,
    'src_write_count' : 0,
    'read_count' : 0,
    'write_count' : 0
}

def reset_counters():
    global counters
    for k in list(counters.keys()):
        counters[k] = 0

def non_zero_counters():
    l = []
    for (k, v) in sorted(list(counters.items())):
        if v:
            l.append((k, v))
    return l

def on_log(cb_data, trigger_obj, typ, message):
    global counters

    if message.find('DMI write access') != -1:
        counters['dmi_access_count'] += 1
    if message.find('DMI read access') != -1:
        counters['dmi_access_count'] += 1
    if message.find('Read destination register') != -1:
        counters['dst_read_count'] += 1
    if message.find('Write destination register') != -1:
        counters['dst_write_count'] += 1
    if message.find('Read source register') != -1:
        counters['src_read_count'] += 1
    if message.find('Write source register') != -1:
        counters['src_write_count'] += 1
    if message.find('ReadMem') != -1:
        counters['read_count'] += 1
    if message.find('WriteMem') != -1:
        counters['write_count'] += 1

def TEST(s):
    print('')  # adding this empty line to separate several tests
    print('Test:', s)

def set_source(addr, regs):
    regs.dma_source.write(addr)
    stest.expect_equal(regs.dma_source.read(), addr)

def set_target(addr, regs):
    regs.dma_destination.write(addr)
    stest.expect_equal(regs.dma_destination.read(), addr)

def setup_test(dma_ram, log_cb):
    # Must use a real memory-space object that implements the memory_page
    # interface in order to test DMI
    dma_image = dma_ram.image
    dma_space = simics.SIM_create_object('memory-space', 'dma_space')
    dma_space.map = [[0x0, dma_ram, 0, 0, dma_image.size]]
    mock = du.Dev([du.Signal])
    dma, regs = config.create_dma_device(dma_space = dma_space,
                                         signal = mock.obj)

    simics.SIM_hap_add_callback('Core_Log_Message', log_cb, None)
    dma.log_level = 3

    return (dma, dma_space, mock, regs)

def run_test(dma, dma_space, mock, regs):
    dst = 4711
    src = 1819
    set_target(dst, regs)
    set_source(src, regs)

    testdata = list(range(18*4))

    memory_space = dma_space.iface.memory_space
    memory_space.write(None, src, tuple(testdata), 0)

    TEST("Start transfer 1")
    memory_space.write(None, dst, tuple([0] * 72), 0)
    regs.dma_control.write(0, EN = 1, SWT=1, TS=18)

    simics.SIM_continue(1000 * 10**6)

    # Check transfer completed
    stest.expect_equal(regs.dma_control.TS, 0)
    stest.expect_equal(regs.dma_control.SWT, 0)
    stest.expect_equal(regs.dma_control.TC, 1)

    # No interrupt enabled
    stest.expect_equal(mock.signal.level, 0)

    TEST("Start transfer 2 with interrupt")
    regs.dma_control.write(0, EN = 1, SWT=1, TS=18, ECI = 1)
    simics.SIM_continue(1000 * 10**6)
    # interrupt enabled, should be raised
    stest.expect_equal(mock.signal.level, 1)

    stest.expect_equal(regs.dma_control.TS, 0)
    stest.expect_equal(regs.dma_control.SWT, 0)
    stest.expect_equal(regs.dma_control.TC, 1)

    TEST("Check that interrupt can be cleared")
    simics.SIM_continue(1000 * 10**6)
    regs.dma_control.write(0, EN = 1)
    # interrupt enabled, should be cleared
    cli.run_command("c 0")
    stest.expect_equal(mock.signal.level, 0)

    # We have to have this check after transaction 2 or it will tear down DMI
    stest.expect_equal(memory_space.read(None, dst, 18*4, 0), tuple(testdata))

    return dma, regs
