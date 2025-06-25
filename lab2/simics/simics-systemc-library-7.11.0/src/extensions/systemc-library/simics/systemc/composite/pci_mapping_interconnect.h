/*                            -*- mode: C++; c-file-style: "virtutech-c++" -*-

  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_COMPOSITE_PCI_MAPPING_INTERCONNECT_H
#define SIMICS_SYSTEMC_COMPOSITE_PCI_MAPPING_INTERCONNECT_H

#if defined SIMICS_5_API || defined SIMICS_6_API

#include <systemc>
#include <tlm>
#include <tlm_utils/multi_passthrough_target_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>

#include <simics/systemc/device.h>
#include <simics/systemc/adapter_log_groups.h>
#include <simics/systemc/iface/extension_sender.h>
#include <simics/systemc/iface/map_info_extension.h>
#include <simics/systemc/iface/pci_bus_extension.h>
#include <simics/systemc/iface/pci_device_query_interface.h>

#include <simics/systemc/simics2tlm/gasket_factory.h>
#include <simics/systemc/simics2tlm/io_memory.h>
#include <simics/systemc/simics2tlm/pci_device.h>
#include <simics/systemc/simics2tlm/pci_express.h>

#include <vector>

namespace {  // NOLINT(build/namespaces_headers)
const char *const PCI_MAPPING_INTERCONNECT_TAG = "intel/PciMappingInterconnect";
}  // namespace

namespace simics {
namespace systemc {
namespace composite {

/** \internal
 */
template<typename TSocket>
class PciMappingInterconnectExtensionSender
    : public iface::ExtensionSender<TSocket> {
  public:
    virtual ~PciMappingInterconnectExtensionSender() {}
    virtual void send_extension(iface::Transaction *transaction) {
        failed_transaction_ = iface::Transaction(NULL);
        iface::ExtensionSender<TSocket>::send_extension(transaction);
    }
    virtual void send_failed(iface::Transaction *transaction) {
        failed_transaction_ = *transaction;
        iface::ExtensionSender<TSocket>::send_failed(transaction);
    }
    iface::Transaction failed_transaction_;
};

/** \internal
 * The purpose of this class is to intercept _writes_ to the Configuration
 * Space BAR and Command registers to do the Simics specific mappings before
 * passing the transaction on to the actual target socket of the PCI device,
 * thus hiding some Simics specific knowledge from the end user.
 *
 * Methods intercepted:
 * - b_transport is always passed on, but will also trigger side-effects in the
 *   IC unless this has been disabled.
 * - transport_dbg is always pass-through (as this should not have any
 *   side-effects, this class is not allowed to make any either)
 * - nb_transport_fw is handled by tlm_utils::simple_target_socket as Simics
 *   does not have any asynchronous interfaces
 * - get_direct_mem_ptr is handled by tlm_utils::simple_target_socket on the
 *   Simics side, as Simics does not support DMI. It is passed through from the
 *   Device side, as pci-bus does support it.
 * - invalidate_direct_mem_ptr is pass-through from Simics to Device
 */
template <unsigned int BUSWIDTH, typename TYPES>
class PciMappingInterconnect : public sc_core::sc_module {
  private:
    // Socket types used by the interconnect
    typedef tlm_utils::simple_target_socket<
        PciMappingInterconnect, BUSWIDTH, TYPES> target_socket_t;
    typedef tlm_utils::multi_passthrough_target_socket<
        PciMappingInterconnect, BUSWIDTH, TYPES> multi_target_socket_t;
    typedef tlm_utils::simple_initiator_socket<
        PciMappingInterconnect, BUSWIDTH, TYPES> initiator_socket_t;
    typedef struct {
        tlm::tlm_target_socket<BUSWIDTH, TYPES> *tlm;
        tlm_utils::multi_target_base<BUSWIDTH, TYPES> *multi;
    } supported_target_socket_t;

