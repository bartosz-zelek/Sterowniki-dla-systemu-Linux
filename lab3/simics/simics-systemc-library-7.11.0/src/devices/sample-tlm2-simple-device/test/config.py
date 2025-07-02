# © 2014 Intel Corporation
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
import conf

def unique_name(name):
    """Returns a unique, unused, name"""
    if hasattr(conf, '%s' % name):
        i = 0
        while hasattr(conf, '%s%d' % (name, i)):
            i += 1
        name = '%s%d' % (name, i)
    return name

def create_simple_device(name='simple_device'):
    simple_device = SIM_create_object('sample_tlm2_simple_device',
                                      unique_name(name),
                                      [['access_delay', 3]])
    return simple_device
