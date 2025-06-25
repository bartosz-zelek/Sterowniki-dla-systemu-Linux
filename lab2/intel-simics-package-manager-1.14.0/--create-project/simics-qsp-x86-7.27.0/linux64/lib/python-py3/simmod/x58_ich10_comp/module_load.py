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


from . import x58_ich10
x58_ich10.northbridge_x58.register()
x58_ich10.southbridge_ich10_panel.register()
x58_ich10.southbridge_ich10.register()
x58_ich10.motherboard_x58_ich10.register()
x58_ich10.chassis_x58_ich10.register()
x58_ich10.chassis_qsp_x86.register()
x58_ich10.x86_reset_signal_conv.register()