  public:
#ifndef SYSTEMC_3_0_0
    SC_HAS_PROCESS(PciMappingInterconnect);
#endif
    explicit PciMappingInterconnect(
            sc_core::sc_module_name = "PciMappingInterconnect")
        : target_socket("target_socket"),
          simics_target_socket("simics_target_socket"),
          pci_bus_initiator_socket("pci_bus_initiator_socket"),
          initiator_sockets_("initiator_socket"),
          pci_target_socket_(),
          pci_device_target_socket_(),
          pci_bus_target_socket_("pci_bus_target_socket"),
          number_of_functions_(1),
          mapping_id_offset_(0x10100) {  // 255 + 64k (max VFs) + 1 (round up)
        target_socket.register_b_transport(
            this, &PciMappingInterconnect::b_transport);
        target_socket.register_transport_dbg(
            this, &PciMappingInterconnect::transport_dbg);
        simics_target_socket.register_b_transport(
            this, &PciMappingInterconnect::simics_b_transport);
        pci_bus_target_socket_.register_b_transport(
            this, &PciMappingInterconnect::pci_bus_b_transport);
        pci_bus_target_socket_.register_transport_dbg(
            this, &PciMappingInterconnect::pci_bus_transport_dbg);
        pci_bus_target_socket_.register_get_direct_mem_ptr(
            this, &PciMappingInterconnect::pci_bus_get_direct_mem_ptr);
        pci_bus_initiator_socket.register_invalidate_direct_mem_ptr(
            this, &PciMappingInterconnect::pci_bus_invalidate_direct_mem_ptr);
        sender_.init(&pci_bus_initiator_socket);
        extension_.init(&sender_);
    }

    virtual void before_end_of_elaboration();

    void retrieveFunctionData(int id, int bar);

    void connect(iface::PciDeviceQueryInterface *pci, ConfObjectRef o);

    void connectConfig(iface::BaseAddressRegisterQueryInterface *bar,
                       simics2tlm::IoMemory *gasket,
                       ConfObjectRef o);

    void connectMmio(iface::BaseAddressRegisterQueryInterface *bar,
                     simics2tlm::IoMemory *gasket,
                     ConfObjectRef o);

    void busReset();

    // Sockets bound by external logic (see also private scope):
    /// Simics -> IC, intercepts the incoming PCI transactions
    multi_target_socket_t target_socket;
    multi_target_socket_t simics_target_socket;
    /// IC -> Simics, forwards the upstream pci-bus transactions
    initiator_socket_t pci_bus_initiator_socket;

  private:
    // TLM2 methods
    void b_transport(int id, tlm::tlm_generic_payload &trans,  // NOLINT: SystemC API
                     sc_core::sc_time &t);  // NOLINT: SystemC API
    unsigned int transport_dbg(int id, tlm::tlm_generic_payload &trans);  // NOLINT: SystemC API
    void simics_b_transport(int id, tlm::tlm_generic_payload &trans,  // NOLINT: SystemC API
                            sc_core::sc_time &t);  // NOLINT: SystemC API
    void pci_bus_b_transport(tlm::tlm_generic_payload &trans,  // NOLINT: SystemC API
                             sc_core::sc_time &t);  // NOLINT: SystemC API
    unsigned int pci_bus_transport_dbg(tlm::tlm_generic_payload &trans);  // NOLINT: SystemC API
    bool pci_bus_get_direct_mem_ptr(tlm::tlm_generic_payload &trans,  // NOLINT: SystemC API
                                    tlm::tlm_dmi &dmi_data);  // NOLINT: SystemC API
    void pci_bus_invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                                           sc_dt::uint64 end_range);

    // Helper methods
    void castToTarget(sc_core::sc_object *object,
                      supported_target_socket_t *target);
    template <typename TGasket>
    void bindToGasket(supported_target_socket_t target,
                      TGasket *gasket, ConfObjectRef o, int id);
    void updateCommand(int id, uint16_t old_command);
    void updateBaseAddress(int id, int bar);
    void updateMemoryMapping(bool enable, types::addr_space_t space,
                             uint64_t base_address, int bar_size_bits,
                             int mapping_id);
    void interceptBarAccess(int id, tlm::tlm_generic_payload &trans,  // NOLINT
                            sc_core::sc_time &t);  // NOLINT: SystemC API

    // Sockets created and bound by internal logic (see also public scope):
    // IC -> Device, forwards the incoming PCI transactions
    sc_core::sc_vector<initiator_socket_t> initiator_sockets_;
    supported_target_socket_t pci_target_socket_;
    initiator_socket_t pci_device_initiator_socket_;
    supported_target_socket_t pci_device_target_socket_;
    // Device -> IC, intercept upstream pci-bus transactions
    target_socket_t pci_bus_target_socket_;

    struct function_data_t {
        int function;
        uint16_t command;
        uint32_t base_address[6];
    };
    std::vector<function_data_t> function_data_;
    int number_of_functions_;
    int mapping_id_offset_;
    iface::BaseAddressRegisterQueryInterface::BarInfo bar_info_;

    PciMappingInterconnectExtensionSender<
        initiator_socket_t> sender_;
    iface::PciBusExtension extension_;
};

