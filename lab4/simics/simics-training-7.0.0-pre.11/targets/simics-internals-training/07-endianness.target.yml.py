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

def build_system(name="lab"):
    ns = pre_conf_object(name,'namespace')
    ns.mem=pre_conf_object('memory-space')
    ns.hart=pre_conf_object('riscv-rv64', freq_mhz=10, physical_memory=ns.mem)
    ns.attr.queue = ns.hart
    ns.ram=pre_conf_object('ram', self_allocated_image_size = 0x1000, 
                           image = None, queue=ns.hart)
    # BE and LE banks from the explore-memory-map object
    ns.emmd=pre_conf_object('explore-mem-map-device', queue=ns.hart)

    # Set up memory map 
    ns.mem.attr.map = [[0x0000, ns.ram, 0, 0, 0x1000],
                       [0x1000, ns.emmd.bank.regs_o_p, 0, 0, 0x20],
                       [0x2000, ns.emmd.bank.regs_o_p_be, 0, 0, 0x20],]

    # Add objects, return handle to top-level 
    SIM_add_configuration([ns],None)
    return SIM_get_object(ns.__object_name__)

sys_name = params.get('name')

build_system(sys_name)
