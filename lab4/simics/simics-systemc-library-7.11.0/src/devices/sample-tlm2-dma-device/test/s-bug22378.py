# © 2015 Intel Corporation
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
import config

phys_mem = SIM_create_object('memory-space', 'phys_mem')
(dma, _) = config.create_dma_device()
phys_mem.map.append([0x1000, dma, 0, 0, 0x100, None, 0, 8, 0])

def read_with_size(size):
    '''Test that a read fails iff it is not of register size'''
    try:
        run_command('phys_mem.read 0x1000 %d -l' % size)
        if size != 4:
            stest.fail('Only 4-byte access should be supported')
    except CliError:
        stest.expect_different(4, size)

def inquiry_read_with_size(size):
    run_command('phys_mem.get 0x1000 %d -l' % size)

# Test that normal read works as expected (only size 4 allowed)
for size in range(1, 9):
    read_with_size(size)

# Test that inquiry read works as expected (any size OK)
for size in range(1, 9):
    inquiry_read_with_size(size)

try:
    run_command('phys_mem.get 0x100c 4 -l')
    stest.fail("Access outside of registers should not work")
except CliError:
    pass
