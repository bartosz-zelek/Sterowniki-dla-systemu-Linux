/*
  © 2022 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/
#ifndef EXTENSIONS_X86_TSC_CYCLES_INTERFACE_TSC_CYCLES_INTERFACE_H
#define EXTENSIONS_X86_TSC_CYCLES_INTERFACE_TSC_CYCLES_INTERFACE_H
#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="x86_tsc_cycles">
    The interface is used to convert CPU time stamp counter values to CPU cycles
    and back. It is primarily intended for systems that use ART (always running
    timer), which have values reported by RDTSC scaled relative to actual
    processor clocks. See description of numerator and denominator values of
    CPUID leaf 0x15 in the Intel SDM.

    The <b>tsc_to_cycles</b> method is used to convert interval <b>tsc</b>
    measured in TSC time units to processor cycles.
    The <b>tsc_from_cycles</b> method is meant to convert interval <b>cycles</b>
    measured in processor cycle time units to value would have been returned by
    RDTSC.

    For systems that do not use ART, both methods may return input unchanged,
    thus yielding ratio 1:1.

   <insert-until text="// ADD INTERFACE x86_tsc_cycles"/>
   </add> */
SIM_INTERFACE(x86_tsc_cycles) {
    uint64  (*tsc_to_cycles)(conf_object_t *obj, uint64 tsc);
    uint64  (*tsc_from_cycles)(conf_object_t *obj, uint64 cycles);
};
#define X86_TSC_CYCLES_INTERFACE "x86_tsc_cycles"
// ADD INTERFACE x86_tsc_cycles

#if defined(__cplusplus)
}
#endif
#endif // EXTENSIONS_X86_TSC_CYCLES_INTERFACE_TSC_CYCLES_INTERFACE_H
