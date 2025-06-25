# © 2017 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


from common import *
from comp import *
from config import *

class pci_sample_gasket_comp(StandardConnectorComponent):
    """Pci Sample Gasket component class."""
    _class_desc = "Pci Sample Gasket component"
    _help_categories = ()

    def setup(self):
        super().setup()
        if not self.instantiated.val:
            self.add_objects()

    def add_objects(self):
        simulation = self.add_pre_obj('simulation', 'sample_tlm2_pci_device_external')

        conf_space = self.add_pre_obj('pci_conf', 'memory-space')
        io_space = self.add_pre_obj('pci_io', 'memory-space')
        mem_space = self.add_pre_obj('pci_mem', 'memory-space')

        pci_bus = self.add_pre_obj('pci_bus', 'pci-bus')
        pci_bus.conf_space = conf_space
        pci_bus.io_space = io_space
        pci_bus.memory_space = mem_space

        gasket = self.add_pre_obj('gasket', 'sample_tlm2_pci_device_gasket_composite_Pci')
        gasket.device = 'pci_device'
        gasket.simulation = simulation
        gasket.pci_bus = pci_bus

        simulation.gasket_list = [gasket]

SIM_run_command("create-pci-sample-gasket-comp")
SIM_run_command("instantiate-components")

regs = create_regs(conf.component0.gasket)

test_id_regs(regs, conf.component0.simulation)
test_mapping(regs, 0x100, 0x4711, conf.component0.pci_bus.io_space, conf.component0.pci_bus.memory_space, conf.component0.gasket)
test_interface(conf.component0.simulation, conf.component0.gasket)
test_mf_device(regs, conf.component0.pci_bus.io_space, conf.component0.pci_bus.memory_space, conf.component0.gasket)
test_dma(conf.component0.pci_bus.memory_space, conf.component0.simulation)
