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


from blueprints import instantiate, Builder, Namespace, BlueprintFun
from simmod.qsp_x86_bp.qsp_x86 import system, SystemParams
from simmod.qsp_x86_bp.qsp_x86.disks import clear_linux
from simmod.std_bp.eth import switch

import conf
# SIMICS-21634
conf.sim.deprecation_level = 0

def config(bp: Builder, name: Namespace):
    params = bp.create_config(name, SystemParams)
    params.disk_bp = "clear_linux"
    params.memory_size_mb = 8192
    bp.expand(name, "board0", system)
    bp.expand(name, "board1", system)
    bp.expand(name, "switch", switch,
        port0 = name.board0.eth0,
        port1 = name.board1.eth0,
    )

instantiate("", config)