template <unsigned int BUSWIDTH, typename TYPES>
void PciMappingInterconnect<BUSWIDTH, TYPES>::before_end_of_elaboration() {
    // Because of the way SystemC has been designed, we cannot create new
    // sockets dynamically ouside of the CTOR except for in the
    // before_end_of_elaboration() method.

    // Create initiator sockets equal to the number of PCI functions
    initiator_sockets_.init(number_of_functions_);
    for (std::size_t i = 0; i < initiator_sockets_.size(); ++i) {
        if (pci_target_socket_.tlm) {
            initiator_sockets_[i].bind(*pci_target_socket_.tlm);
        } else {
            initiator_sockets_[i].bind(*pci_target_socket_.multi);
        }
    }

    // Allocate and initialize function data based on device configuration
    function_data_ = std::vector<function_data_t>(number_of_functions_);
    int prev_function = -1;
    int id = -1;
    for (iface::BaseAddressRegisterQueryInterface::BarInfo::const_iterator it
             = bar_info_.begin(); it != bar_info_.end(); ++it) {
        int current_function = (*it).function;
        if (current_function != prev_function) {
            function_data_[++id].function = current_function;
            prev_function = current_function;
        }
    }

    if (pci_device_target_socket_.tlm) {
        pci_device_initiator_socket_.bind(*pci_device_target_socket_.tlm);
    } else if (pci_device_target_socket_.multi) {
        pci_device_initiator_socket_.bind(*pci_device_target_socket_.multi);
    }
}

// Get up-to-date BAR/CMD information whenever we need it. It cannot be cached
// in the interconnect module as it might change over time by the model,
// reverse execution, checkpoint restore, ...
// @id is the function ID (socket ID) currently accessed
// @bar is the BAR number currently access, or -1 for config register access in
// which case all BARs must be retrieved
template <unsigned int BUSWIDTH, typename TYPES>
void PciMappingInterconnect<BUSWIDTH, TYPES>::retrieveFunctionData(
        int id, int bar) {
    tlm::tlm_generic_payload trans;
    trans.set_command(tlm::TLM_READ_COMMAND);
    trans.set_data_length(4);
    trans.set_streaming_width(4);

    uint32_t value = 0;
    trans.set_data_ptr(reinterpret_cast<unsigned char *>(&value));
    trans.set_address(0x4);
    initiator_sockets_[id]->transport_dbg(trans);
    function_data_[id].command = value;

    // -1 means config register access => read all bars
    bool all_bars = (bar == -1) ? true : false;

    // NOTE: the IO, Mem32, Mem64 bits are ignored by updateMemoryMapping, no
    // need to filter them out here
    int function = function_data_[id].function;
    for (iface::BaseAddressRegisterQueryInterface::BarInfo::const_iterator
             it = bar_info_.begin(); it != bar_info_.end(); ++it) {
        // BarInfo holds _all_ BARs for _all_ functions. This can probably
        // be done in a much more clever way that avoids iterating over the
        // entire BarInfo for each function in the outer loop.
        if ((*it).function != function) {
            continue;
        }
        if (all_bars) {
            bar = ((*it).offset - 0x10) / 4;
        } else if (bar != ((*it).offset - 0x10) / 4) {
            continue;
        }
        uint32_t base_address = 0;
        trans.set_address((*it).offset);
        trans.set_data_ptr(
            reinterpret_cast<unsigned char *>(&base_address));
        trans.set_data_length(sizeof base_address);
        trans.set_streaming_width(trans.get_data_length());
        initiator_sockets_[id]->transport_dbg(trans);
        function_data_[id].base_address[bar] = base_address;
        if ((*it).is_64bit) {
            // Read higher 32 bits in next BAR
            trans.set_address((*it).offset + 4);
            trans.set_data_ptr(
                reinterpret_cast<unsigned char *>(&base_address));
            trans.set_data_length(sizeof base_address);
            trans.set_streaming_width(trans.get_data_length());
            initiator_sockets_[id]->transport_dbg(trans);
            function_data_[id].base_address[bar + 1] = base_address;
        }
    }
}

