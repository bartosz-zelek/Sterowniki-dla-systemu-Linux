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

from cli import run_command

def device_name():
    device_name.num += 1
    return 'device%s' % device_name.num
device_name.num = -1

def create_test_device(_class, name = None):
    if not name:
        name = device_name()

    device = simics.SIM_create_object(_class, name)
    banks = {
        'bank0' : device.bank.bank0,
        'bank1[0]' : device.bank.bank1[0],
        'bank1[1]' : device.bank.bank1[1],
        'bank1[2]' : device.bank.bank1[2],
        'empty' : device.bank.empty,
        'bank2' : device.bank.bank2
    }

    return (name, banks, device)

def create_tool(bank):
    return run_command(
        'new-bank-coverage-tool banks=%s' % bank.name)

def add_instrumentation(tool_name, bank):
    run_command('%s.add-instrumentation %s' % (tool_name,
                                               bank.name))
