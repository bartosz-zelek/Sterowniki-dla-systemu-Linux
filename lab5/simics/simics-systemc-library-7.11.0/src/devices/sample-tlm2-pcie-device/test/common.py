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

import simics
import stest
import os
import shutil
import cli

ALL_FUNCTION_NUMBERS = (0, 5,)

class fake_upstream_target:
    cls = simics.confclass("fake_upstream_target")

    @cls.iface.transaction.issue
    def issue(self, t, addr):
        self.issue.append((addr, t.pcie_type, t.size, t.read))
        return simics.Sim_PE_No_Exception

    @cls.iface.pcie_map.add_function
    def add_function(self, map_obj, function_id):
        self.add_function.append(function_id)

    @cls.iface.pcie_map.del_function
    def del_function(self, map_obj, function_id):
        self.del_function.append(function_id)

    @cls.iface.pcie_map.add_map
    def add_map(self, map_obj, info, type):
        self.add_map.append((info, type))

    @cls.iface.pcie_map.del_map
    def del_map(self, map_obj, base, type):
        self.del_map.append((base, type))

    @cls.iface.pcie_map.get_device_id
    def get_device_id(self, dev_obj):
        return 0

    issue = []
    add_function = []
    del_function = []
    add_map = []
    del_map = []

def create_upstream_target(fake=True, name="up"):
    if fake:
        up = simics.pre_conf_object(name, "fake_upstream_target")
    else:
        up = simics.pre_conf_object(name, "pcie-downstream-port")
    simics.SIM_add_configuration([up], None)
    return simics.SIM_get_object(up.name)

def create_sample_pcie_device():
    dev = simics.pre_conf_object("dev", "sample_tlm2_pcie_device")
    simics.SIM_add_configuration([dev], None)
    return simics.SIM_get_object(dev.name)

def save_checkpoint(file_name):
    print("Saving checkpoint")
    ckpt = stest.scratch_file('sample-tlm2-pcie-device-ckpt-' + file_name)
    if os.path.exists(ckpt):
        shutil.rmtree(ckpt)
    simics.SIM_write_configuration_to_file(ckpt, 0)

def load_checkpoint(file_name):
    '''Load a checkpoint'''
    print("Loading checkpoint")
    try:
        cli.run_command("log-level 3")
        simics.SIM_read_configuration(
            stest.scratch_file('sample-tlm2-pcie-device-ckpt-' + file_name))
    except Exception as ex:
        print(ex)
        simics.SIM_quit(1)