/**
 * Called by (outer) composite class to connect interconnect module
 * to the PCI device module.
 *
 * NOTE: the binding of pci_target_socket_ is done in
 * before_end_of_elaboration() because we cannot create the initiator sockets
 * outside of the CTOR.
 */
template <unsigned int BUSWIDTH, typename TYPES>
void PciMappingInterconnect<BUSWIDTH, TYPES>::connect(
        iface::PciDeviceQueryInterface *pci, ConfObjectRef o) {
    castToTarget(pci->getPciDeviceTargetSocket(),
                             &pci_device_target_socket_);
    castToTarget(pci->getConfigTargetSocket(), &pci_target_socket_);
    tlm::tlm_initiator_socket<BUSWIDTH, TYPES> *initiator
        = dynamic_cast<tlm::tlm_initiator_socket<BUSWIDTH, TYPES> *>(
            pci->getPciBusInitiatorSocket());
    if (initiator) {
        pci_bus_target_socket_.bind(*initiator);
    } else {
        SIM_LOG_ERROR(o, Log_Configuration,
                      "object returned by getPciBusInitiatorSocket was not"
                      " a tlm::tlm_initiator_socket.");
    }
}

/**
 * Called by (outer) composite class to connect the config space of each
 * function within the PCI(e) device to the IC where transactions are snooped
 * before they are forwarded to the device. Connecting IC to the device is done
 * at before_end_of_elaboration().
 */
template <unsigned int BUSWIDTH, typename TYPES>
void PciMappingInterconnect<BUSWIDTH, TYPES>::connectConfig(
        iface::BaseAddressRegisterQueryInterface *bar,
        simics2tlm::IoMemory *gasket, ConfObjectRef o) {
    // F0 must always exist
    // NOTE: by Simics convention, F0 always get mapping ID = 255
    gasket->addGasket(
        255, simics2tlm::createGasket(&target_socket, o));

    // How many more functions are there?
    bar_info_ = bar->getBarInfo();
    int prev_function = 0;
    for (iface::BaseAddressRegisterQueryInterface::BarInfo::const_iterator it
             = bar_info_.begin(); it != bar_info_.end(); ++it) {
        int current_function = (*it).function;
        if (current_function != prev_function) {
            ++number_of_functions_;
            prev_function = current_function;
            gasket->addGasket(255 + current_function,
                              simics2tlm::createGasket(&target_socket, o));
        }
        FATAL_ERROR_IF(current_function < prev_function,
                       "BarInfo must be sorted on function number"
                       " in ascending order");
    }
}

/**
 * Called by (outer) composite class to connect io/mem spaces of the PCI device
 * directly to the io_memory gasket. I.e. these transactions go straight
 * through and is not intercepted by the IC.
 */
