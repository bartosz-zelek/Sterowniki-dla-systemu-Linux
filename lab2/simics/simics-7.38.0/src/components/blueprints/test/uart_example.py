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

from blueprints import Builder, Namespace, State, ConfObject

#:: pre uart {{
class UARTState(State):
    uart: ConfObject = None
    console: ConfObject = None

def uart(builder: Builder, name: Namespace):
    con = builder.read_state(name, UARTState)
    con.uart = builder.obj(name, "NS16550", console=con.console)

def console(builder: Builder, name: Namespace):
    con = builder.read_state(name, UARTState)
    con.console = builder.obj(name, "textcon", device=con.uart,
                              recorder=name.recorder)
    builder.obj(name.recorder, "recorder")

def board(builder: Builder, name: Namespace):
    builder.expose_state(name, UARTState)
    builder.obj(name, "blueprint-namespace", info="UART example")
    builder.expand(name, "uart", uart)
    builder.expand(name, "console", console)
# }}
