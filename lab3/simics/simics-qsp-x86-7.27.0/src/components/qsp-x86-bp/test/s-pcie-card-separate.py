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

from blueprints import instantiate, Builder, Namespace, BlueprintFun
from simmod.qsp_x86_bp.qsp_x86 import disks, system, SystemParams
from simmod.std_bp.devices import sii3e132
from simmod.std_bp.state import PCIESlotConnectionState, Queue
from simmod.qsp_x86_bp.qsp_x86.state import ScriptConsole
import conf
import cli

# SIMICS-21634
conf.sim.deprecation_level = 0

QSP_IMAGES = "%simics%/targets/qsp-x86/images"

def config(bp: Builder, name: Namespace):
    params = bp.create_config(name, SystemParams)
    params.disk_bp = "clear_linux"
    params.memory_size_mb = 8192
    bp.expand(name, "board", system)
    # Make sure the queue is set for all objects
    queue = bp.read_state(name.board, Queue)
    bp.obj(name, "namespace", queue=queue.queue)

    # Transfer PCIe connection information from the board hierarchy
    bp.alias_state(name.board.pcie1, PCIESlotConnectionState, name.sii)
    bp.expand(name, "sii", sii3e132)

    # The busybox blueprint requires ScriptConsole
    con = bp.read_state(name.board, ScriptConsole)
    bp.expose_state(name.sii, con)

    bp.expand(name, "sii.sata0.disk", disks.busybox, path=QSP_IMAGES)

instantiate("top", config)

# Verify boot
conf.bp.console_string.cli_cmds.run_until(object=conf.top.board.com0.con,
                                          string="root@cl-qsp")
cli.global_cmds.run_script(script="verify-pcie-disk.simics")
