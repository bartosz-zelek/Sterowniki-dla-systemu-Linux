# © 2025 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

# Setup blueprint python files
set(python_files
  # Standard blueprints
  colors.py
  state.py
  console.py
  devices.py
  disks.py
  eth.py
  gpu.py
  usb.py

  # Classes
  classes/__init__.py
  classes/connector.py
  classes/std_classes.py
)

simics_add_module(std-bp
  CLASSES script-engine eth-connector eth-link-connector usb-device-connector usb3-device-connector usb-host-connector uart-device-connector uart-remote-connector sata-controller-connector sata-device-connector pci-controller-connector pci-device-connector
  SOURCES module_load.py ${python_files}
  SIMICS_API latest
)
