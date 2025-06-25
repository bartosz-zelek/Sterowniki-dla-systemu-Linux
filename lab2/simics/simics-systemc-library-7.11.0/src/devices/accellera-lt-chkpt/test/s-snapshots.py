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


import simics
import stest
import cli

all_logs = {}
dut = simics.SIM_create_object('lt_chkpt_example', 'dut', log_level=2)
simulation_end = 4800000

# Create a snapshot so that we can start from the beginning again
cli.run_command('take-snapshot time-0')

def get_cycles():
    return dut.vtime.cycles.cycle[0]

def log(data, src, len):
    logs = []
    for line in src.splitlines():
        logs.append(line)

    cycles = get_cycles()
    if cycles not in all_logs:
        all_logs[cycles] = []

    all_logs[cycles].append(logs)


simics.SIM_add_output_handler(log, None)

# Run to the end of the simulation
simics.SIM_continue(simulation_end + 1)

# Copy the log containing the output of the simulation mapped to simics cycles
old_logs = all_logs

# Test that traffic generators have completed
stest.expect_true(any('=========================================================' in s for s in old_logs[simulation_end]))
stest.expect_true(any('      Traffic Generator : 101' in s for s in old_logs[simulation_end]))
stest.expect_true(any('      Traffic Generator : 102' in s for s in old_logs[simulation_end]))

# Restore beginning of simulation
cli.run_command('restore-snapshot time-0')

snapshots = []
all_logs = {}

# Create a snapshot every time output has been written
for t in sorted(old_logs.keys()):
    cycles = t - get_cycles() + 1
    simics.SIM_continue(cycles)
    snapshots.append(get_cycles())
    cli.run_command('take-snapshot time-' + str(get_cycles()))

snapshots_reversed = list(reversed(snapshots))

# Start from the end by restoring a snapshot, run the simulation
# to the previous snapshot and compare that output matches the log
# produced when run through the simulation initially.
for (i, b) in enumerate(snapshots_reversed):
    if len(snapshots_reversed) <= i + 1:
        break

    cli.run_command('restore-snapshot time-' + str(snapshots_reversed[i + 1]))
    all_logs = {}
    simics.SIM_continue(b - snapshots_reversed[i + 1])
    key = get_cycles() - 1
    stest.expect_true(key in old_logs)
    stest.expect_true(key in all_logs)
    stest.expect_equal(all_logs[key], old_logs[key])

# Start from the beginng by restoring a snapshot, run the simulation
# to the next snapshot and compare that output matches the log
# produced when run through the simulation at the first time.
for (i, b) in enumerate(snapshots):
    cli.run_command('restore-snapshot time-' + str(b))
    if len(snapshots) > i + 1:
        all_logs = {}
        simics.SIM_continue(snapshots[i + 1] - b)
        key = get_cycles() - 1
        stest.expect_true(key in old_logs)
        stest.expect_true(key in all_logs)
        stest.expect_equal(all_logs[key], old_logs[key])
