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


from blueprints import Builder, instantiate, Namespace
from simmod.risc_v_simple_bp.system import simple_system
from simmod.risc_v_simple_bp.state import RISCVConfig

def config(bp: Builder, name: Namespace):
    params = bp.create_config(name.board, RISCVConfig)
    params.num_cores = 1
    bp.expand(name, "board", simple_system)

instantiate("", config)
