# © 2017 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import update_checkpoint

def remove_ep_slave_table_attr(obj):
    if hasattr(obj, 'slave_table'):
        delattr(obj, 'slave_table')

update_checkpoint.install_class_configuration_update(
    5094, "i2c-link-endpoint", remove_ep_slave_table_attr)
