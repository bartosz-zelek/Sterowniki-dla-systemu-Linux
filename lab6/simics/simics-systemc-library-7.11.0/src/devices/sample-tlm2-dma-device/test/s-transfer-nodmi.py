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


import transfer
import simics
import stest
import pyobj
import dev_util as du

# The actual test takes place here: with our fake memory that does not support
# DMI, we don't expect any DMI entries in the log
def on_log(cb_data, trigger_obj, typ, message):
    stest.expect_equal(message.find('DMI write access'), -1)
    stest.expect_equal(message.find('DMI read access'), -1)

class fake_mem(pyobj.ConfObject):
    '''Fake memory to test DMI/no-DMI'''
    _class_desc = "mock device to test DMI/no-DMI"

    def _initialize(self):
        super()._initialize()

    class image(pyobj.SimpleAttribute(None, 'o')):
        """Backing image"""

    class io_memory(pyobj.Interface):
        def operation(self, mop, info):
            # offset within our device
            offset = (SIM_get_mem_op_physical_address(mop)
                      + info.start - info.base)
            size = SIM_get_mem_op_size(mop)
            img = self._up.image.val.iface.image

            if SIM_mem_op_is_read(mop):
                data = tuple(img.get(offset, size))
                value = du.tuple_to_value_le(data)
                SIM_set_mem_op_value_le(mop, value)

            if SIM_mem_op_is_write(mop):
                value = SIM_get_mem_op_value_le(mop)
                data = du.value_to_tuple_le(value, size)
                img.set(offset, bytes(data))

            return simics.Sim_PE_No_Exception

dma_image = SIM_create_object('image', 'dma_image', size=1 << 20)
dma_ram = SIM_create_object('fake_mem', 'dma_ram', image=dma_image)
(dma, dma_space, mock, regs) = transfer.setup_test(dma_ram, on_log)

def test():
    transfer.run_test(dma, dma_space, mock, regs)

test()
