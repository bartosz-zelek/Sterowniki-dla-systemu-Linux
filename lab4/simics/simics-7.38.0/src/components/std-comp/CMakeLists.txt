# © 2023 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

simics_add_module(std-comp
  COMPONENTS dummy_comp etg_comp etg_panel_comp ide_cdrom_comp ide_disk_comp mmc_card_comp pcmcia_flash_disk_comp sata_disk_comp sata_cdrom_comp
  SOURCES module_load.py std_comp.py
  SIMICS_API latest
)
