# © 2015 Intel Corporation

import stest
import info_status
import simics
from config import create_test_device

dut = create_test_device()

# Verify that info/status commands have been registered for all
# classes in this module.
info_status.check_for_info_status(['empty-device-sc-only'])
simics.SIM_run_command("output-radix 16")

for cmd in ['info', 'status']:
    try:
        simics.SIM_run_command(dut.name + '.' + cmd)
    except simics.SimExc_General as e:
        stest.fail(cmd + ' command failed: ' + str(e))
