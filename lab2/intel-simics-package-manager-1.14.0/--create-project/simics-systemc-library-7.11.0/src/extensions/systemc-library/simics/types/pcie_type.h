// -*- mode: C++; c-file-style: "virtutech-c++" -*-

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

#ifndef SIMICS_TYPES_PCIE_TYPE_H
#define SIMICS_TYPES_PCIE_TYPE_H

namespace simics {
namespace types {

typedef enum {
    PCIE_Type_Not_Set,
    PCIE_Type_Mem,
    PCIE_Type_IO,
    PCIE_Type_Cfg,
    PCIE_Type_Msg,
    PCIE_Type_Other,
} pcie_type_t;

typedef enum {
    /* Address Translation */
    PCIE_ATS_Invalidate = 0x01,
    PCIE_ATS_Invalidate_Completion = 0x02,
    PCIE_PRS_Request    = 0x04,
    PCIE_PRS_Response   = 0x05,

    PCIE_Latency_Tolerance_Reporting = 0x10,
    PCIE_Optimized_Buffer_Flush_Fill = 0x12,

    /* INTx emulation */
    PCIE_Msg_Assert_INTA       = 0x20,
    PCIE_Msg_Assert_INTB       = 0x21,
    PCIE_Msg_Assert_INTC       = 0x22,
    PCIE_Msg_Assert_INTD       = 0x23,
    PCIE_Msg_Deassert_INTA     = 0x24,
    PCIE_Msg_Deassert_INTB     = 0x25,
    PCIE_Msg_Deassert_INTC     = 0x26,
    PCIE_Msg_Deassert_INTD     = 0x27,

    /* Power Management */
    PCIE_PM_Active_State_Nak   = 0x14,
    PCIE_PM_PME                = 0x18,
    PCIE_PM_Turn_Off           = 0x19,
    PCIE_PM_PME_TO_Ack         = 0x1B,

    /* Error Messages */
    PCIE_ERR_COR               = 0x30,
    PCIE_ERR_NONFATAL          = 0x31,
    PCIE_ERR_FATAL             = 0x33,

    /* Locked Transaction */
    PCIE_Unlock                = 0x00,

    /* Slot Power Limit */
    PCIE_Set_Slot_Power_Limit  = 0x50,

    PCIE_Precision_Time_Measurement = 0x52,

    /* Hot Plug Messages */
    PCIE_HP_Power_Indicator_On        = 0x45,
    PCIE_HP_Power_Indicator_Blink     = 0x47,
    PCIE_HP_Power_Indicator_Off       = 0x44,
    PCIE_HP_Attention_Button_Pressed  = 0x48,
    PCIE_HP_Attention_Indicator_On    = 0x41,
    PCIE_HP_Attention_Indicator_Blink = 0x43,
    PCIE_HP_Attention_Indicator_Off   = 0x40,

    PCIE_Vendor_Defined_Type_0 = 0x7e,
    PCIE_Vendor_Defined_Type_1 = 0x7f,

    PCIE_Locked_Transaction    = 0x00,  // legacy name for PCIE_Unlock

    /* Data Link Layer (virtual) Messages

       NOTE: these messages only exist on Simics simulator, as they are
       normally part of the Data Link Layer which is below the level of
       abstraction for Simics PCIe models

       According to PCIe rev 2.0, when a target receives a message that it
       does not recognize or support, except for the "Vendor Defined Type
       1" message, it should treat the request as an "Unsupported Request"
       and report it accordingly (see sec 2.3.1 for reference).

       Hence models that comply with rev 2.0 must be updated to either
       1) handle these messages or 2) ignore these messages.

       Ideally we would like to use a new pcie_link interface to transmit
       this information - see bug 17849 for more info. */
    PCIE_DLL_Link_Down = -1,
    PCIE_DLL_Link_Up = -2,
} pcie_message_type_t;

struct pcie_ide_secured_t {
    bool t;
    bool k;
    bool m;
    bool p;
    uint8_t sub_stream;
    uint8_t stream_id;
    uint8_t pr_sent_counter;
};

}  // namespace types
}  // namespace simics

#endif
