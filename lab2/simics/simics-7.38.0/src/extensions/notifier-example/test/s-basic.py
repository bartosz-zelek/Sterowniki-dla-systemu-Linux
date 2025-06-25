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


# Basic test of the notifier example code.

import sys
import socket
import stest

# Create a simple configuration with the class under test.
sys.path.append("..")
import demo_config

msgs = []
def collect_haps(arg, obj, typ, msg):
    msgs.append(msg)

hid = SIM_hap_add_callback_obj("Core_Log_Message", demo_config.dev0, 0,
                               collect_haps, None)

def tcp_connect(host, port):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((host, port))
    return s

sock = tcp_connect('localhost', demo_config.serv0.port)
sock.send(b'Hello')

# Since the simulation isn't running (this is just a Python script),
# we must allow the notifiers to run. More precisely, we wait until
# a log message has been issued.
SIM_process_work(lambda _: msgs != [], None)

stest.expect_equal(msgs, ["received data: {Hello}"])

sock.close()
