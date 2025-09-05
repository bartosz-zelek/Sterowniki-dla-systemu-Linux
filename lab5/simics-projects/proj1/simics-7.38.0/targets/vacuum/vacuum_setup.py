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


# The configuration looks as follows:
#   + A single memory space, 'phys_mem'
#   + In this memory space, a RAM 'ram' of 256 MB,
#     is mapped at 0x10000000 to 0x2000000
#   + User devices can be mapped at any other address
#
# Run this Python file to gain access to device setup functions, using the
# command "run-script vacuum_setup.py"
#
# Then add devices using the phys_mem.add-map command.
#

def add_config(obj_list):
    try:
        SIM_add_configuration(obj_list, None)
        return 1
    except Exception as msg:
        print(f'''Failed loading the configuration in Simics. This is probably
due to some misconfiguration, or that some required file is missing.

Error message: {msg}''')
        return 0

clock = pre_conf_object('timer', 'clock')
clock.freq_mhz = 20
ram_image = pre_conf_object('ram_image', 'image')
ram_image.size = 0x10000000
ram_image.queue = clock
ram = pre_conf_object('ram', 'ram')
ram.image = ram_image
space = pre_conf_object('phys_mem', 'memory-space')
#             base         object  function  offset  length
space.map = [[0x10000000,  ram,    0,        0,      0x10000000]]
space.queue = clock

if add_config((ram_image, ram, space, clock)):
    print('''Welcome to vacuum, a machine for testing devices in isolation.

Please add devices using the phys_mem.add-map command.
To read from memory or device, use 'phys_mem.read <addr> -l/-b'.
To write to memory or device, use 'phys_mem.write <addr> <value> -l/-b.

For help on adding devices and read/write options regarding transfer sizes,
use 'help phys_mem.add-map', 'help phys_mem.read',
and 'help phys_mem.write' commands.''')