template <unsigned int BUSWIDTH, typename TYPES>
void PciMappingInterconnect<BUSWIDTH, TYPES>::connectMmio(
        iface::BaseAddressRegisterQueryInterface *bar,
        simics2tlm::IoMemory *gasket, ConfObjectRef o) {
    iface::BaseAddressRegisterQueryInterface::BarSockets bar_sockets
        = bar->getBarTargetSockets();
    iface::BaseAddressRegisterQueryInterface::BarSockets::const_iterator it;
    for (iface::BaseAddressRegisterQueryInterface::BarSockets::const_iterator it
             = bar_sockets.begin(); it != bar_sockets.end(); ++it) {
        supported_target_socket_t target = {};
        castToTarget((*it).second, &target);
        bindToGasket(target, gasket, o,
                     (*it).first.mapping_id + mapping_id_offset_);
    }
}

/// Called by (outer) composite class to handle pci_bus::bus_reset()
template <unsigned int BUSWIDTH, typename TYPES>
void PciMappingInterconnect<BUSWIDTH, TYPES>::busReset() {
    for (iface::BaseAddressRegisterQueryInterface::BarInfo::const_iterator it
             = bar_info_.begin(); it != bar_info_.end(); ++it) {
        int mapping_id = (*it).mapping_id + mapping_id_offset_;
        types::addr_space_t space = types::Sim_Addr_Space_IO;
        if ((*it).is_memory) {
            space = types::Sim_Addr_Space_Memory;
        }
        extension_.remove_map(space, mapping_id);
        if (sender_.failed_transaction_.payload()) {
            SC_REPORT_INFO(PCI_MAPPING_INTERCONNECT_TAG,
                           "Failure to unmap device on bus_reset");
        }
    }
}

////////// PRIVATE BELOW THIS LINE //////////////////

/*
 * Intercept the BAR and Command regs, always forward transaction to target to
 * handle all errors there
 */
template <unsigned int BUSWIDTH, typename TYPES>
void PciMappingInterconnect<BUSWIDTH, TYPES>::b_transport(
        int id,
        tlm::tlm_generic_payload &trans,  // NOLINT
        sc_core::sc_time &t) {  // NOLINT
    interceptBarAccess(id, trans, t);
    initiator_sockets_[id]->b_transport(trans, t);
}

/*
 * transport_dbg is side-effect free = forwarded without inspection
 */
template <unsigned int BUSWIDTH, typename TYPES>
unsigned int PciMappingInterconnect<BUSWIDTH, TYPES>::transport_dbg(
        int id,
        tlm::tlm_generic_payload &trans) {  // NOLINT
    return initiator_sockets_[id]->transport_dbg(trans);
}

/*
 * For pci-device, just relay the incoming transaction to outgoing socket
 */
template <unsigned int BUSWIDTH, typename TYPES>
void PciMappingInterconnect<BUSWIDTH, TYPES>::simics_b_transport(
        int id, tlm::tlm_generic_payload &trans,  // NOLINT
        sc_core::sc_time &t) {  // NOLINT
    pci_device_initiator_socket_->b_transport(trans, t);
}

/*
 * For pci-bus, just relay the incoming transaction to outgoing socket
 */
template <unsigned int BUSWIDTH, typename TYPES>
void PciMappingInterconnect<BUSWIDTH, TYPES>::pci_bus_b_transport(
        tlm::tlm_generic_payload &trans,  // NOLINT
        sc_core::sc_time &t) {  // NOLINT
    pci_bus_initiator_socket->b_transport(trans, t);
}

/*
 * For pci-bus, just relay the incoming transaction to outgoing socket
 */
template <unsigned int BUSWIDTH, typename TYPES>
unsigned int PciMappingInterconnect<BUSWIDTH, TYPES>::pci_bus_transport_dbg(
        tlm::tlm_generic_payload &trans) {  // NOLINT
    return pci_bus_initiator_socket->transport_dbg(trans);
}

/*
 * For pci-bus, just relay the incoming transaction to outgoing socket
 */
template <unsigned int BUSWIDTH, typename TYPES>
bool PciMappingInterconnect<BUSWIDTH, TYPES>::pci_bus_get_direct_mem_ptr(
        tlm::tlm_generic_payload &trans,  // NOLINT
        tlm::tlm_dmi &dmi_data) {  // NOLINT
    return pci_bus_initiator_socket->get_direct_mem_ptr(trans, dmi_data);
}

/*
 * For pci-bus, just relay the incoming BW transaction to target socket
 */
