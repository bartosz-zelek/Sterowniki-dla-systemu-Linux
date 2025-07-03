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
from blueprints import Builder, Namespace, instantiate, BlueprintError
from simmod.std_bp.state import Queue
import stest

buf = StringIO()
def tb(bp: Builder, name: Namespace):
    queue = bp.expose_state(name, Queue)
    queue.queue = bp.obj(name.clock, "clock", freq_mhz=1)
    print(queue, file=buf)
    bp.obj(name, "namespace", queue=queue.queue)

instantiate("top", tb)
stest.expect_equal(buf.getvalue(), """Queue[top] state
Current: {'queue': None}
Next: {'queue': top.clock}
Queue[top] state
Current: {'queue': top.clock}
Next: {'queue': top.clock}
""")
