# © 2021 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


from blueprints import (instantiate, Builder, Namespace)
from blueprints.params import preset_from_args
from simmod.qsp_x86_bp.qsp_x86 import system, SystemParams
from simmod.qsp_x86_bp.x86_cpu.state import CPUConfig
import cli
import conf
import stest

def multi_core_qsp(bp: Builder, name: Namespace):
    params = bp.create_config(name, SystemParams)
    params.memory_size_mb=8192
    params.disk_bp="clear_linux"
    cparams = bp.create_config(name.board_4c, CPUConfig)
    cparams.num_cores = 4
    bp.expand(name, "board_4c", system)

# SIMICS-21634
conf.sim.deprecation_level = 0

instantiate("board", system, params=dict(memory_size_mb=8192,
                                         disk_bp="clear_linux"))

# Verify hotplug
cli.global_cmds.disconnect(cnt0=conf.board.com0.con.connector,
                           cnt1=conf.board.com0)
cli.global_cmds.connect(cnt0=conf.board.com0.con.connector,
                        cnt1=conf.board.com0)

cli.global_cmds.disconnect(cnt0=conf.board.sata0.disk.connector,
                           cnt1=conf.board.sata0)
cli.global_cmds.connect(cnt0=conf.board.sata0.disk.connector,
                        cnt1=conf.board.sata0)

instantiate("", multi_core_qsp)
expected_procs = ['board_4c.mb.cpu0.core[0][0]',
                  'board_4c.mb.cpu0.core[1][0]',
                  'board_4c.mb.cpu0.core[2][0]',
                  'board_4c.mb.cpu0.core[3][0]']
existing_procs = [proc[0].name for proc in global_cmds.list_processors(substr = 'board_4c')]
stest.expect_equal(expected_procs, existing_procs)
