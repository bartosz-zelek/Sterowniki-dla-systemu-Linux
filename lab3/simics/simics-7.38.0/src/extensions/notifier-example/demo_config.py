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


# A very simple configuration, showing how the tcp-server works with a
# DML device.

import simics

clk0 = simics.SIM_create_object("clock", "clk0", freq_mhz=1)
rec0 = simics.SIM_create_object("recorder", "rec0", queue=clk0)
dev0 = simics.SIM_create_object("connected_device", "dev0")
serv0 = simics.SIM_create_object("tcp_server", "serv0",
                                 recorder=rec0, recipient=dev0)

# After running this script, try connecting to the machine (telnet) at the port
# displayed in the log message, and write a couple of lines.
