# © 2024 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

from blueprints import Config, State, Builder, ConfObject, Namespace
from simmod.std_bp.state import Queue

#:: pre clock {{
class ClockParams(Config):
    freq_mhz = 42

class SystemFabric(State):
    clk: ConfObject = None

def my_clock(builder: Builder, name: Namespace,
             params: ClockParams, fabric: SystemFabric):
    fabric.clk = builder.obj(name, "clock",
                             freq_mhz=params.freq_mhz)

def board(builder: Builder, name: Namespace, clock_freq=10):
    # Expose main state
    fabric = builder.expose_state(name, SystemFabric)
    # Expose Queue state, useful for external blueprints
    queue = builder.expose_state(name, Queue)
    # Set queue on whole hierarchy
    builder.obj(name, "blueprint-namespace", queue=queue.queue)
    # Create config for sub-blueprints
    clk_conf = builder.create_config(name.clk, ClockParams)
    clk_conf.freq_mhz = clock_freq
    queue.queue = fabric.clk
    # Expand sub-blueprints
    builder.expand(name, "clk", my_clock)
# }}
