# © 2015 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


from . import x86_nehalem
x86_nehalem.processor_core_i7.register()
x86_nehalem.processor_core_i7_single.register()
x86_nehalem.processor_core_i7_duo.register()
x86_nehalem.processor_core_i7_6c_2t.register()
x86_nehalem.processor_xeon_5500.register()
x86_nehalem.processor_xeon_5530.register()
x86_nehalem.processor_core_i7_8c_4t.register()
x86_nehalem.processor_x86_intel64.register()
x86_nehalem.processor_x86QSP1.register()
