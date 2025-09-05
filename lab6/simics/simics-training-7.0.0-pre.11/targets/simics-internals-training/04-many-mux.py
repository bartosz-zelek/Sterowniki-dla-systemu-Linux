# © 2024 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

has_port = params.get('use_ports')

if has_port:
    serial_mux_cls = 'serial-out-mux-p'
else:
    serial_mux_cls = 'serial-out-mux'

##
## Create several objects at once using SIM_add_configuration
##
clock = pre_conf_object('clk', 'clock', freq_mhz=10)
gbl_mem = pre_conf_object('phys_mem', 'memory-space')

# The Python variables are objects of type pre_conf_object (Python class).
# The names of the variables have no relationship to the names of the objects
namespacename = "sub"
ns = pre_conf_object(namespacename, 'namespace')
py_dev = pre_conf_object(ns.name + '.pyart', 'python_simple_serial')
phys_mem = pre_conf_object(ns.name + '.phys_mem', 'memory-space')
phys_mem.map = [ [0x1000, py_dev, 0, 0, 0x8] ]
py_dev.queue = clock
rec = pre_conf_object(ns.name + '.rec','recorder')
mux0 = pre_conf_object(ns.name + '.mux0', serial_mux_cls)
# Another way to build an hierarchical name:
mux0.mux1 = pre_conf_object(serial_mux_cls) 
mux1 = mux0.mux1                              # store in a variable for ease of use
mux2 = pre_conf_object(mux0.name + '.mux2', serial_mux_cls)
con0 = pre_conf_object(mux1.name + '.con0', 'textcon')
con1 = pre_conf_object(mux1.name + '.con1', 'textcon')
con2 = pre_conf_object(mux2.name + '.con0', 'textcon')
con3 = pre_conf_object(mux2.name + '.con1', 'textcon')
con0.window_title = "Console s.0.1.0"
con1.window_title = "Console s.0.1.1"
con2.window_title = "Console s.0.2.0"
con3.window_title = "Console s.0.2.1"
con0.device = py_dev                      # Someone has to talk back to pyart
# Use a loop to configure common properties for all the consoles
for c in [con0,con1,con2,con3]:
    c.recorder = rec
    c.queue = clock
    c.visible = True
    c.screen_size = [40,10]
# Set up the mux forwarding down the tree
if has_port:
    py_dev.console = mux0.port.serial_in      # pyart talks to mux0
    mux0.original_target = mux1.port.serial_in
    mux0.mux_target      = mux2.port.serial_in
else:
    py_dev.console = mux0                     # pyart talks to mux0
    mux0.original_target = mux1
    mux0.mux_target      = mux2
mux1.original_target = con0
mux1.mux_target      = con1
mux2.original_target = con2
mux2.mux_target      = con3

SIM_add_configuration([clock, gbl_mem, ns, py_dev, phys_mem, rec, mux0, mux1, mux2, con0, con1, con2, con3], None)

# Fix the memory map of the top level to point at the newly created memory map
# At this point, all pre_conf_objects have been turned into real objects 
conf.phys_mem.map += [[0x200_0000, SIM_get_object(phys_mem.name), 0, 0, 0x100_0000]]

