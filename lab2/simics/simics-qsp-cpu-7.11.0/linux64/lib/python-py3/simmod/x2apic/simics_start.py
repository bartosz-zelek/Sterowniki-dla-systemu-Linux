# INTEL CONFIDENTIAL

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


from update_checkpoint import install_class_configuration_update, remove_attr

def delete_attributes(obj):
    remove_attr(obj, "count_initial")
    remove_attr(obj, "count_start")

install_class_configuration_update(6111, "x2apic", delete_attributes)
