# © 2022 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


from blueprints import State, Default, ConfObject, Config
from simmod.std_bp.state import UARTConnectionState

class RISCVConfig(Config):
    num_cores = 4
    freq_mhz = 100.0
    core_class = "riscv-rv64"
    timebase_freq_mhz = 1.0
    memory_megs = 2048
    s_core = False
    has_clic = False
    ram_size = 0x1000
    create_con = True

class RISCVBackplane(State):
    com = UARTConnectionState()
    queue = ConfObject()
    processors: list[str] = []

class CPUFabric(State):
    mem_space = ConfObject()
    cell = ConfObject()
    clic: list[ConfObject] = []
    hart: list[ConfObject] = []
    bp = RISCVBackplane()
