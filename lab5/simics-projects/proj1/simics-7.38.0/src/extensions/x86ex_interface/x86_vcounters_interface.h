/* x86_pmu_vcounters_interface.h - event counters reported by CPU */

/*
  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef X86_VCOUNTERS_INTERFACE_H
#define X86_VCOUNTERS_INTERFACE_H

#include <simics/device-api.h>
#include <simics/pywrap.h>

#ifdef __cplusplus
extern "C" {
#endif

SIM_INTERFACE(x86_vcounters) {
        uint64 (*read_vcounter)(conf_object_t *cpu, int index);
        void (*reset_vcounter)(conf_object_t *cpu, int index);
        void (*enable_vcounter)(conf_object_t *cpu, int index);
        void (*disable_vcounter)(conf_object_t *cpu, int index);
};
#define X86_VCOUNTERS_INTERFACE "x86_vcounters"

#ifdef __cplusplus
}
#endif

#endif /* ! X86_VCOUNTERS_INTERFACE_H */
