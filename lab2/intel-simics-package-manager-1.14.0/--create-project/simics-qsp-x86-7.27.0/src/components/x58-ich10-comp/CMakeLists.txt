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

# NOTE: does not build outside of Base repo, need base_copy_images
simics_add_module(x58-ich10-comp
  CLASSES x86_reset_signal_conv auto_apic_bus
  COMPONENTS northbridge_x58 southbridge_ich10_panel southbridge_ich10 motherboard_x58_ich10 chassis_x58_ich10 chassis_qsp_x86
  SOURCES x58_ich10.py simics_start.py module_load.py
  SIMICS_API latest
)
if(COMMAND base_copy_images)
  base_copy_images(x58-ich10-comp IMAGES x58-ich10.png)
endif()
