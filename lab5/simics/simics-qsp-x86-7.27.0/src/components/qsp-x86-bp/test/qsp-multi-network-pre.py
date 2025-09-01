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
from sim_params import params

mac = "00:19:A0:E1:1C:"

# Set up all QSP machines, but without network
for i in range(params['num_machines']):
    ns = f"qsp[{i}]"
    params.setdefault(f"{ns}:hw:board_name", f"{params['board_name']}{i}")
    params.setdefault(f"{ns}:hw:qsp:mac_address", f"{mac}{0x9f+i}")
    params.setdefault(f"{ns}:hw:net:switch:links",
                      [f"{params['board_name']}{i}.eth0"])
    params.setdefault(f"{ns}:hw:net_name", f"net{i+1}")
# Network blueprint parameters imported under "net" namespace
params.setdefault("net:switch:links", [f"{params['board_name']}{i}.eth0"
                                for i in range(params['num_machines'])])
