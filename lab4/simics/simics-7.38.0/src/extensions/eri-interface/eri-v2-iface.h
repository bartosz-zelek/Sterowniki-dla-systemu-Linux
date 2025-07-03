/*
  ERI - external register interface
  Also used for implementing DCRA interface, because they so close.

  © 2024 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef ERI_V2_IFACE_H
#define ERI_V2_IFACE_H

#include <simics/device-api.h>
#include <simics/pywrap.h>

#ifdef __cplusplus
extern "C" {
#endif

/* <add-type id="eri_v2_transaction_t"> </add-type> */
typedef struct {
        uint32 value; // read or write value
        uint16 addr;
        uint8  scope;
        uint8  type;
        uint8  wrbe;  // write bytes enable mask
        bool   valid; // valid or not data in val field
} eri_v2_transaction_t;
SIM_PY_ALLOCATABLE(eri_v2_transaction_t);

/* <add id="x86_eri_v2">
   The interface is used to read/writer external registers.

   The <arg>data</arg> is owned by caller. Callee always fills field valid to
   indicate transaction result. Field val contains the result of read method 
   and input for write method.

   <insert-until text="// ADD INTERFACE x86_eri_v2"/>
   </add> */
SIM_INTERFACE(x86_eri_v2) {
        void (*read)(conf_object_t *listener, eri_v2_transaction_t *data);
        void (*write)(conf_object_t *listener, eri_v2_transaction_t *data);
};

#define X86_ERI_V2_INTERFACE "x86_eri_v2"
// ADD INTERFACE x86_eri_v2

#ifdef __cplusplus
}
#endif

#endif /* ! ERI_V2_IFACE_H */

