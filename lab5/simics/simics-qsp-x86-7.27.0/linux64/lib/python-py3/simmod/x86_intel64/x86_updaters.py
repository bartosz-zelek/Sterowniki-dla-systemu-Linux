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


import update_checkpoint

def vmexit_instruction_info(obj):
    if hasattr(obj, "vmcs_layout"):
        vmcs_layout = getattr(obj, "vmcs_layout")
        for field in vmcs_layout:
            if field[0] == 0x440e:
                field[1] = 'VM-exit instruction information'
        setattr(obj, "vmcs_layout", vmcs_layout)

def x86_register_configuration_update(classname):
    update_checkpoint.SIM_register_class_update(6002, classname,
                                                vmexit_instruction_info)
