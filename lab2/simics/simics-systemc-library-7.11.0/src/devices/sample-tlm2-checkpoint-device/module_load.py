# © 2014 Intel Corporation
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
from . import utility

device_class_name = "sample_tlm2_checkpoint_device"

def get_info(obj):
    return []

cli.new_info_command(device_class_name, get_info)

cli.new_status_command(device_class_name, utility.get_status)
