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


import update_checkpoint

def apic_freq_mhz_type_update(cpu_comp):
    cpu_comp.apic_freq_mhz = float(cpu_comp.apic_freq_mhz)

# Needs to match list in Makefile
module_classes = ("processor_core_i7",
                  "processor_core_i7_single",
                  "processor_core_i7_duo",
                  "processor_core_i7_6c_2t",
                  "processor_xeon_5500",
                  "processor_xeon_5530",
                  "processor_core_i7_8c_4t",
                  "processor_x86_intel64",
                  "processor_x86QSP1")

for cl in module_classes:
    update_checkpoint.install_class_configuration_update(
        5085, cl, apic_freq_mhz_type_update)
