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


from blueprints import Builder, instantiate, Namespace
from simmod.qsp_x86_bp.x86_cpu.system import simple_system
from simmod.qsp_x86_bp.x86_cpu.state import CPUConfig

def config(bp: Builder, name: Namespace):
    params = bp.create_config(name.system, CPUConfig)
    params.num_cores = 1
    params.num_threads = 1
    bp.expand(name, "system", simple_system)

instantiate("", config)
