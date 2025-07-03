// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_DIRECT_MEMORY_UPDATE_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_DIRECT_MEMORY_UPDATE_SIMICS_ADAPTER_H

#include <simics/model-iface/direct-memory.h>
#include <simics/systemc/iface/direct_memory_update_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** Adapter for Simics direct_memory_update interface. */
template<typename TBase,
         typename TInterface = DirectMemoryUpdateInterface>
class DirectMemoryUpdateSimicsAdapter
    : public SimicsAdapter<direct_memory_update_interface_t> {
  public:
    DirectMemoryUpdateSimicsAdapter()
        : SimicsAdapter<direct_memory_update_interface_t>(
            DIRECT_MEMORY_UPDATE_INTERFACE, init_iface()) {
    }

  protected:
    static void release(conf_object_t *obj,
                        conf_object_t *target,
                        direct_memory_handle_t handle,
                        direct_memory_ack_id_t id) {
        adapter<TBase, TInterface>(obj)->release(target, handle, id);
    }
    static void update_permission(conf_object_t *obj,
                                  conf_object_t *target,
                                  direct_memory_handle_t handle,
                                  access_t lost_access,
                                  access_t lost_permission,
                                  access_t lost_inhibit,
                                  direct_memory_ack_id_t id) {
        adapter<TBase, TInterface>(obj)->update_permission(
            target, handle, lost_access, lost_permission, lost_inhibit, id);
    }
    static void conflicting_access(conf_object_t *obj,
                                   conf_object_t *target,
                                   direct_memory_handle_t handle,
                                   access_t conflicting_permission,
                                   direct_memory_ack_id_t id) {
        adapter<TBase, TInterface>(obj)->conflicting_access(
            target, handle, conflicting_permission, id);
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    direct_memory_update_interface_t init_iface() {
        direct_memory_update_interface_t iface = {};
        iface.release = release;
        iface.update_permission = update_permission;
        iface.conflicting_access = conflicting_access;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
