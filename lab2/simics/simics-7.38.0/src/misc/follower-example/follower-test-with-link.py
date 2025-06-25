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


# A sample follower agent implementation in Python.
# It acts as a serial device, passing on data in either direction.

class Agent:
    def __init__(self, obj):
        self.obj = obj
        self.buffer = b""
    def enqueue(self, s):
        self.buffer += s
    def send_buffered(self):
        while self.buffer:
            if not self.device.iface.serial_device.write(self.buffer[0]):
                return
            self.buffer = self.buffer[1:]

def init_agent(obj):
    return Agent(obj)

agent = SIM_create_class("agent", class_info_t(init=init_agent))

def agent_get_leader(obj):
    return obj.object_data.leader
def agent_set_leader(obj, val):
    obj.object_data.leader = val
    return Sim_Set_Ok
SIM_register_attribute(agent, "leader",
                       agent_get_leader,
                       agent_set_leader,
                       Sim_Attr_Required, "o",
                       "Leader that send messages to a follower")
def agent_get_device(obj):
    return obj.object_data.device
def agent_set_device(obj, val):
    obj.object_data.device = val
    return Sim_Set_Ok
SIM_register_attribute(agent, "device",
                       agent_get_device,
                       agent_set_device,
                       Sim_Attr_Required, "o",
                       "(serial) device that the agent talks to")

# The agent must implement follower_message. We just pass on any data to the
# serial device here.

# When this method is called, the clock is at the same time as the
# follower when it was originally sent.
def agent_accept(obj, msg):
    agent = obj.object_data
    # add a new line, to make the output prettier
    agent.enqueue(msg + b"\r\n")
    agent.send_buffered()

def agent_accept_async(obj, msg):
    print("agent: got async message (%r)" % (msg,))

SIM_register_interface(agent, "follower_agent",
                       follower_agent_interface_t(
                           accept=agent_accept,
                           accept_async=agent_accept_async))

# The agent must be able to talk to other parts of the Simics configuration.
# This usually means that it, or an accomplice, must masquerade as the
# devices that are in the follower. Here we pretend we can send and receive
# serial communications:

def agent_write(obj, val):
    msg = b"Serial input: %c" % (val,)
    agent = obj.object_data
    t = agent.device.iface.link_endpoint_v2.delivery_time()
    skey = agent.device.iface.link_endpoint_v2.delivery_skey()
    agent.leader.iface.leader_message.send(t, skey, msg)
    return 1

def agent_receive_ready(obj):
    obj.object_data.send_buffered()

SIM_register_interface(
    agent, "serial_device",
    serial_device_interface_t(write=agent_write,
                              receive_ready=agent_receive_ready))

# A minimal configuration using the above agent class.
# It can be combined with other configurations in the same Simics.

from configuration import *

SIM_set_configuration(
    [OBJECT("cell0", "cell",
            leader=OBJ("leader0")),
     OBJECT("clock0", "clock",
            cell=OBJ("cell0"),
            freq_mhz=1),
     OBJECT("leader0", "leader",
            clock=OBJ("clock0"),
            agent=OBJ("agent0")),
     OBJECT("agent0", "agent",
            leader=OBJ("leader0"),
            queue=OBJ("clock0"),
            device=OBJ("slink0_agent0_ep")),

     # We connect the agent to a serial link. Normally links are created using
     # components, but here we do it all by hand so we can see exactly how
     # things fit together.
     OBJECT("slink0", "ser-link-impl",
            goal_latency=0.1,
            # Up the link buffer size, to avoid congestion with high latencies.
            buffer_size=1000),
     OBJECT("slink0_kon0_ep", "ser-link-endpoint",
            id=1,
            link=OBJ("slink0"),
            device=OBJ("kon0")),
     OBJECT("slink0_agent0_ep", "ser-link-endpoint",
            id=2,
            link=OBJ("slink0"),
            device=OBJ("agent0"),
            # For the link endpoint to be useful to the agent, it must be set
            # to indirect delivery
            indirect_delivery=True),

     # At the other end of the link, we add a text-console in another cell.
     OBJECT("cell1", "cell"),
     OBJECT("clock1", "clock",
            cell=OBJ("cell1"),
            freq_mhz=1),
     OBJECT("kon0", "textcon",
            window_title="ohne titel",
            device=OBJ("slink0_kon0_ep"),
            recorder=OBJ("rek0"),
            queue=OBJ("clock1")),
     OBJECT("rek0", "recorder"),
     ])

print("leader listening on port", conf.leader0.port)
