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


from blueprints import Builder, Namespace, Config, blueprint
from blueprints.params import Param, ParamGroup
from simmod.qsp_x86_bp.qsp_x86.board import system
from simmod.std_bp.eth import SwitchConfig
from .network_setup import NetworkParams, setup

class QSPSystemParams(Config):
    board_name = "board"
    net_name = "net"

@blueprint([Param("board_name", "QSP board root namespace", QSPSystemParams),
            Param("net_name", "Network root namespace", QSPSystemParams),
            ParamGroup("qsp", "", ns="board_name", import_bp="qsp_system"),
            ParamGroup("net", "", ns="net_name", import_bp="network_setup")],
           "qsp_platform")
def qsp_platform(bp: Builder, name: Namespace, conf: QSPSystemParams):
    bp.expand(name, conf.board_name, system)
    bp.expand(name, conf.net_name, setup)
