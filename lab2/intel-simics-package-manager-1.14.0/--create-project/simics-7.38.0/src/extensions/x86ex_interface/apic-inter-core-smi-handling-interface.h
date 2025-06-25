/*
 © 2023 Intel Corporation

 This software and the related documents are Intel copyrighted materials, and
 your use of them is governed by the express license under which they were
 provided to you ("License"). Unless the License provides otherwise, you may
 not use, modify, copy, publish, distribute, disclose or transmit this software
 or the related documents without Intel's prior written permission.

 This software and the related documents are provided as is, with no express or
 implied warranties, other than those that are expressly stated in the License.
 */

#ifndef X86EX_INTERFACE_APIC_INTER_CORE_SMI_HANDLING_INTERFACE_H
#define X86EX_INTERFACE_APIC_INTER_CORE_SMI_HANDLING_INTERFACE_H

#include <simics/device-api.h>

#ifdef __cplusplus
extern "C" {
#endif
SIM_INTERFACE(apic_inter_core_smi_handling) {
    int (*handling_needed)(conf_object_t *obj);
    void (*stall_cpu)(conf_object_t *obj);
    void (*unstall_cpu)(conf_object_t *obj);
    void (*trigger_post_smi_send_action)(conf_object_t *obj,
                                         conf_object_t *target_apic);  // to be called on sender
    void (*trigger_smi_receive_action)(conf_object_t *obj,
                                       conf_object_t *src_apic);  // to be called on target
};

#define APIC_INTER_CORE_SMI_HANDLING_INTERFACE "apic_inter_core_smi_handling"
// ADD INTERFACE inter_core_smi_handling

#ifdef __cplusplus
}
#endif

#endif  // X86EX_INTERFACE_APIC_INTER_CORE_SMI_HANDLING_INTERFACE_H
