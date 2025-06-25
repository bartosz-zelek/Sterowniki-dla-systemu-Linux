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


import cli

#
# sample PCIe Device
#

device_class_name = "sample_tlm2_pcie_device"

#
# ------------------------ info -----------------------
#

def get_info(obj):
    return [("Common",
             [("Connect to PCIe upstream target", obj.upstream_target)]),
            ("Function 0",
             [("Vendor ID", hex(obj.vendor_id[0])),
              ("Device ID", hex(obj.device_id[0]))]),
            ("Function 5",
             [("Vendor ID", hex(obj.vendor_id[1])),
              ("Device ID", hex(obj.device_id[1]))])]

cli.new_info_command(device_class_name, get_info)

#
# ------------------------ status -----------------------
#

def get_status(obj):
    return [("Function 0",
             [("Command register", hex(obj.command[0])),
              ("Status register", hex(obj.status[0])),
              ("Version register", hex(obj.version[0]))]),
            ("Function 5",
             [("Command register", hex(obj.command[1])),
              ("Status register", hex(obj.status[1])),
              ("Version register", hex(obj.version[1]))])]

cli.new_status_command(device_class_name, get_status)
