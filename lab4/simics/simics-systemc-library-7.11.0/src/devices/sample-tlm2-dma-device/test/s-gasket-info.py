# © 2020 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import config
import stest

config.create_dma_device()

# Trace log handler
logs = []
def log(data, src, len):
    global logs
    for line in src.splitlines():
        logs.append(line)

def reset_log():
    global logs
    logs = []

SIM_add_output_handler(log, None)
SIM_run_command('set-table-border-style borderless')
SIM_run_command('dma.gasket-info')

stest.expect_true(logs == [
    'simics2tlm',
    'OBJECT   INTERFACE          GASKET              SOCKET    ',
    'dma     transaction  gasket_DMADevice_mmio  DMADevice.mmio',
    'tlm2simics',
    '      SOCKET                 GASKET             INTERFACE     OBJECT  ',
    'DMADevice.phys_mem  gasket_DMADevice_phys_mem  memory_space  dma_space',
    'systemc2simics',
    '             SIGNAL                          GASKET            INTERFACE     OBJECT   ',
    'gasket_DMADevice_interrupt.signal  gasket_DMADevice_interrupt  signal     fake_signal0',])