template <unsigned int BUSWIDTH, typename TYPES>
void PciMappingInterconnect<BUSWIDTH, TYPES>::pci_bus_invalidate_direct_mem_ptr(
        sc_dt::uint64 start_range,
        sc_dt::uint64 end_range) {
    pci_bus_target_socket_->invalidate_direct_mem_ptr(start_range,
                                                     end_range);
}

// Cast sc_object to a supported target socket and return it.
// TPciDevice is the type of the PCI device,
template <unsigned int BUSWIDTH, typename TYPES>
void PciMappingInterconnect<BUSWIDTH, TYPES>::castToTarget(
        sc_core::sc_object *object, supported_target_socket_t *target) {
    tlm::tlm_target_socket<BUSWIDTH, TYPES> *tlm_target
        = dynamic_cast<tlm::tlm_target_socket<BUSWIDTH, TYPES> *>(object);
    if (tlm_target) {
        target->tlm = tlm_target;
        return;
    }

    tlm_utils::multi_target_base<BUSWIDTH, TYPES> *multi_target
        = dynamic_cast<tlm_utils::multi_target_base<BUSWIDTH, TYPES> *>(object);
    if (multi_target) {
        target->multi = multi_target;
        return;
    }

    SC_REPORT_ERROR(PCI_MAPPING_INTERCONNECT_TAG,
                    "Unable to dynamic-cast PCI target socket");
}

template <unsigned int BUSWIDTH, typename TYPES>
template <typename TGasket>
void PciMappingInterconnect<BUSWIDTH, TYPES>::bindToGasket(
        supported_target_socket_t target,
        TGasket *gasket, ConfObjectRef o, int id) {
    if (target.tlm) {
        gasket->addGasket(id, simics2tlm::createGasket(target.tlm, o));
    } else if (target.multi) {
        // coverity[var_deref_model], safe cast based on castToTarget() func
        gasket->addGasket(id, simics2tlm::createGasket(target.multi, o));
    }
}

/*
 * When command register is written to, the mappings need to be re-evaluated
 * and possibly updated.
 *
 * There is no error handling, so if the mapping fails this will only be logged
 * but the access will still pass (i.e. transaction forwarded to target device
 * where registers are updated without side-effects)
 */
template <unsigned int BUSWIDTH, typename TYPES>
void PciMappingInterconnect<BUSWIDTH, TYPES>::updateCommand(
        int id, uint16_t old_command) {
    int command = function_data_[id].command;
    if (command == old_command) {
        return;
    }
    int function = function_data_[id].function;
    bool mem_enable = (command >> 1) & 1;
    bool io_enable = command & 1;

    for (iface::BaseAddressRegisterQueryInterface::BarInfo::const_iterator it
             = bar_info_.begin(); it != bar_info_.end(); ++it) {
        if ((*it).function == function) {
            bool enable
                = (*it).is_memory ? mem_enable : io_enable;
            types::addr_space_t space
                = (*it).is_memory ? types::Sim_Addr_Space_Memory
                                  : types::Sim_Addr_Space_IO;
            int bar = ((*it).offset - 0x10) / 4;
            uint64_t base_address = function_data_[id].base_address[bar];
            if ((*it).is_64bit) {
                base_address |= static_cast<uint64_t>(
                    function_data_[id].base_address[bar + 1]) << 32;
            }
            updateMemoryMapping(enable, space, base_address,
                                (*it).size_bits,
                                (*it).mapping_id + mapping_id_offset_);
        }
    }
}

/*
 * When BAR register is written to, the mapping need to be re-evaluated and
 * possibly updated.
 *
 * There is no error handling, so if the mapping fails this will only be logged
 * but the access will still pass (i.e. transaction forwarded to target device
 * where registers are updated without side-effects)
 */
