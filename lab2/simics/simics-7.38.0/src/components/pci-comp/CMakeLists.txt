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

simics_add_module(pci-comp
  COMPONENTS pci_bcm5703c_comp pci_bcm5704c_comp pci_am79c973_comp pci_vga_comp pci_accel_vga_comp pci_i21152_comp pci_i82543gc_comp pci_i82546bg_comp pci_i82559_comp pci_pd6729_comp pci_dec21041_comp pci_dec21143_comp pci_dec21140 pci_dec21140_panel pci_dec21140a_comp pci_accel_vga_v2_comp
  SOURCES module_load.py pci_comp.py
  SIMICS_API latest
)
