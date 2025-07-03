# © 2022 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import cli

for i in range(params['num_machines']):
    ns = f"qsp[{i}]"
    cli.global_cmds.run_script(script="qsp-clear-linux.yml",
                               namespace=ns)

# Hotplug machines to switch
for i in range(params['num_machines']):
    # Each QSP connects to its own switch, disconnect first
    cli.global_cmds.disconnect(
        cnt0=f"{params['board_name']}{i}.eth0",
        cnt1=f"net{i+1}.ethernet_switch0.port1")
    # Then connect to central switch
    cli.global_cmds.connect(cnt0=f"{params['board_name']}{i}.eth0",
                            cnt1=f"{params['net:eth_link_name']}.port{i+1}")
