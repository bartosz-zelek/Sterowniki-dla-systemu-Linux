/*
  ERI - external register interface
  Also used for implementing DCRA interface, because they so close.

  © 2023 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef ERI_V1_IFACE_H
#define ERI_V1_IFACE_H

#include <simics/device-api.h>
#include <simics/pywrap.h>

#ifdef __cplusplus
extern "C" {
#endif

// Registers number used by ERI or DCRA
/* <add-type id="eri_reg_num_t"> </add-type> */
typedef enum {
    DCRA_ERI_PMA_RESOLUTION_CONTROL = 0,
    ERI_PMA_CORE_TTT_L32,   // low 32-bit of arat_ttt small core
    ERI_PMA_CORE_TTT_H32,   // high 32-bit of arat_ttt small core
    DCRA_ERI_PMA_CORE_STATUS, 
    DCRA_PMA_ACODE_TO_UCODE_CFG,
    DCRA_PMA_ARAT_TTT,      // arat_ttt of big core
    DCRA_ERI_PMA_CSTT_THRESHOLD,
    ERI_PMA_CR_UCODE_ASSIST_INFO,
} eri_reg_num_t;

/* <add-type id="eri_transaction_t"> </add-type> */
typedef struct {
        uint64 val;   // Read or Write value
        bool   valid; // Valid or not data in val field
} eri_transaction_t;
SIM_PY_ALLOCATABLE(eri_transaction_t);

/* <add id="x86_eri_v1">
   The interface is used to read/writer external registers.

   The <arg>data</arg> is owned by caller. Callee always fills field valid to
   indicate transaction result. Field val contains the result of read method 
   and input for write method.

   <insert-until text="// ADD INTERFACE x86_eri_v1"/>
   </add> */
SIM_INTERFACE(x86_eri_v1) {
        void (*read)(conf_object_t *listener,
                uint64 reg_num, eri_transaction_t *data);
        void (*write)(conf_object_t *listener,
                uint64 reg_num, eri_transaction_t *data);
};

#define X86_ERI_V1_INTERFACE "x86_eri_v1"
// ADD INTERFACE x86_eri_v1

#ifdef __cplusplus
}
#endif

#endif /* ! ERI_V1_IFACE_H */
