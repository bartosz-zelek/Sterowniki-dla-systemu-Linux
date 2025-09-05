# © 2023 Intel Corporation
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
import conf
from simmod.x58_ich10_comp.x58_ich10 import get_module_for_class
from itertools import chain

def extract_needed_apic_bus_class(cpuname):
    cpu_comp_if = simics.SIM_get_object(cpuname).iface.component
    pre_apics = [cpu_comp_if.get_slot_value(e)
                 for e in cpu_comp_if.get_slots() if 'apic' in e]
    apic_classes = set([
        e.__class_name__
        for e in chain.from_iterable(chain.from_iterable(pre_apics))])
    if len(apic_classes) != 1:
        simics.SIM_log_critical(
            conf.sim, 0,
            f"Different apic classes ({apic_classes})"
            f" used in class {simics.SIM_get_object(cpuname).classname}."
            " Not supported.")
        return 'invalid'
    module = get_module_for_class(apic_classes.pop(), conf.sim)
    bus_classes = [ cl for cl in module[7] if 'bus' in cl ]
    if len(bus_classes) != 1:
        simics.SIM_log_critical(conf.sim, 0, "Found none or multiple bus classes"
                                f" ({bus_classes}) in module {module[0]}.")
    return bus_classes[0]
