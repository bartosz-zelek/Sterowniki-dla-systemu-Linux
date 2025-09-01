# © 2019 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import simics

# Simics object doing post_instantiate tasks. Could be replaced
# with a function registering tasks to be done after instantiation.

# Reuse CMOS commands
class X58MB:
    cls = simics.confclass("x58-mb", doc = "X58 motherboard.",
                           short_doc = "an X58 motherboard")

    cls.attr.rtc("o", doc = "The rtc object, used for CMOS commands.")
    @staticmethod
    def get_rtc(obj):
        return obj.attr.rtc

registered = False

def cmos_post_instantiate():
    from simmod.x86_comp.x86_motherboard import register_cmos_commands
    global registered
    if not registered:
        register_cmos_commands('x58-mb', X58MB.get_rtc)
        registered = True
