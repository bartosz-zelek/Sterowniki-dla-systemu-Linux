# © 2021 Intel Corporation
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
import sys

classes = {
    "tcp-server",
}

if sys.platform.startswith("win"):
    classes.add("named-pipe-server")
else:
    classes.add("unix-socket-server")

def get_info(obj):
    return []

def get_status(obj):
    return []

for class_name in classes:
    cli.new_info_command(class_name, get_info)
    cli.new_status_command(class_name, get_status)
