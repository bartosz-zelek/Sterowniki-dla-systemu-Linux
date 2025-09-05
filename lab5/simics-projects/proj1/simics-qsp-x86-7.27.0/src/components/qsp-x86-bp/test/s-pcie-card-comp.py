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

from blueprints import Builder, Namespace
from blueprints.adapter import bp_legacy_comp
from blueprints.params import ParamGroup
from simmod.qsp_x86_bp.qsp_x86 import disks, system
from simmod.qsp_x86_bp.qsp_x86.state import ScriptConsole
import cli
import conf
import simics

# SIMICS-21634
conf.sim.deprecation_level = 0

QSP_IMAGES = "%simics%/targets/qsp-x86/images"

@bp_legacy_comp([ParamGroup("board", "", import_bp="qsp_system")],
                name="qsp_config_comp")
def config(bp: Builder, name: Namespace):
    bp.expand(name, "board", system)

    # Expose ScriptConsole state at the top level
    con = bp.read_state(name.board, ScriptConsole)
    ic = bp.expose_state(name, ScriptConsole)
    ic.con = con.con

@bp_legacy_comp([], name="busybox_comp")
def busybox(bp: Builder, name: Namespace):
    # Expose ScriptConsole state at the top level
    con = bp.read_state(name.busybox, ScriptConsole)
    ic = bp.expose_state(name.busybox, ScriptConsole)
    ic.con = con.con

    bp.expand(name, "busybox", disks.busybox, path=QSP_IMAGES)

simics.SIM_load_module("sii3132-comp")

# Blueprint parameters exposed as component parameters
cli.global_cmds.create_qsp_config_comp(name="qsp",
                                       **{'board_memory_size_mb': 8192,
                                          'board_disk_bp': "clear_linux",
                                          'board_disk_image_path': QSP_IMAGES})
cli.global_cmds.create_sii3132_comp(name="pcie")

# Connect PCIe card
cli.global_cmds.connect(cnt0="qsp.board_pcie1_PCIESlotConnectionState",
                        cnt1="pcie.pci_bus")

# Connect disk
cli.global_cmds.create_busybox_comp(name="disk")
cli.global_cmds.connect(cnt0="disk.busybox_SATAConnectionState",
                        cnt1="pcie.sata_slot0")

cli.global_cmds.instantiate_components()

# Verify boot
conf.bp.console_string.cli_cmds.run_until(object=conf.qsp.board.com0.con,
                                          string="root@cl-qsp")
cli.global_cmds.run_script(script="verify-pcie-disk.simics")
