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


from simics import SIM_create_object
from cli import simenv, CliError
import dev_util as du
import conf

# SIMICS-21543
conf.sim.deprecation_level = 0

def unique_name(name):
    """Returns a unique, unused, name"""
    if hasattr(conf, '%s' % name):
        i = 0
        while hasattr(conf, '%s%d' % (name, i)):
            i += 1
        name = '%s%d' % (name, i)
    return name

def create_dma_registers(dma_device):
    class regs:
        pass

    regs.dma_control = du.Register_LE(dma_device, 0, size = 4,
                                      bitfield = du.Bitfield_LE(
                                          {'EN': 31,
                                           'SWT': 30,
                                           'ECI': 29,
                                           'TC': 28,
                                           'SG': 27,
                                           'ERR': 26,
                                           'TS': (15,0)}))
    regs.dma_source = du.Register_LE(dma_device, 4, size = 4)
    regs.dma_destination = du.Register_LE(dma_device, 8, size = 4)

    return regs

def create_dma_device(name='dma', dma_space=None, signal=None, cell=None):
    name = unique_name(name)

    # Must use a real memory-space object that implements the memory_page
    # interface in order to test DMI
    if not dma_space:
        dma_image = SIM_create_object('image', '%s_image' % name,
                                      [['size', 1 << 20]])
        dma_ram = SIM_create_object('ram', '%s_ram' % name,
                                    [['image', dma_image]])
        dma_space = SIM_create_object('memory-space', '%s_space' % name)
        dma_space.map = [[0x0, dma_ram, 0, 0, dma_image.size]]

    # Allow test case to be reused for other classes by specifying $test_class
    try:
        class_name = simenv.test_class
    except CliError:
        class_name = 'sample_tlm2_dma_device'
    except AttributeError:
        class_name = 'sample_tlm2_dma_device'

    dma = SIM_create_object(class_name, name,
                            [['phys_mem', dma_space],
                             ['cell', cell]])

    if not signal:
        mock = du.Dev([du.Signal])
        signal = mock.obj

    # Set interrupt target after creating the object to test optional
    # connect attributes.
    dma.irq_target = signal

    return dma, create_dma_registers(dma)
