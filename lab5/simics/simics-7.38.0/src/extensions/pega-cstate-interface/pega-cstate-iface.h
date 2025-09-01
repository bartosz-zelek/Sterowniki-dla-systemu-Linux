/*
  © 2024 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef PEGA_CSTATE_IFACE_H
#define PEGA_CSTATE_IFACE_H

#include <simics/device-api.h>
#include <simics/pywrap.h>

#ifdef __cplusplus
extern "C" {
#endif

/* <add id="pega_cstate">

   PEGA interface is needed to process cstate changes initiated by PCODE, not
   by MWAIT instruction.

   <fun>set_cstate</fun> set cstate with calling cstate controller

   <insert-until text="// ADD INTERFACE pega_cstate_interface"/>
   </add> */
SIM_INTERFACE(pega_cstate)
{
        void (*set_cstate)(conf_object_t *obj, uint32 state,
                uint32 sub_state);
};

#define PEGA_CSTATE_INTERFACE "pega_cstate"
// ADD INTERFACE pega_cstate_interface

#ifdef __cplusplus
}
#endif

#endif /* ! PEGA_CSTATE_IFACE_H */

