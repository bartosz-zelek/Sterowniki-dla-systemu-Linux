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


# Feature: SystemC process can be profiled with information about
# an ordered list of how much real world time was spend on each
# SystemC process
# Test this command runs OK and the expect method process is shown
# in the profiling result

from config import create_pci_device, LogHandler

(device, _, _, _) = create_pci_device("pci_dev")
log = LogHandler()

run_command("pci_dev.enable-process-profiler")
device.pci_device.blink_onoff_event.iface.sc_event.notify()
SIM_continue(1)

run_command("pci_dev.process-profiler-results")
log.match("Process profiler enabled.")
log.contains("pci_dev.pci_device.blinkOnoff (method)")
