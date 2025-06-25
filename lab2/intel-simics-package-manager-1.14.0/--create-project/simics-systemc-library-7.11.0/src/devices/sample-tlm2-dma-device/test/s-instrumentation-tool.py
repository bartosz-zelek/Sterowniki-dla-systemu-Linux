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


# Test the instrumentation tool can trace and log different actions

from transfer import run_test
from simics import SIM_add_output_handler
import stest
import dev_util as du
import config

mock = du.Dev([du.Signal])
dma, regs = config.create_dma_device(signal = mock.obj)
dma.log_level = 1

# Trace log handler
logs = []
def log(data, src, len):
    global logs
    for line in src.splitlines():
        logs.append(line)

def match_log(line):
    for log in logs:
        if line in log:
            return True
    return False

def reset_log():
    global logs
    logs = []

SIM_add_output_handler(log, None)

def test():
    run_test(dma, SIM_get_object('dma_space'), mock, regs)

def match_trace_log(kind):
    '''Used to verify different kinds of trace logs existed in the
    trace output.'''
    if kind == 'socket':
        stest.expect_true(match_log('trace-b-'))
    else:
        stest.expect_true(match_log('trace-%s' % kind))

module_name = 'sample-tlm2-dma-device'
tool_name = 'sc_test_tool'

# Test with different tool settings
for provider in (
        "dma.DMADevice.onReset", # method process
        "dma.DMADevice.phys_mem", # initiator socket
        "dma.DMADevice.mmio", # target socket
        "dma.DMADevice.event_3", # event
        "dma.DMADevice.interrupt", # sc_out
):
    print("=============Checking tool provider: %s=============" % provider)
    reset_log()
    SIM_run_command("dma.new-sc-trace-tool providers = %s name = %s" % (provider, tool_name))
    test()
    stest.expect_true(match_log(provider))
    SIM_run_command("%s.delete" % tool_name)

tool_filters = ("event", "process", "signal", "socket")
filter_name = 'sc_test_filter'
for filt in tool_filters:
    print("=============Checking tool filter: %s=============" % filt)
    reset_log()
    SIM_run_command("dma.new-sc-trace-tool -connect-all name = %s" % tool_name)
    SIM_run_command("new-systemc-filter kind = %s name = %s" % (filt, filter_name))
    SIM_run_command("%s.add-filter %s" % (tool_name, filter_name))
    test()
    match_trace_log(filt)
    for other_type in tool_filters:
        if other_type != filt:
            stest.expect_false(match_log('trace-%s' % other_type))

    print()
    print('Test: when filter is removed, all connections are enabled')
    SIM_run_command("%s.remove-filter %s" % (tool_name, filter_name))
    test()
    for tf in tool_filters:
        match_trace_log(tf)
    SIM_run_command("%s.delete" % tool_name)
    SIM_run_command("%s.delete" % filter_name)

for fn in ("b_transport_post", "b_transport_pre"):
    print("=============Checking tool function: %s=============" % fn)
    reset_log()
    SIM_run_command("dma.new-sc-trace-tool -connect-all functions = %s name = %s" % (fn, tool_name))
    test()
    if fn == "b_transport_post":
        stest.expect_true(match_log('dma.DMADevice.mmio trace-b-out'))
        stest.expect_false(match_log('dma.DMADevice.mmio trace-b-in'))
    else:
        stest.expect_true(match_log('dma.DMADevice.mmio trace-b-in'))
        stest.expect_false(match_log('dma.DMADevice.mmio trace-b-out'))
    SIM_run_command("%s.delete" % tool_name)
