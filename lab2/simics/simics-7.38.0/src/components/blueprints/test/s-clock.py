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

from io import StringIO
import logging
import simics
from blueprints import Builder, Namespace, instantiate, BlueprintError
from simmod.std_bp.eth import switch
from simmod.std_bp.state import Queue
import stest

# Initialize logger
logger = logging.getLogger("")
assert not logger.hasHandlers()
log = StringIO()
logger.addHandler(logging.StreamHandler(log))

def tb(bp: Builder, name: Namespace):
    queue = bp.expose_state(name, Queue)
    bp.obj(name, "namespace", queue=queue.queue)
    tb = name.tb
    queue.queue = bp.obj(tb.clock, "clock", freq_mhz=1)
    bp.expand(tb, "ethernet_switch", switch)

try:
    instantiate("", tb, logger=logger)
    simics.SIM_quit(1)
except BlueprintError:
    pass

class MyFilter(logging.Filter):
    def filter(self, r):
        return 0 if r.filename.endswith("impl.py") and r.lineno == 538 else 1

logger.setLevel(logging.DEBUG)
logger.addFilter(MyFilter())
instantiate("top", tb, logger=logger)
assert simics.SIM_object_clock(conf.top.tb.ethernet_switch) is not None

print(log.getvalue())
stest.expect_equal(log.getvalue(),"""Adding unnamed object of class "namespace".
Expansion of blueprint "tb" at node "top"
Expansion roots:
[(top, 'tb', {})]
Expansion presets:
[]
Iteration start: 0
Queue[top].queue -> None
Queue[top].queue <- top.tb.clock
SwitchConnector[top.tb.ethernet_switch].switch <- top.tb.ethernet_switch
SwitchConfig[top.tb.ethernet_switch].links -> SwitchConfig[top.tb.ethernet_switch].links state
Current: {}
Next: {}
Iteration end: 0
Iteration start: 1
Queue[top].queue -> top.tb.clock
SwitchConnector[top.tb.ethernet_switch].switch -> top.tb.ethernet_switch
Queue[top].queue -> top.tb.clock
Queue[top].queue <- top.tb.clock
SwitchConnector[top.tb.ethernet_switch].switch <- top.tb.ethernet_switch
SwitchConfig[top.tb.ethernet_switch].links -> SwitchConfig[top.tb.ethernet_switch].links state
Current: {}
Next: {}
Iteration end: 1
""")
