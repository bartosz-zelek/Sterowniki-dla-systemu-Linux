# © 2016 Intel Corporation
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

simics.SIM_read_configuration(stest.scratch_file('accellera-lt-temporal-decouple-chkpt-state-t0'))
dut = SIM_get_object("dut")
simulation_end = 4800000

# Enable log output for capturing
dut.log_level = 2

logs = []

def log(data, src, len):
    for line in src.splitlines():
        logs.append(line)

simics.SIM_add_output_handler(log, None)

# Verify that simulation can be continued after restore
SIM_continue(1)

stest.expect_true(any('      ID: 201 COMMAND: WRITE Length: 04' in s for s in logs))

# Run to the end of the simulation
logs = []
SIM_continue(simulation_end - 1)

# Test that traffic generators have completed
stest.expect_true(any('=========================================================' in s for s in logs))
stest.expect_true(any('      Traffic Generator : 101' in s for s in logs))
stest.expect_true(any('      Traffic Generator : 102' in s for s in logs))
