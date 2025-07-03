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

simics_add_module(x86ex_interface
  INTERFACES x86_txt_chipset x86_unplug x86_unplug_v2 x86_external_reset x86_smm_notification apic_bus_v2 apic_timer x86_sai mktme apic_to_apic_bus apic_bus_to_apic x86_cstate_notification_v2 x86_monitor_notification x86_monitor x86_vcounters apic_inter_core_smi_handling
  SOURCES x86ex_interface.h apic_to_apic_bus_interface.h apic_bus_to_apic_interface.h x86-cstate-v2-iface.h x86_monitor_notification_interface.h x86_monitor_interface.h x86_vcounters_interface.h apic-inter-core-smi-handling-interface.h
  SIMICS_API latest
)
