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

#ifndef SIMICS_BASE_SIM_EXCEPTION_H
#define SIMICS_BASE_SIM_EXCEPTION_H

#include <simics/host-info.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define FOR_ALL_EXCEPTIONS(op)                                  \
        op(SimExc_No_Exception,         SimExc_No_Exception),   \
        op(SimExc_General,              SimExc_General),        \
        op(SimExc_Lookup,               SimExc_General),        \
        op(SimExc_Attribute,            SimExc_General),        \
        op(SimExc_IOError,              SimExc_General),        \
        op(SimExc_Index,                SimExc_General),        \
        op(SimExc_Memory,               SimExc_General),        \
        op(SimExc_Type,                 SimExc_General),        \
        op(SimExc_Break,                SimExc_General),        \
        op(SimExc_Stop,                 SimExc_General),        \
        op(SimExc_PythonTranslation,    SimExc_General),        \
        op(SimExc_License,              SimExc_General),        \
        op(SimExc_IllegalValue,         SimExc_Attribute),      \
        op(SimExc_InquiryOutsideMemory, SimExc_Memory),         \
        op(SimExc_InquiryUnhandled,     SimExc_Memory),         \
        op(SimExc_InterfaceNotFound,    SimExc_Lookup),         \
        op(SimExc_AttrNotFound,         SimExc_Lookup),         \
        op(SimExc_AttrNotReadable,      SimExc_Attribute),      \
        op(SimExc_AttrNotWritable,      SimExc_Attribute)

#define ENUM_DEF(this, parent) this

typedef enum sim_exception {
        FOR_ALL_EXCEPTIONS(ENUM_DEF),
        Sim_Exceptions
} sim_exception_t;

#undef ENUM_DEF

#if !defined(__LEAVE_EXCEPTION_DEFS)
#undef FOR_ALL_EXCEPTIONS
#endif

#if !defined(GULP_PYTHON)
EXPORTED sim_exception_t SIM_clear_exception(void);
EXPORTED sim_exception_t SIM_get_pending_exception(void);
#endif

EXPORTED sim_exception_t DBG_get_pending_exception(void);

EXPORTED const char *SIM_last_error(void);

EXPORTED void VT_frontend_exception(sim_exception_t exc_type,
                                    const char *NOTNULL str);

#if defined(__cplusplus)
}
#endif

#endif
