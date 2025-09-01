// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2013 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/base/conf-object.h>
#include <simics/systemc/tlm2simics/dmi_transaction_handler.h>
#include <simics/systemc/adapter_log_groups.h>

namespace simics {
namespace systemc {
namespace tlm2simics {

DmiTransactionHandler::DmiTransactionHandler(
        InterfaceProvider *interface_provider,
        iface::ReceiverInterface *ignore_receiver,
        InterfaceProvider *dmi_interface_provider)
    : TransactionHandler(dmi_interface_provider
                         ? dmi_interface_provider : interface_provider,
                         ignore_receiver, interface_provider)
    , direct_memory_provider_("direct_memory")
    , direct_memory_lookup_provider_("direct_memory_lookup")
    , has_direct_memory_update_iface_checked_(false)
    , has_direct_memory_update_iface_(false) {
    // Don't call interface_provider_ from here. In general the object will not
    // be fully constructed, so it is only safe to store a pointer to it.
}

DmiTransactionHandler::~DmiTransactionHandler() {
    interface_provider_->remove_target_update_listener(this);
}

bool DmiTransactionHandler::get_direct_mem_ptr(
        ConfObjectRef &simics_obj,
        tlm::tlm_generic_payload &trans,
        tlm::tlm_dmi& dmi_data) {
    const access_t R = Sim_Access_Read;
    const access_t W = Sim_Access_Write;

    if (!hasDirectMemoryUpdateInterface(simics_obj)) {
        SIM_LOG_INFO(4, simics_obj, Log_TLM,
                     "DMI is not supported in %s",
                     simics_obj.name().c_str());
        return false;
    }

    SimicsTargetLock<const direct_memory_lookup_interface_t>
        direct_memory_lookup_iface = direct_memory_lookup();

      if (direct_memory_lookup_iface == NULL) {
        SIM_LOG_INFO(4, simics_obj, Log_TLM,
                     "DMI is not supported in %s",
                     SIM_object_name(
                         interface_provider_->target().object()));
        return false;
    }

    if (!trans.is_read() && !trans.is_write()) {
        SIM_LOG_INFO(4, simics_obj, Log_TLM,
                     "Transaction is neither read nor write");
        return false;
    }

    // Parse DMI parameters
    access_t access = trans.is_read() ? R : W;
    physical_address_t pa = trans.get_address();

    physical_address_t alloc_pa = pa & ~(4096 - 1);
    unsigned alloc_size = 4096;

    SIM_LOG_INFO(4, simics_obj, Log_TLM,
                 "DMI %s request received at addr 0x%llx",
                 access == Sim_Access_Read ? "read" : "write", pa);

    direct_memory_lookup_t dml = direct_memory_lookup_iface->lookup(
        interface_provider_->target().object(), simics_obj,
        alloc_pa, alloc_size, access);

    if (!dml.target) {
        SIM_LOG_INFO(4, simics_obj, Log_TLM,
                     "DMI is not supported for addr 0x%llx with size: %u",
                     alloc_pa, alloc_size);
        return false;
    }

    if ((dml.breakpoints & access) || (dml.tracers & access)) {
        SIM_LOG_INFO(4, simics_obj, Log_TLM,
                     "DMI not allowed since breakpoint or tracer is present");
        return false;
    }

    SimicsTargetLock<const direct_memory_interface_t> dm_iface = direct_memory(
        dml.target);
    direct_memory_handle_t handle = dm_iface->get_handle(
        dml.target, simics_obj, 0, dml.offs, alloc_size);
    if (!handle) {
        SIM_LOG_INFO(4, simics_obj, Log_TLM,
                     "DMI is not supported for addr 0x%llx with size: %u",
                     alloc_pa, alloc_size);
        return false;
    }

    access_t permission = access;
    access_t inhibit = (access == R) ? (W)
        : static_cast<access_t>(static_cast<int>(R) | static_cast<int>(W));

    direct_memory_t dm = dm_iface->request(
        dml.target, handle, permission, inhibit);

    FATAL_ERROR_IF(!dm.data, "direct_memory.request returned NULL.");

    void *user_data = gasket_.get();
    dm_iface->set_user_data(dml.target, handle, user_data);

    // Form DMI descriptor
    dmi_data.set_dmi_ptr(dm.data);
    dmi_data.set_start_address(alloc_pa);
    dmi_data.set_end_address(alloc_pa + alloc_size - 1);

    // Form DMI descriptor
    tlm::tlm_dmi::dmi_access_e dmi_access = tlm::tlm_dmi::DMI_ACCESS_NONE;
    int dm_perm = static_cast<int>(dm.permission & (R | W));
    switch (dm_perm) {
    case R:
        dmi_access = tlm::tlm_dmi::DMI_ACCESS_READ;
        break;
    case W:
        dmi_access = tlm::tlm_dmi::DMI_ACCESS_WRITE;
        break;
    case R | W:
        dmi_access = tlm::tlm_dmi::DMI_ACCESS_READ_WRITE;
        break;
    default:
        SIM_LOG_ERROR(simics_obj, Log_TLM,
                      "DMI granted access type incorrect (%d)",
                      dm_perm);
    }
    dmi_data.set_granted_access(dmi_access);

    SIM_LOG_INFO(4, simics_obj, Log_TLM,
                 "DMI access granted at start addr 0x%llx with size %u",
                 alloc_pa, alloc_size);

    // Since we now have granted DMI access to the device, we need to be
    // notified if the Simics object is changed, since that requires us to
    // invalidate the DMI pointers.
    interface_provider_->add_target_update_listener(this);

    return true;
}

void DmiTransactionHandler::update_dmi_allowed(
    ConfObjectRef &simics_obj,
    tlm::tlm_generic_payload *trans) {

    const access_t R = Sim_Access_Read;
    const access_t W = Sim_Access_Write;

    trans->set_dmi_allowed(false);
    if (trans->is_dmi_allowed())
        return;
    if (!interface_provider_->target())
        return;
    if (!hasDirectMemoryUpdateInterface(simics_obj))
        return;

    SimicsTargetLock<const direct_memory_lookup_interface_t>
        direct_memory_lookup_iface = direct_memory_lookup();
    if (direct_memory_lookup_iface == NULL)
        return;

    if (!trans->is_read() && !trans->is_write()) {
        SIM_LOG_INFO(4, simics_obj, Log_TLM,
                     "Transaction is neither read nor write");
        return;
    }

    // Parse DMI parameters
    access_t access = trans->is_read() ? R : W;
    physical_address_t pa = trans->get_address();

    physical_address_t alloc_pa = pa & ~(4096 - 1);
    unsigned alloc_size = 4096;

    direct_memory_lookup_t dml = direct_memory_lookup_iface->lookup(
        interface_provider_->target().object(), simics_obj,
        alloc_pa, alloc_size, access);

    if (!dml.target)
        return;

    if ((dml.breakpoints & access) || (dml.tracers & access))
        return;

    trans->set_dmi_allowed(true);
}

void DmiTransactionHandler::update_target(ConfObjectRef old_target,
        ConfObjectRef new_target) {
    if (gasket_) {
        gasket_->invalidate_direct_mem_ptr(0, ~static_cast<sc_dt::uint64>(0));
    }
}

SimicsTargetLock<const direct_memory_interface_t>
DmiTransactionHandler::direct_memory(ConfObjectRef target) {
    if (target != direct_memory_provider_.target()) {
        direct_memory_provider_.set_target(target);
    }
    return direct_memory_provider_.get_interface<
            const direct_memory_interface_t>();
}

SimicsTargetLock<const direct_memory_lookup_interface_t>
DmiTransactionHandler::direct_memory_lookup() {
    if (interface_provider_->target()
        != direct_memory_lookup_provider_.target()) {
        direct_memory_lookup_provider_.set_target(
            interface_provider_->target());
    }
    return direct_memory_lookup_provider_.get_interface<
            const direct_memory_lookup_interface_t>();
}

bool DmiTransactionHandler::hasDirectMemoryUpdateInterface(
        ConfObjectRef simics_obj) {
    // From the documentation of the direct_memory interface:
    // "A memory user using the direct_memory interface must
    // implement the direct_memory_update interface."
    if (!has_direct_memory_update_iface_checked_) {
        has_direct_memory_update_iface_checked_ = true;
        has_direct_memory_update_iface_
            = simics_obj.get_interface("direct_memory_update") != NULL;
    }

    return has_direct_memory_update_iface_;
}

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics
