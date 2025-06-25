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


from blueprints import Namespace, Builder, blueprint, Config
from blueprints.params import Param, ParamGroup
from simmod.std_bp.eth import (service_node, switch,
                               SwitchConfig, ServiceNodeConfig)

class NetworkParams(Config):
    sn_name = "service_node0"
    eth_link_name = "ethernet_switch0"

# Network setup blueprint
@blueprint([Param("sn_name", "Service node object name", NetworkParams),
            Param("eth_link_name", "Ethernet switch object name",
                  NetworkParams),
            ParamGroup("sn", "", ns="sn_name", import_bp="service_node"),
            ParamGroup("switch", "", ns="eth_link_name",
                       import_bp="ethernet_switch")],
           "network_setup")
def setup(bp: Builder, name: Namespace, params: NetworkParams):
    # Create service node
    bp.expand(name, params.sn_name, service_node)

    # Create switch and connect given links and service node
    bp.expand(name, params.eth_link_name, switch, port0=getattr(name, params.sn_name))
