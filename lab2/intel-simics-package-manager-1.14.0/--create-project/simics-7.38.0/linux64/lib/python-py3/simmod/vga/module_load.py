# © 2010 Intel Corporation
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
import vga_common

vga_name = "vga"

def get_info(obj):
    return vga_common.get_info(obj)

def get_status(obj):
    return vga_common.get_status(obj)

cli.new_info_command(vga_name, get_info)
cli.new_status_command(vga_name, get_status)
vga_common.new_vga_commands(vga_name)
