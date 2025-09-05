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

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_DIRECT_MEMORY_UPDATE_GASKET_ADAPTER_H
#define SIMICS_SYSTEMC_SIMICS2TLM_DIRECT_MEMORY_UPDATE_GASKET_ADAPTER_H

#include <simics/systemc/iface/direct_memory_update_interface.h>
#include <simics/systemc/simics2tlm/gasket_adapter.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

/**
 * Adapter for DirectMemoryUpdate gasket.
 */
class DirectMemoryUpdateGasketAdapter
    : public iface::DirectMemoryUpdateInterface,
      public GasketAdapter<iface::DirectMemoryUpdateInterface> {
  public:
    DirectMemoryUpdateGasketAdapter(
            DirectMemoryUpdateInterface *direct_mem_update,
            iface::SimulationInterface *simulation)
        : direct_mem_update_(direct_mem_update),
          simulation_(simulation) {
    }
    virtual ~DirectMemoryUpdateGasketAdapter() = default;

    // iface::DirectMemoryUpdateInterface
    void release(conf_object_t *target,
                 direct_memory_handle_t handle,
                 direct_memory_ack_id_t id) override {
        if (simulation_) {
            Context context(simulation_);
            direct_mem_update_->release(target, handle, id);
        } else {
            direct_mem_update_->release(target, handle, id);
        }
    }
    void update_permission(conf_object_t *target,
                           direct_memory_handle_t handle,
                           access_t lost_access,
                           access_t lost_permission,
                           access_t lost_inhibit,
                           direct_memory_ack_id_t id) override {
        if (simulation_) {
            Context context(simulation_);
            direct_mem_update_->update_permission(target, handle, lost_access,
                                                  lost_permission, lost_inhibit,
                                                  id);
        } else {
            direct_mem_update_->update_permission(target, handle, lost_access,
                                                  lost_permission, lost_inhibit,
                                                  id);
        }
    }
    void conflicting_access(conf_object_t *target,
                            direct_memory_handle_t handle,
                            access_t conflicting_permission,
                            direct_memory_ack_id_t id) override {
        if (simulation_) {
            Context context(simulation_);
            direct_mem_update_->conflicting_access(target, handle,
                                                   conflicting_permission, id);
        } else {
            direct_mem_update_->conflicting_access(target, handle,
                                                   conflicting_permission, id);
        }
    }

    // GasketAdapter<iface::DirectMemoryUpdateInterface>
    simics2tlm::GasketOwner *gasket_owner() const override {
        return dynamic_cast<simics2tlm::GasketOwner *>(direct_mem_update_);
    }

  private:
    DirectMemoryUpdateInterface *direct_mem_update_;
    iface::SimulationInterface *simulation_;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
