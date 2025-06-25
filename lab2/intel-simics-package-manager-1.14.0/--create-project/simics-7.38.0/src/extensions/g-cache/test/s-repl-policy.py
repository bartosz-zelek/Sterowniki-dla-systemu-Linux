# © 2011 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import stest
from comp import pre_obj
import pyobj

class FakeCpu(pyobj.ConfObject):
    pass

def get_physical_breakpoint_block_size(obj):
    return 1

ifc = stc_interface_t(get_physical_breakpoint_block_size
                      = get_physical_breakpoint_block_size)

SIM_register_interface("FakeCpu", "stc", ifc)

simics.SIM_create_object("FakeCpu", "cpu0")

def create_cache(repl_policy):
    c = pre_obj('c$', 'g-cache',
                config_replacement_policy = repl_policy,
                cpus = conf.cpu0)
    SIM_add_configuration([c], None)
    return c.config_replacement_policy == repl_policy

stest.expect_true(create_cache('random'))
stest.expect_true(create_cache('lru'))
stest.expect_true(create_cache('cyclic'))
stest.expect_exception(create_cache, ['asdf'], SimExc_Attribute)
