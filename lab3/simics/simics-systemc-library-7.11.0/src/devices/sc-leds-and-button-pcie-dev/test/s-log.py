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


# Feature: SystemC logs are correctly organized in Simics
# Test the SystemC log can captured by Simics and can be filtered using
# Simics logging system

from conf import sim
from config import create_pci_device
from stest import expect_true, expect_false

class CaptureInfoLogs:
    def __init__(self):
        SIM_hap_add_callback("Core_Log_Message",
                             self.callback,
                             sim.log_types.index('info'))
        self.message_list = []

    def callback(self, arg, obj, log_type, msg):
        self.message_list.append((obj, msg))

(device, _, regs, _) = create_pci_device("pci_dev")
info_capture = CaptureInfoLogs()

# With default log level 1, the debug level SystemC logs are not shown
regs.bar0.read()
expect_false("Good access (Read BAR (MEM)) -- f: 0 o: 0x10 s: 0x4 v: 0x0" in str(info_capture.message_list))

# With Simics log level set to 4, the debug SystemC logs are shown
device.log_level = 4
regs.bar0.read()
expect_true("Good access (Read BAR (MEM)) -- f: 0 o: 0x10 s: 0x4 v: 0x0" in str(info_capture.message_list))
