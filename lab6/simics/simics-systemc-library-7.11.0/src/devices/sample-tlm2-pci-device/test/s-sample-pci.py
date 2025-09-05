# © 2013 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


from config import *
from common import *

sample_pci, _, regs, pci_bus = create_pci_device('sample_pci')

# Test the PCI vendor and device IDs
test_id_regs(regs, sample_pci)

# Test setting BAR to map the device in memory
test_mapping(regs, 0x100, 0x4711, pci_bus.io, pci_bus.mem, sample_pci)

# Test interface
test_interface(sample_pci, sample_pci)

# Test multi-function (MF) device
# NOTE: the PCI F5 has been designed to work just like F0
test_mf_device(regs, pci_bus.io, pci_bus.mem, sample_pci)
test_mf_device_mapped_by_pci_bus(pci_bus.bus, sample_pci)

# Test DMA/memory access
test_dma(pci_bus.mem, sample_pci)

print("All tests passed.")
