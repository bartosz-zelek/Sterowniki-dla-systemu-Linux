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

#ifndef SIMICS_SYSTEMC_IFACE_TEMPORAL_STATE_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_TEMPORAL_STATE_SIMICS_ADAPTER_H

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

#include <simics/systemc/iface/temporal_state_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

template<typename TBase, typename TInterface = TemporalStateInterface>
class TemporalStateSimicsAdapter
        : public SimicsAdapter<temporal_state_interface_t> {
  public:
    TemporalStateSimicsAdapter() : SimicsAdapter<
        temporal_state_interface_t>(TEMPORAL_STATE_INTERFACE, init_iface()) {}

  protected:
    static lang_void *save(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->save();
    }
    static void merge(conf_object_t *obj, lang_void *prev, lang_void *killed) {
        return adapter<TBase, TInterface>(obj)->merge(prev, killed);
    }
    static void prepare_restore(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->prepare_restore();
    }
    static void finish_restore(conf_object_t *obj, lang_void *state) {
        return adapter<TBase, TInterface>(obj)->finish_restore(state);
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    temporal_state_interface_t init_iface() {
        temporal_state_interface_t iface = {};
        iface.save = save;
        iface.merge = merge;
        iface.prepare_restore = prepare_restore;
        iface.finish_restore = finish_restore;

        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif  // SIMICS_SYSTEMC_IFACE_TEMPORAL_STATE_SIMICS_ADAPTER_H
