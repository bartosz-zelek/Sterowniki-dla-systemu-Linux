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


from . import classes
import cli

all_classes = (
    "x58-mb",
)
def nil_info_status(_):
    return []

for cls in all_classes:
    cli.new_info_command(cls, nil_info_status)
    cli.new_status_command(cls, nil_info_status)
