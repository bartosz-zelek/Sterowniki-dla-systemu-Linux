// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_TEMPORAL_STATE_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_TEMPORAL_STATE_INTERFACE_H

#include <simics/base/types.h>

/* Some parts of SystemC is built with SHOW_OBSOLETE_API set and some is not. 
   We always need the temporal_state interface. */
#ifndef SHOW_OBSOLETE_API
#define SHOW_OBSOLETE_API
#define WE_DEFINED_SHOW_OBSOLETE_API
#endif
#include <simics/model-iface/temporal-state.h>
#ifdef WE_DEFINED_SHOW_OBSOLETE_API
#undef SHOW_OBSOLETE_API
#undef WE_DEFINED_SHOW_OBSOLETE_API
#endif

namespace simics {
namespace systemc {
namespace iface {

class TemporalStateInterface {
  public:
    virtual ~TemporalStateInterface() {}

    virtual lang_void *save() = 0;
    virtual void merge(lang_void *prev, lang_void *killed) = 0;
    virtual void prepare_restore() = 0;
    virtual void finish_restore(lang_void *state) = 0;
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif  // SIMICS_SYSTEMC_IFACE_TEMPORAL_STATE_INTERFACE_H
