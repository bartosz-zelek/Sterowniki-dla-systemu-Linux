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


from . import x86_commands

module_classes = ('x86-p4e', 'x86-p4-nocona', 'x86-core2', 'x86-nehalem', 'x86-intel64', 'x86-p4e-model4', 'x86-nehalem-xeon', 'x86QSP1')

i64_capabilities = {
    "pm": True,
    "ia32": True,
    "aa64": True,
    "xmm": True
    }

for c in module_classes:
    x86_commands.register_capabilities(c, i64_capabilities)
    x86_commands.setup_processor_ui(c)
