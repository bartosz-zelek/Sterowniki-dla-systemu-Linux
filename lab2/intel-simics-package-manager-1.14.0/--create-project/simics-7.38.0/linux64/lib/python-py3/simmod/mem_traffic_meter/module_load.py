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

_class_name = 'mem-traffic-meter'

def info_status():
    def get_info(obj):
        return []
    def get_status(obj):
        return []
    cli.new_info_command(_class_name, get_info)
    cli.new_status_command(_class_name, get_status)
info_status()
