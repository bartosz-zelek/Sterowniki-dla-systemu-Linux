# © 2013 Intel Corporation
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
import stest

def on_log(cb_data, trigger_obj, typ, message):
    pass

# We need some object references here to test with
dma_image = simics.SIM_create_object('image', 'dma_image', size=1 << 20)
dma_ram = simics.SIM_create_object('ram', 'dma_ram', image=dma_image)
(dma, dma_space, mock, regs) = transfer.setup_test(dma_ram, on_log)

# Delete dma object should delete also it's cycle object and all proxies.
SIM_delete_objects([conf.dma])

names = {x.name for x in SIM_object_iterator(None)}
stest.expect_false('dma' in names)
stest.expect_false([x for x in names if x.startswith('dma.')])

# Check that other objects can be deleted now
SIM_run_command('dma_space.del-map device = dma_ram')
SIM_delete_objects([conf.dma_image, conf.dma_ram])
