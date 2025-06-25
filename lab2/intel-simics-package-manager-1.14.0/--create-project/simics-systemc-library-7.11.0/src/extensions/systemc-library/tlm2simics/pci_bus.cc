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

#include <simics/systemc/tlm2simics/pci_bus.h>
#include <simics/systemc/adapter_log_groups.h>

#include <simics/devs/pci.h>
#include <simics/devs/io-memory.h>

namespace simics {
namespace systemc {
namespace tlm2simics {

namespace {
::map_info_t convertToSimics(types::map_info_t info) {
    ::map_info_t map_info = {};
    map_info.base = info.base;
    map_info.start = info.start;
    map_info.length = info.length;
    map_info.function = info.function;
    return map_info;
}

// decode the exception and log it, then return true if anything was logged
bool log_pci_bus_exception(exception_type_t ex, ConfObjectRef device_) {
    if (ex == Sim_PE_IO_Not_Taken) {
        SIM_LOG_INFO(4, device_, Log_TLM,
                     "PciBus::operation(): Master Abort received");
        return true;
    } else if (ex == Sim_PE_IO_Error) {
        SIM_LOG_INFO(4, device_, Log_TLM,
                     "PciBus::operation(): Target Abort received");
        return true;
    } else if (ex != Sim_PE_No_Exception) {
        SIM_LOG_INFO(4, device_, Log_TLM,
                     "PciBus::operation(): Unknown exception received"
                     " on PCI bus");
        return true;
    }
    return false;
}
}  // namespace

bool PciBus::get_direct_mem_ptr(ConfObjectRef &simics_obj,
                                tlm::tlm_generic_payload &trans,
                                tlm::tlm_dmi& dmi_data) {
    // To support DMI the DMI transaction handler needs a pointer to a memory
    // space object (implementing the memory_page interface). The pci-bus
    // object can provide this via the pci_bus interface.
    // At this point we _know_ that the pci_bus interface will return a valid
    // memory space object and that this object _will_ implement the
    // memory_page interface so there is no need to add any specific checks
    dmi_interface_provider_.set_target(ConfObjectRef(
        get_interface<pci_bus_interface_t>()->memory_space(target().object())));

    return DmiTransactionHandler::get_direct_mem_ptr(simics_obj, trans,
                                                     dmi_data);
}

void PciBus::raise_interrupt(int pin) {
    get_interface<pci_bus_interface_t>()->raise_interrupt(target().object(),
        device_, pin);
}

void PciBus::lower_interrupt(int pin) {
    get_interface<pci_bus_interface_t>()->lower_interrupt(target().object(),
        device_, pin);
}

int PciBus::interrupt_acknowledge() {
    return get_interface<pci_bus_interface_t>()->interrupt_acknowledge(
        target().object());
}

int PciBus::add_map(types::addr_space_t space, types::map_info_t info) {
    return get_interface<pci_bus_interface_t>()->add_map(target().object(),
        device_, static_cast< ::addr_space_t>(space), NULL,
        convertToSimics(info));
}

int PciBus::remove_map(types::addr_space_t space, int function) {
    return get_interface<pci_bus_interface_t>()->remove_map(target().object(),
        device_, static_cast< ::addr_space_t>(space), function);
}

void PciBus::set_bus_number(int bus_id) {
    get_interface<pci_bus_interface_t>()->set_bus_number(
        target().object(), bus_id);
}

void PciBus::set_sub_bus_number(int bus_id) {
    get_interface<pci_bus_interface_t>()->set_sub_bus_number(
        target().object(), bus_id);
}

void PciBus::add_default(types::addr_space_t space, types::map_info_t info) {
    get_interface<pci_bus_interface_t>()->add_default(target().object(),
        device_, static_cast< ::addr_space_t>(space), NULL,
        convertToSimics(info));
}

void PciBus::remove_default(types::addr_space_t space) {
    get_interface<pci_bus_interface_t>()->remove_default(
        target().object(), static_cast< ::addr_space_t>(space));
}

void PciBus::bus_reset() {
    get_interface<pci_bus_interface_t>()->bus_reset(target().object());
}

void PciBus::special_cycle(uint32_t value) {
    get_interface<pci_bus_interface_t>()->special_cycle(
        target().object(), value);
}

void PciBus::system_error() {
    get_interface<pci_bus_interface_t>()->system_error(target().object());
}

int PciBus::get_bus_address() {
    return get_interface<pci_bus_interface_t>()->get_bus_address(
        target().object(), device_);
}

void PciBus::set_device_status(int device, int function,
                               int enabled) {
    get_interface<pci_bus_interface_t>()->set_device_status(
        target().object(), device, function, enabled);
}

types::pci_bus_exception_type_t PciBus::read(uint16_t rid,
                                             types::addr_space_t space) {
    if (payload_->is_write()) {
        SIM_LOG_SPEC_VIOLATION(1, device_, Log_TLM,
                               "PciBus::read(): GP command set to write,"
                               " expected read");
        payload_->set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
        return types::PCI_BUS_GP_ERROR;
    }

    // NOTE: it is up to the initiator (i.e. the PCI device model) to verify
    // that the 'Bus Master Enable' bit has been set _before_ sending the
    // transaction to Simics. The gasket itself has no way of knowing this
    // (unlike our DMI code that is part of the device model). We could let the
    // composite::PciGasket's IC module handle this, by querying the bit using
    // a transport_dbg transaction back towards the model. The good: find bugs
    // in the SW by having a detailed model. The bad: unexpected. Conclusion:
    // left as future enhancement for now, given that gaskets should just
    // translate the interface and do as little protocol checking as possible.

    addr_space_t pci_space = static_cast<addr_space_t>(space);
    buffer_t buffer;
    buffer.data = payload_->get_data_ptr();
    buffer.len = payload_->get_data_length();
    exception_type_t ex = upstream_provider_.get_interface<
        pci_upstream_operation_interface_t>()->read(
            target().object(), device_,
            rid, pci_space, payload_->get_address(), buffer);
    // NOTE: again, it is up to the initiator (i.e. the PCI device model) to
    // handle the return value and set the corresponding status bits. Writing
    // back to the model would be too intrusive. Logging could be useful
    // though. Also set the correct error response.
    if (log_pci_bus_exception(ex, device_)) {
        payload_->set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
    }

    return static_cast<types::pci_bus_exception_type_t>(ex);
}

types::pci_bus_exception_type_t PciBus::write(uint16_t rid,
                                              types::addr_space_t space) {
    if (payload_->is_read()) {
        SIM_LOG_SPEC_VIOLATION(1, device_, Log_TLM,
                               "PciBus::write(): GP command set to read,"
                               " expected write");
        payload_->set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
        return types::PCI_BUS_GP_ERROR;
    }

    // for comments, see read()
    addr_space_t pci_space = static_cast<addr_space_t>(space);
    bytes_t bytes;
    bytes.data = payload_->get_data_ptr();
    bytes.len = payload_->get_data_length();
    exception_type_t ex = upstream_provider_.get_interface<
        pci_upstream_operation_interface_t>()->write(
            target().object(), device_,
            rid, pci_space, payload_->get_address(), bytes);

    if (log_pci_bus_exception(ex, device_)) {
        payload_->set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
    }

    return static_cast<types::pci_bus_exception_type_t>(ex);
}

iface::ReceiverInterface *PciBus::receiver() {
    return &dispatcher_;
}

tlm::tlm_response_status PciBus::simics_transaction(
        simics::ConfObjectRef &simics_obj,
        tlm::tlm_generic_payload *trans) {
    if (!device_)
        device_ = simics_obj.object();

    payload_ = trans;
    if (dispatcher_.handle(trans))
        return tlm::TLM_OK_RESPONSE;

    SIM_LOG_SPEC_VIOLATION(1, simics_obj, 0,
        "Expected PciBusExtension or PciUpstreamOperationExtension");
    return tlm::TLM_COMMAND_ERROR_RESPONSE;
}

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics
