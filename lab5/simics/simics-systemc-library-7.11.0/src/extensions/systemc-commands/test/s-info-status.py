# © 2019 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import cli
import stest

# append two times: first internally and then for project builds
sys.path.append("../../../devices/sample-tlm2-dma-device/test")
sys.path.append("../../sample-tlm2-dma-device/test")
import config

dma, _ = config.create_dma_device()

# Standard info (defined by DMA device)
_, output = cli.quiet_run_command('dma.info')
stest.expect_true('DMA Target : dma_space' in output)

# Standard status (defined by systemc-commands utility.py)
_, output = cli.quiet_run_command('dma.status')
stest.expect_true('Time : 0 ps' in output)
stest.expect_true('Delta count : 0' in output)
stest.expect_true('Time to pending activity : 0 ps' in output)

# info on gaskets (target, initiator, signal)
_, output = cli.quiet_run_command('dma.gasket_DMADevice_mmio.info')
stest.expect_true('Sockets : dma.gasket_DMADevice_mmio.initiator_socket' in output)
stest.expect_true('Signals : none' in output)
_, output = cli.quiet_run_command('dma.gasket_DMADevice_phys_mem.info')
stest.expect_true('Sockets : dma.gasket_DMADevice_phys_mem.target_socket' in output)
stest.expect_true('Signals : none' in output)

_, output = cli.quiet_run_command('dma.gasket_DMADevice_reset.info')
stest.expect_true('Sockets : none' in output)
stest.expect_true('Signals : dma.gasket_DMADevice_reset.output_pin' in output)
_, output = cli.quiet_run_command('dma.gasket_DMADevice_interrupt.info')
stest.expect_true('Sockets : none' in output)
stest.expect_true('Signals : dma.gasket_DMADevice_interrupt.signal' in output)

# info on sc_ports (and initiator sockets)
_, output = cli.quiet_run_command('dma.DMADevice.reset.info')
stest.expect_true('Kind : sc_in' in output)
stest.expect_true('Connected to : dma.gasket_DMADevice_reset.output_pin' in output)
_, output = cli.quiet_run_command('dma.DMADevice.interrupt.info')
stest.expect_true('Kind : sc_out' in output)
stest.expect_true('Connected to : dma.gasket_DMADevice_interrupt.signal' in output)
_, output = cli.quiet_run_command('dma.DMADevice.mmio_port_0.info')
stest.expect_true('Kind : sc_port' in output)
stest.expect_true('Target : dma.gasket_DMADevice_mmio.initiator_socket_export_0' in output)
_, output = cli.quiet_run_command('dma.gasket_DMADevice_mmio.initiator_socket.info')
stest.expect_true('Kind : tlm_initiator_socket' in output)
stest.expect_true('Target : dma.DMADevice.mmio' in output)
_, output = cli.quiet_run_command('dma.DMADevice.phys_mem.info')
stest.expect_true('Kind : tlm_initiator_socket' in output)
stest.expect_true('Target : dma.gasket_DMADevice_phys_mem.target_socket' in output)

# info on sc_exports (and target sockets)
_, output = cli.quiet_run_command('dma.DMADevice.mmio.info')
stest.expect_true('Kind : tlm_target_socket' in output)
stest.expect_true('Initiator : dma.gasket_DMADevice_mmio.initiator_socket' in output)
_, output = cli.quiet_run_command('dma.gasket_DMADevice_phys_mem.target_socket.info')
stest.expect_true('Kind : tlm_target_socket' in output)
stest.expect_true('Initiator : dma.DMADevice.phys_mem' in output)

# info in sc_signal_read
_, output = cli.quiet_run_command('dma.DMADevice.reset.info')
stest.expect_true('Kind : sc_in' in output)
stest.expect_true('Connected to : dma.gasket_DMADevice_reset.output_pin' in output)
_, output = cli.quiet_run_command('dma.gasket_DMADevice_interrupt.signal.info')
stest.expect_true('Kind : sc_signal' in output)
stest.expect_true('Connected to : dma.DMADevice.interrupt' in output)
_, output = cli.quiet_run_command('dma.gasket_DMADevice_reset.output_pin.info')
stest.expect_true('Kind : sc_signal' in output)
stest.expect_true('Connected to : dma.DMADevice.reset' in output)

# status on sc_initiator_gasket

# info/status on sc_process
_, output = cli.quiet_run_command('dma.DMADevice.onReset.info')
stest.expect_true('Kind : sc_method_process' in output)
stest.expect_true('Initialize : True' in output)
_, output = cli.quiet_run_command('dma.DMADevice.onReset.status')
stest.expect_true('State : Normal' in output)
stest.expect_true('Process ID : 3' in output)
stest.expect_true('Waiting at : <not applicable for methods>' in output)
stest.expect_true('Events:' in output)
stest.expect_true('Memory use:' in output)
stest.expect_true('Total (bytes) : 0' in output)
stest.expect_true('Number of allocations : 0' in output)
stest.expect_true('Execution:' in output)
stest.expect_true('Profiling status : disabled' in output)
stest.expect_true('Profiling data : no data available' in output)

_, output = cli.quiet_run_command('dma.DMADevice.completeDMA.info')
stest.expect_true('Kind : sc_method_process' in output)
stest.expect_true('Initialize : False' in output)
_, output = cli.quiet_run_command('dma.DMADevice.completeDMA.status')
stest.expect_true('State : Normal' in output)
stest.expect_true('Process ID : 0' in output)
stest.expect_true('Waiting at : <not applicable for methods>' in output)
stest.expect_true('Events:' in output)
stest.expect_true('Memory use:' in output)
stest.expect_true('Total (bytes) : 0' in output)
stest.expect_true('Number of allocations : 0' in output)
stest.expect_true('Execution:' in output)
stest.expect_true('Profiling status : disabled' in output)
stest.expect_true('Profiling data : no data available' in output)
