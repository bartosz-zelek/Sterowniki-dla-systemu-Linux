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

simics_add_module(usb-hid-components
  COMPONENTS usb_mouse_comp usb_keyboard_comp usb_hs_keyboard_comp
  SOURCES module_load.py usb_hid_components.py
  SIMICS_API latest
)