template <unsigned int BUSWIDTH, typename TYPES>
void PciMappingInterconnect<BUSWIDTH, TYPES>::updateBaseAddress(int id,
                                                                int bar) {
    int command = function_data_[id].command;
    int function = function_data_[id].function;
    bool mem_enable = (command >> 1) & 1;
    bool io_enable = command & 1;
    for (iface::BaseAddressRegisterQueryInterface::BarInfo::const_iterator it
             = bar_info_.begin(); it != bar_info_.end(); ++it) {
        if ((*it).function == function && (*it).offset == 0x10 + 4 * bar) {
            bool enable = (*it).is_memory ? mem_enable : io_enable;
            if (!enable) {
                // no re-mapping will be done as encoding is not enabled
                return;
            }
            types::addr_space_t space
                = (*it).is_memory ? types::Sim_Addr_Space_Memory
                                  : types::Sim_Addr_Space_IO;
            uint64_t base_address = function_data_[id].base_address[bar];
            if ((*it).is_64bit) {
                base_address |= static_cast<uint64_t>(
                    function_data_[id].base_address[bar + 1]) << 32;
            }
            updateMemoryMapping(enable, space, base_address,
                                (*it).size_bits,
                                (*it).mapping_id + mapping_id_offset_);
            break;
        }
    }
}

template <unsigned int BUSWIDTH, typename TYPES>
void PciMappingInterconnect<BUSWIDTH, TYPES>::updateMemoryMapping(
        bool enable,
        types::addr_space_t space,
        uint64_t base_address,
        int bar_size_bits,
        int mapping_id) {
    extension_.remove_map(space, mapping_id);
    if (sender_.failed_transaction_.payload()) {
        SC_REPORT_INFO(PCI_MAPPING_INTERCONNECT_TAG, "Failure to unmap device");
        return;
    }

    if (enable) {
        types::map_info_t info(base_address & ~((1 << bar_size_bits) - 1),
                               0, 1 << bar_size_bits,
                               mapping_id);
        extension_.add_map(space, info);

        if (sender_.failed_transaction_.payload()) {
            SC_REPORT_INFO(PCI_MAPPING_INTERCONNECT_TAG,
                           "Failure to map device");
            return;
        }
    }
}

template <unsigned int BUSWIDTH, typename TYPES>
void PciMappingInterconnect<BUSWIDTH, TYPES>::interceptBarAccess(
        int id,
        tlm::tlm_generic_payload &trans,  // NOLINT
        sc_core::sc_time &t) {  // NOLINT
    // Decode the generic payload (GP) struct
    sc_dt::uint64 offset = trans.get_address();
    unsigned int size = trans.get_data_length();
    uint64_t *data = reinterpret_cast<uint64_t *>(trans.get_data_ptr());

    // Check assumptions (but leave it up to target device to report errors)
    if (trans.get_byte_enable_ptr()) {
        return;
    }
    if (trans.get_streaming_width() != size) {
        return;
    }
    // NOTE: we should also check that address is DWORD/4-byte aligned, but
    //       our Viper/X58 platform seems to address single bytes so ...

    // We are only interested in write access to BARn or Command regs
    if (trans.is_write()) {
        uint16_t old_command = function_data_[id].command;

        switch (offset) {
        case 0x04: {  // command & status
            retrieveFunctionData(id, -1);
            function_data_[id].command = (*data) & 0xffff;  // ignore status
            updateCommand(id, old_command);
            break;
        }
        case 0x10:  // BAR0-BAR5
        case 0x14:
        case 0x18:
        case 0x1c:
        case 0x20:
        case 0x24:
            int bar = (offset - 0x10) / 4;
            retrieveFunctionData(id, bar);
            // NOTE: the IO, Mem32, Mem64 bits are ignored by
            // updateMemoryMapping, no need to filter them out here. In
            // addition, any bits less than the size of the BAR as defined by
            // the BarInfo will also be ignored/truncated by
            // updateMemoryMapping; making sure we always send a naturally
            // aligned address to Simics
            function_data_[id].base_address[bar] = *data & 0xffffffff;
            if (size == 8 && !(offset % 8)) {
                function_data_[id].base_address[bar + 1] = *data >> 32;
            }
            updateBaseAddress(id, bar);
            break;
        }
    }
}

}  // namespace composite
}  // namespace systemc
}  // namespace simics

#endif
#endif
