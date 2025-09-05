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

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_DIRECT_MEMORY_UPDATE_H
#define SIMICS_SYSTEMC_SIMICS2TLM_DIRECT_MEMORY_UPDATE_H

#include <simics/systemc/tlm2simics/gasket_interface.h>
#include <simics/systemc/interface_provider.h>
#include <simics/systemc/iface/direct_memory_update_interface.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

/**
 * Class that implements the Simics direct_memory_update interface. What makes
 * this class a bit special, is that it is _not_ derived from GasketOwner.
 * This is because it does not carry any information for the forward-path TLM2
 * protocol. Instead, it is intended to be associated with a tlm2simics gasket
 * and use this gasket to get the target socket so that it can invalidate the
 * DMI using the backward-path of the TLM2 protocol.
 */
class DirectMemoryUpdate : public iface::DirectMemoryUpdateInterface {
  public:
    DirectMemoryUpdate() : direct_memory_provider_("direct_memory") {}
    DirectMemoryUpdate(const DirectMemoryUpdate&) = delete;
    DirectMemoryUpdate& operator=(const DirectMemoryUpdate&) = delete;
    virtual ~DirectMemoryUpdate() = default;

    void release(conf_object_t *target,
                 direct_memory_handle_t handle,
                 direct_memory_ack_id_t id) {
        invalidate_direct_mem_ptr(target, handle, id);
    }
    void update_permission(conf_object_t *target,
                           direct_memory_handle_t handle,
                           access_t lost_access,
                           access_t lost_permission,
                           access_t lost_inhibit,
                           direct_memory_ack_id_t id) {
        invalidate_direct_mem_ptr(target, handle, id);
    }
    void conflicting_access(conf_object_t *target,
                            direct_memory_handle_t handle,
                            access_t conflicting_permission,
                            direct_memory_ack_id_t id) {
        // Assuming the systemc device does not construct any internal caches
        // based on the DMI pointer we don't have to do anything here.
        direct_memory(target)->ack(target, id);
    }

    // Deprecated
    void set_gasket(tlm2simics::GasketInterface::Ptr gasket) {
    }

  protected:
    // Helper function
    SimicsTargetLock<const direct_memory_interface_t> direct_memory(
            ConfObjectRef target) {
        if (target != direct_memory_provider_.target()) {
            direct_memory_provider_.set_target(target);
        }

        return direct_memory_provider_.get_interface<
            const direct_memory_interface_t>();
    }
    void invalidate_direct_mem_ptr(conf_object_t *target,
                                   direct_memory_handle_t handle,
                                   direct_memory_ack_id_t id) {
        SimicsTargetLock<const direct_memory_interface_t> dm_iface =
            direct_memory(target);
        void *user_data = dm_iface->get_user_data(target, handle);
        tlm2simics::GasketInterface *gasket =
            static_cast<tlm2simics::GasketInterface *>(user_data);
        if (gasket) {
            // TODO(zeffer): We don't have to invalidate all pages here. Add
            // fine-grain invalidation mechanism based on adapter_pages concept
            gasket->invalidate_direct_mem_ptr(0,
                                              static_cast<sc_dt::uint64>(-1));
        }

        dm_iface->ack(target, id);
    }

  private:
    InterfaceProvider direct_memory_provider_;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
