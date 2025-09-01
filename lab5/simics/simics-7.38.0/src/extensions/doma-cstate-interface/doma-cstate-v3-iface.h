/*
  Doma cstate - aggregator of cpus cstates.

  © 2024 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef DOMA_CSTATE_V3_IFACE_H
#define DOMA_CSTATE_V3_IFACE_H

#include <simics/device-api.h>
#include <simics/pywrap.h>

#ifdef __cplusplus
extern "C" {
#endif

/* <add-type id="x86_doma_cstate_info_v2_t"> </add-type> */
typedef struct {
        uint32 state;
        uint32 sub_state;
        uint64 deadline;
        uint64 tsc;
} x86_doma_cstate_info_v2_t;
SIM_PY_ALLOCATABLE(x86_doma_cstate_info_v2_t);


/* <add id="x86_doma_cstate_v3">
   The interface is used to aggregate information about cpu cstates.

   <insert-until text="// ADD INTERFACE x86_doma_cstate_v3"/>
   </add> */
SIM_INTERFACE(x86_doma_cstate_v3)
{
        void (*notify_sleep)(conf_object_t *listener, conf_object_t *cpu,
                const x86_doma_cstate_info_v2_t *info);
        void (*notify_resume)(conf_object_t *listener,
                const x86_doma_cstate_info_v2_t *info);
        void (*notify_waiting_interrupt)(conf_object_t *listener,
                bool is_waiting);
        void (*notify_reset)(conf_object_t *listener);
};

#define X86_DOMA_CSTATE_V3_INTERFACE "x86_doma_cstate_v3"
// ADD INTERFACE x86_doma_cstate_v3

#ifdef __cplusplus
}
#endif

#endif /* ! DOMA_CSTATE_V3_IFACE_H */
