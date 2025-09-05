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
from simmod.qsp_x86_bp.qsp_x86 import system
from simmod.qsp_x86_bp.qsp_x86.state import ScriptConsole
import cli
import conf
import simics

# SIMICS-21634
conf.sim.deprecation_level = 0

@bp_legacy_comp([ParamGroup("board", "", import_bp="qsp_system")],
                name="qsp_config_comp")
def config(bp: Builder, name: Namespace):
    bp.expand(name, "board", system)

    # Expose ScriptConsole state at the top level
    con = bp.read_state(name.board, ScriptConsole)
    ic = bp.expose_state(name, ScriptConsole)
    ic.con = con.con

# Blueprint parameters exposed as component parameters
cli.global_cmds.new_qsp_config_comp(name="qsp",
                                    **{'board_memory_size_mb': 8192,
                                       'board_disk_bp': "clear_linux"})

# Verify info and status commands
conf.qsp.cli_cmds.info()
conf.qsp.cli_cmds.status()
