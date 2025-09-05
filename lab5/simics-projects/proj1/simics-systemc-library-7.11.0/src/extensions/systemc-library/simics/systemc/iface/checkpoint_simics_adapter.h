// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_CHECKPOINT_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_CHECKPOINT_SIMICS_ADAPTER_H

#include <simics/simulator-iface/checkpoint.h>
#include <simics/systemc/iface/checkpoint_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** Adapter for Simics checkpoint interface. */
template<typename TBase, typename TInterface = CheckpointInterface>
class CheckpointSimicsAdapter : public SimicsAdapter<checkpoint_interface_t> {
  public:
    CheckpointSimicsAdapter()
        : SimicsAdapter<checkpoint_interface_t>(
            CHECKPOINT_INTERFACE, init_iface()) {
    }

  protected:
    static void save(conf_object_t *obj, const char *path, save_flags_t flags) {
        return adapter<TBase, TInterface>(obj)->save(path);
    }
    static void finish(conf_object_t *obj, int success) {
        return adapter<TBase, TInterface>(obj)->finish(success);
    }
    static int has_persistent_data(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->has_persistent_data();
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    checkpoint_interface_t init_iface() {
        checkpoint_interface_t iface = {};
        iface.save_v2 = save;
        iface.finish  = finish;
        iface.has_persistent_data = has_persistent_data;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
