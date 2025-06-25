/*                                                              -*- C++ -*-

  © 2024 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_COMPOSITE_PCIE_MAPPING_INTERCONNECT_H
#define SIMICS_SYSTEMC_COMPOSITE_PCIE_MAPPING_INTERCONNECT_H

#include <systemc>
#include <tlm>
#include <tlm_utils/multi_socket_bases.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

#include <simics/cc-api.h>
#include <simics/systemc/adapter_log_groups.h>
#include <simics/systemc/pcie_tlm_extension.h>
#include <simics/systemc/iface/extension_sender.h>
#include <simics/systemc/iface/pcie_map_extension.h>
#include <simics/systemc/iface/pcie_map_interface.h>
#include <simics/systemc/iface/pcie_device_query_interface.h>

#include <sstream>
#include <map>
#include <vector>
#include <stdexcept>
#include <string>
#include <set>
#include <utility>  // pair

namespace {  // NOLINT(build/namespaces_headers)
const char *const PCIE_MAPPING_INTERCONNECT_TAG = \
    "intel/PcieMappingInterconnect";

/// Return the string type name for a PCIe type
const char *pcieTypeName(simics::types::pcie_type_t t) {
    switch (t) {
    case simics::types::PCIE_Type_Not_Set:
        return "Not Set";
    case simics::types::PCIE_Type_Mem:
        return "Memory";
    case simics::types::PCIE_Type_IO:
        return "I/O";
    case simics::types::PCIE_Type_Cfg:
        return "Config";
    case simics::types::PCIE_Type_Msg:
        return "Message";
    case simics::types::PCIE_Type_Other:
        return "Other";
    default:
        return "Unknown";
    }
}

std::string pcieDeviceIdStr(uint16_t device_id) {
    std::ostringstream os;
    os << "device id 0x" << std::hex << device_id
       << " (" << std::dec << (device_id >> 8)
       << ":" << ((device_id >> 3) & 0x1f)
       << "." << (device_id & 7) << ")";
    return os.str();
}
}  // namespace

namespace simics {
namespace systemc {
namespace composite {

/** \internal
 */
template<typename TSocket>
class PcieMappingInterconnectExtensionSender
    : public iface::ExtensionSender<TSocket> {
  public:
    virtual ~PcieMappingInterconnectExtensionSender() {}

    // iface::ExtensionSender
    void send_extension(iface::Transaction *transaction) override {
        failed_transaction_ = iface::Transaction(nullptr);
        iface::ExtensionSender<TSocket>::send_extension(transaction);
    }
    void send_failed(iface::Transaction *transaction) override {
        failed_transaction_ = *transaction;
        iface::ExtensionSender<TSocket>::send_failed(transaction);
    }

    iface::Transaction failed_transaction_;
};

/** \internal
 * The purpose of this class is to intercept _writes_ to the Configuration
 * Space BAR and Command registers to do the Simics specific mappings before
 * passing the transaction on to the actual target socket of the PCIe device,
 * thus hiding some Simics specific knowledge from the end user.
 */
template <unsigned int BUSWIDTH, typename TYPES>
class PcieMappingInterconnect : public sc_core::sc_module {
    // Socket types used by the interconnect
    typedef tlm_utils::simple_target_socket<
        PcieMappingInterconnect, BUSWIDTH, TYPES> target_socket_t;
    typedef tlm_utils::simple_initiator_socket<
        PcieMappingInterconnect, BUSWIDTH, TYPES> initiator_socket_t;
    typedef struct {
        tlm::tlm_target_socket<BUSWIDTH, TYPES> *tlm;
        tlm_utils::multi_target_base<BUSWIDTH, TYPES> *multi;
    } supported_target_socket_t;

  public:
#ifndef SYSTEMC_3_0_0
    SC_HAS_PROCESS(PcieMappingInterconnect);
#endif
    explicit PcieMappingInterconnect(
        sc_core::sc_module_name = "PcieMappingInterconnect")
        : transaction_target_socket("transaction_target_socket"),
          pcie_device_target_socket("dummy_pcie_device_target_socket"),
          pcie_map_initiator_socket("pcie_map_initiator_socket"),
          initiator_sockets_("initiator_socket"),
          config_target_socket_(),
          msg_target_socket_(),
          pcie_map_target_socket_("pcie_map_target_socket") {
        transaction_target_socket.register_b_transport(
            this, &PcieMappingInterconnect::transaction_b_transport);
        transaction_target_socket.register_transport_dbg(
            this, &PcieMappingInterconnect::transaction_transport_dbg);
        pcie_map_initiator_socket.register_invalidate_direct_mem_ptr(
            this, &PcieMappingInterconnect::pcie_map_invalidate_direct_mem_ptr);
        pcie_map_target_socket_.register_b_transport(
            this, &PcieMappingInterconnect::pcie_map_b_transport);
        pcie_map_target_socket_.register_transport_dbg(
            this, &PcieMappingInterconnect::pcie_map_transport_dbg);
        pcie_map_target_socket_.register_get_direct_mem_ptr(
            this, &PcieMappingInterconnect::pcie_map_get_direct_mem_ptr);

        sender_.init(&pcie_map_initiator_socket);
        pcie_map_extension_.init(&sender_);

        // f0 is required
        function_data_[0];

        SC_METHOD(warmReset);
        sensitive << warm_reset_pin.pos();
        dont_initialize();

        config_target_socket_.tlm = nullptr;
        config_target_socket_.multi = nullptr;
        msg_target_socket_.tlm = nullptr;
        msg_target_socket_.multi = nullptr;
    }

    // sc_core::sc_module
    void before_end_of_elaboration() override;

    // Called by (outer) composite class from PcieDevice::connected
    void connected(uint16_t device_id);

    // Called by (outer) composite class from PcieDevice::disconnected
    void disconnected(uint16_t device_id);

    // Called by (outer) composite class from PcieDevice::hot_reset
    void hotReset();

    /**
     * Called by (outer) composite class to retrieve the required information
     * to connect the IC with the PCI(e) device. The actual connecting/binding
     * of the IC to the device is done at before_end_of_elaboration().
     */
    void connect(iface::PcieDeviceQueryInterface *pci,
                 iface::PcieBaseAddressRegisterQueryInterface *bar,
                 iface::PcieResetInterface *reset,
                 ConfObjectRef o);

    // Create map helpers which are mapped on the cfg space. It contains
    // PCI information like PCIe type and function id which is used to
    // route the transaction to the right TLM2 socket
    void createCfgMapHelper();

    // Return the current map from address range to socket ID (Memory)
    std::map<std::pair<size_t, size_t>, size_t> addressIdMemMap() const;

    // Return the current map from address range to socket ID (IO)
    std::map<std::pair<size_t, size_t>, size_t> addressIdIoMap() const;

    // Sockets bound by external logic (see also private scope):
    // Simics -> IC, intercepts the incoming PCIe transactions
    target_socket_t transaction_target_socket;
    // Dummy socket as PcieDevice is handled by connected/disconnected/hotReset
    target_socket_t pcie_device_target_socket;
    /// IC -> Simics, forwards the upstream pcie-map transactions
    initiator_socket_t pcie_map_initiator_socket;
    // Simics -> IC, receives warm reset signal
    sc_core::sc_in<bool> warm_reset_pin;
    // For MEM/IO, this parameter controls if the base address of the
    // memory range is subtracted before sending the GP payload to endpoint
    bool enable_base_address_subtraction {true};

  private:
    // Called when warm_reset_pin is positive
    void warmReset();

    // Help methods
    // Adds the BAR mappings of type with map info
    void addMap(types::map_info_t info, types::pcie_type_t type);

    // Removes the BAR mappings of type for socket ID
    void delMap(size_t socket_id, types::pcie_type_t type);

    // The supplied device_id passed to addFunction & delFunction should
    // contain the full function ID and not just the function number.
    // Adds the function's configuration header in the upstream target
    void addFunction(uint16_t device_id);
    // Removes the current mapping (if any) of this function's configuration
    // header in the upstream target
    void delFunction(uint16_t device_id);

    // Cast sc_object to a supported target socket
    void castToTarget(sc_core::sc_object *object,
                      supported_target_socket_t *target);

    // Update the PCIe mapping
    // @param function_id the config id used to index into function_data_,
    //                    -1 means checks all config ids
    // @param bar_id the bar id used to index into base_address,
    //               -1 means checks all bar ids
    void updateMappings(int function_id = -1, int bar_id = -1);

    /*
     * When command register is written to, the mappings need to be re-evaluated
     * and possibly updated.
     *
     * There is no error handling, so if the mapping fails this will only be
     * logged but the access will still pass (i.e. transaction forwarded to
     * target device where registers are updated without side-effects)
     */
    void updateCommand(uint16_t function_id, uint16_t old_command);

    // Get up-to-date BAR/CMD information whenever we need it. It cannot be
    // cached in the interconnect module as it might change over time by the
    // model, reverse execution, checkpoint restore, ...
    // @param id is the accessing initiator socket ID
    // @param bar_id is the BAR number currently access, or -1 for config
    //               register access in which case all BARs must be retrieved
    void retrieveFunctionData(size_t socket_id, int bar_id);

    // Intercept Config space header write access and update the mapping table
    void interceptCfgHeaderAccess(size_t socket_id,
                                  tlm::tlm_generic_payload &trans);  // NOLINT

    // Find the id of the initiator_sockets_ list to route the transaction to
    // based on the PCIe transaction type, function number and offset
    size_t findSocketId(types::pcie_type_t type, uint16_t function_id,
                        tlm::tlm_generic_payload &trans) const;  // NOLINT

    // Return the created cfg map helper by device_id
    conf_object_t *getCfgMapHelper(uint16_t device_id);

    // Validate and update TLM2 response status
    // If it passes validation it returns the socket-id as well
    size_t validateTransaction(tlm::tlm_generic_payload &trans);  // NOLINT
    /*
     * The single entry for all incoming PCIe transactions from Simics
     * Depends on the PCIe type (CFG/MEM/IO/MSG), the transaction is
     * routed to the corresponding initiator socket towards the endpoint device
     * For CFG space header write, updating the mapping table as well
     */
    void transaction_b_transport(tlm::tlm_generic_payload &trans,  // NOLINT
                                 sc_core::sc_time &t);  // NOLINT
    unsigned int transaction_transport_dbg(
        tlm::tlm_generic_payload &trans);  // NOLINT

    void pcie_map_invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                                            sc_dt::uint64 end_range);
    void pcie_map_b_transport(
        tlm::tlm_generic_payload &trans,  // NOLINT: SystemC API
        sc_core::sc_time &t);  // NOLINT: SystemC API
    unsigned int pcie_map_transport_dbg(
        tlm::tlm_generic_payload &trans);  // NOLINT: SystemC API
    bool pcie_map_get_direct_mem_ptr(
        tlm::tlm_generic_payload &trans,  // NOLINT: SystemC API
        tlm::tlm_dmi &dmi_data);  // NOLINT: SystemC API

    // Sockets created and bound by internal logic (see also public scope):
    // IC -> Device, forwards the incoming PCIe transactions
    sc_core::sc_vector<initiator_socket_t> initiator_sockets_;
    supported_target_socket_t config_target_socket_;
    supported_target_socket_t msg_target_socket_;
    // Device -> IC, intercept upstream pcie-map transactions
    target_socket_t pcie_map_target_socket_;

    struct cfg_header_t {
        uint16_t command;  // command register
        uint32_t base_address[7];  // 6 BARs + expansion ROM BAR
    };
    // Cache each function's cfg header, f0 is required
    std::map<uint16_t, cfg_header_t> function_data_;
    std::vector<iface::PcieBaseAddressRegisterQueryInterface::PCIeBar>
    bar_info_;

    // Extension and sender for talking to the Simics pcie upstream target
    PcieMappingInterconnectExtensionSender<initiator_socket_t> sender_;
    iface::PcieMapExtension pcie_map_extension_;

    // Trigger hot reset on the device
    iface::PcieResetInterface *reset_ {nullptr};

    // A map to store MEM address ranges and corresponding socket IDs
    std::map<std::pair<size_t, size_t>, size_t> address_id_mem_map_;
    // A map to store IO address ranges and corresponding socket IDs
    std::map<std::pair<size_t, size_t>, size_t> address_id_io_map_;
    // A map from function_number/bar_offset to socket IDs
    std::map<std::pair<size_t, size_t>, size_t> fn_bar_id_map_;

    ConfObjectRef simulation_obj_ {nullptr};
    // For PCIe gasket object, the pcie_device and transaction interfaces
    // are implemented on the gasket_obj_. For other cases, it is the same
    // object as simulation_obj_
    ConfObjectRef gasket_obj_ {nullptr};

    // In some case (for example ARI), function number has 8 bits
    // This boolean variable is set when the function number from getBarInfo()
    // returns a value greater than 7
    bool function_number_has_8bits_ {false};
};

template <unsigned int BUSWIDTH, typename TYPES>
void PcieMappingInterconnect<BUSWIDTH, TYPES>::before_end_of_elaboration() {
    // Because of the way SystemC has been designed, we cannot create new
    // sockets dynamically ouside of the CTOR except for in the
    // before_end_of_elaboration() method.

    // Create initiator sockets equal to the number of Config, Mem/IO
    // target sockets and Msg target socket
    initiator_sockets_.init(function_data_.size() + bar_info_.size() + 1);

    size_t socket_id = 0;
    // Allocate and initialize function data based on device configuration
    for (; socket_id < function_data_.size(); ++socket_id) {
        if (config_target_socket_.tlm) {
            initiator_sockets_[socket_id].bind(*config_target_socket_.tlm);
        } else {
            initiator_sockets_[socket_id].bind(*config_target_socket_.multi);
        }
    }

    for (auto &bar : bar_info_) {
        supported_target_socket_t target = {};
        castToTarget(bar.target_socket, &target);
        if (target.tlm) {
            initiator_sockets_[socket_id].bind(*target.tlm);
        } else if (target.multi) {
            initiator_sockets_[socket_id].bind(*target.multi);
        } else {
            SC_REPORT_ERROR(
                PCIE_MAPPING_INTERCONNECT_TAG,
                "BAR target_socket type is not supported");
        }
        fn_bar_id_map_[std::make_pair(bar.function, bar.offset)] = socket_id;
        ++socket_id;
    }

    if (msg_target_socket_.tlm) {
        initiator_sockets_[socket_id].bind(*msg_target_socket_.tlm);
    } else {
        initiator_sockets_[socket_id].bind(*msg_target_socket_.multi);
    }
}

template <unsigned int BUSWIDTH, typename TYPES>
void PcieMappingInterconnect<BUSWIDTH, TYPES>::connected(uint16_t device_id) {
    // Use the side effect of it to set device object on pcie map gasket.
    // The device object is used internally for get the mapping helper port
    // object
    pcie_map_extension_.get_device_id(gasket_obj_);

    // Endpoints must add their functions and map other resources such as
    // Memory and I/O BARs
    size_t socket_id = 0;
    for (const auto &fd : function_data_) {
        delFunction(device_id | fd.first);
        addFunction(device_id | fd.first);
        retrieveFunctionData(socket_id++, -1);
        updateMappings(fd.first);
    }
}

template <unsigned int BUSWIDTH, typename TYPES>
void PcieMappingInterconnect<BUSWIDTH, TYPES>::
disconnected(uint16_t device_id) {
    for (auto &fd : function_data_) {
        delFunction(device_id | fd.first);
        // disabled both MEM and IO
        fd.second.command = 0;
        updateMappings(fd.first);
    }
}

template <unsigned int BUSWIDTH, typename TYPES>
void PcieMappingInterconnect<BUSWIDTH, TYPES>::hotReset() {
    if (reset_) {
        reset_->hotReset();
        size_t socket_id = 0;
        for (const auto &fd : function_data_) {
            retrieveFunctionData(socket_id++, -1);
        }
        updateMappings();
    }
}

template <unsigned int BUSWIDTH, typename TYPES>
void PcieMappingInterconnect<BUSWIDTH, TYPES>::warmReset() {
    if (reset_) {
        reset_->warmReset();
        size_t socket_id = 0;
        for (const auto &fd : function_data_) {
            retrieveFunctionData(socket_id++, -1);
        }
        updateMappings();
    }
}

template <unsigned int BUSWIDTH, typename TYPES>
void PcieMappingInterconnect<BUSWIDTH, TYPES>::
addFunction(uint16_t device_id) {
    std::ostringstream os;
    os << "Adding function for device_id: "
       << device_id;
    SC_REPORT_INFO(PCIE_MAPPING_INTERCONNECT_TAG, os.str().c_str());
    pcie_map_extension_.add_function(getCfgMapHelper(device_id), device_id);
    if (sender_.failed_transaction_.payload()) {
        SC_REPORT_INFO(PCIE_MAPPING_INTERCONNECT_TAG,
                       "Failure to add function");
        return;
    }
}

template <unsigned int BUSWIDTH, typename TYPES>
void PcieMappingInterconnect<BUSWIDTH, TYPES>::
delFunction(uint16_t device_id) {
    std::ostringstream os;
    os << "Deleting function for device_id: "
       << device_id;
    SC_REPORT_INFO(PCIE_MAPPING_INTERCONNECT_TAG, os.str().c_str());
    pcie_map_extension_.del_function(getCfgMapHelper(device_id), device_id);
    if (sender_.failed_transaction_.payload()) {
        SC_REPORT_INFO(PCIE_MAPPING_INTERCONNECT_TAG,
                       "Failure to delete function");
        return;
    }
}

template <unsigned int BUSWIDTH, typename TYPES>
void PcieMappingInterconnect<BUSWIDTH, TYPES>::
connect(iface::PcieDeviceQueryInterface *pci,
        iface::PcieBaseAddressRegisterQueryInterface *bar,
        iface::PcieResetInterface *reset, ConfObjectRef o) {
    if (pci == nullptr) {
        SIM_LOG_ERROR(
            o, Log_Configuration,
            "PcieDeviceQueryInterface not implemented on the device");
        return;
    }

    if (bar == nullptr) {
        SIM_LOG_ERROR(
            o, Log_Configuration,
            "PcieBaseAddressRegisterQueryInterface not implemented on"
            " the device");
        return;
    }

    if (reset == nullptr) {
        SIM_LOG_ERROR(o, Log_Configuration,
                      "PcieResetInterface not implemented on the device");
        return;
    }

    // Get the config target_socket from the device
    castToTarget(pci->getConfigTargetSocket(), &config_target_socket_);

    // Get the message target_socket from the device
    castToTarget(pci->getMsgTargetSocket(), &msg_target_socket_);

    // Get the BAR info
    bar_info_ = bar->getBarInfo();

    // Set the reset interface
    reset_ = reset;

    // Calculate how many functions are there (f0 is required)
    std::set<int> functions {0};
    for (const auto &it : bar_info_) {
        if (functions.find(it.function) == functions.end()) {
            function_data_[it.function];
            functions.insert(it.function);
            if (it.function > 7) {
                function_number_has_8bits_ = true;
            }
        }
    }

    // Bind pcie_map_target_socket_ so device can call pcie_map interface
    auto *initiator = dynamic_cast<
            tlm::tlm_initiator_socket<BUSWIDTH, TYPES> *>(
                pci->getPcieMapInitiatorSocket());
    if (initiator) {
        pcie_map_target_socket_.bind(*initiator);
    } else {
        SIM_LOG_ERROR(o, Log_Configuration,
                      "object returned by getPcieMapInitiatorSocket was not"
                      " a tlm::tlm_initiator_socket.");
    }

    simulation_obj_ = o;
    createCfgMapHelper();
}

template <unsigned int BUSWIDTH, typename TYPES>
std::map<std::pair<size_t, size_t>, size_t>
PcieMappingInterconnect<BUSWIDTH, TYPES>::addressIdMemMap() const {
    return address_id_mem_map_;
}

template <unsigned int BUSWIDTH, typename TYPES>
std::map<std::pair<size_t, size_t>, size_t>
PcieMappingInterconnect<BUSWIDTH, TYPES>::addressIdIoMap() const {
    return address_id_io_map_;
}

template <unsigned int BUSWIDTH, typename TYPES>
void PcieMappingInterconnect<BUSWIDTH, TYPES>::
retrieveFunctionData(size_t socket_id, int bar_id) {
    if (initiator_sockets_.size() <= socket_id) {
        std::ostringstream os;
        os << "Invalid socket_id: " << socket_id;
        SC_REPORT_INFO(PCIE_MAPPING_INTERCONNECT_TAG, os.str().c_str());
        return;
    }

    tlm::tlm_generic_payload trans;
    // The data pointer, length and stream width attribute are reused
    // since they are not modifiable by the target according to the spec
    uint32_t value = 0;
    trans.set_data_ptr(reinterpret_cast<unsigned char *>(&value));
    trans.set_data_length(4);
    trans.set_streaming_width(4);

    trans.set_command(tlm::TLM_READ_COMMAND);
    trans.set_address(0x4);
    initiator_sockets_[socket_id]->transport_dbg(trans);
    auto fd = function_data_.begin();
    std::advance(fd, socket_id);
    fd->second.command = value;

    // NOTE: the IO, Mem32, Mem64 bits are ignored by updateMapping,
    // no need to filter them out here
    for (const auto &bar : bar_info_) {
        // BarInfo holds _all_ BARs for _all_ functions. This can probably
        // be done in a much more clever way that avoids iterating over the
        // entire BarInfo for each function in the outer loop.
        if (bar.function != fd->first) {
            continue;
        }

        int expected_bar_id = bar.offset == 0x30 ? 6 : (bar.offset - 0x10) / 4;
        // bar_id == -1 => read all bars
        if (bar_id != -1 && bar_id != expected_bar_id) {
            continue;
        }

        trans.set_address(bar.offset);
        initiator_sockets_[socket_id]->transport_dbg(trans);
        fd->second.base_address[expected_bar_id] = value;
        if (bar.is_64bit) {
            if (expected_bar_id == 6) {
                SC_REPORT_ERROR(
                    PCIE_MAPPING_INTERCONNECT_TAG,
                    "Expansion ROM BAR can't support 64-bit address");
            } else {
                // Read higher 32 bits in next BAR
                trans.set_address(bar.offset + 4);
                initiator_sockets_[socket_id]->transport_dbg(trans);
                fd->second.base_address[expected_bar_id + 1] = value;
            }
        }
    }
}

template <unsigned int BUSWIDTH, typename TYPES>
void PcieMappingInterconnect<BUSWIDTH, TYPES>::
addMap(types::map_info_t info, types::pcie_type_t type) {
    std::ostringstream os;
    os << "Adding mapping of type: "
       << pcieTypeName(type);
    SC_REPORT_INFO(PCIE_MAPPING_INTERCONNECT_TAG, os.str().c_str());
    pcie_map_extension_.add_map(info, type);
    if (sender_.failed_transaction_.payload()) {
        SC_REPORT_INFO(PCIE_MAPPING_INTERCONNECT_TAG, "Failure to add map");
        return;
    }
}

template <unsigned int BUSWIDTH, typename TYPES>
void PcieMappingInterconnect<BUSWIDTH, TYPES>::
delMap(size_t socket_id, types::pcie_type_t type) {
    types::map_info_t::physical_address_t base = 0xffffffffffffffff;
    if (type == types::PCIE_Type_Mem) {
        auto it = std::find_if(
            address_id_mem_map_.begin(),
            address_id_mem_map_.end(),
            [socket_id](
                const std::pair<std::pair<size_t, size_t>, size_t> &entry) {
                return entry.second == socket_id;
            });

        if (it != address_id_mem_map_.end()) {
            base = it->first.first;
            address_id_mem_map_.erase(it);
        }
    } else if (type == types::PCIE_Type_IO) {
        auto it = std::find_if(
            address_id_io_map_.begin(),
            address_id_io_map_.end(),
            [socket_id](
                const std::pair<std::pair<size_t, size_t>, size_t> &entry) {
                return entry.second == socket_id;
            });

        if (it != address_id_io_map_.end()) {
            base = it->first.first;
            address_id_io_map_.erase(it);
        }
    }

    if (base != 0xffffffffffffffff) {
        std::ostringstream os;
        os << "Deleting mapping of type: "
           << pcieTypeName(type) << ", base: 0x" << std::hex << base;
        SC_REPORT_INFO(PCIE_MAPPING_INTERCONNECT_TAG, os.str().c_str());

        pcie_map_extension_.del_map(base, type);
        if (sender_.failed_transaction_.payload()) {
            SC_REPORT_INFO(PCIE_MAPPING_INTERCONNECT_TAG, "Failure to del map");
            return;
        }
    }
}

template <unsigned int BUSWIDTH, typename TYPES>
void PcieMappingInterconnect<BUSWIDTH, TYPES>::
updateMappings(int function_id, int bar_id) {
    for (const auto &bi : bar_info_) {
        if (function_id != -1 && bi.function != function_id) {
            continue;
        }

        auto header = function_data_.at(bi.function);

        int expected_bar_id = bi.offset == 0x30 ? 6 : (bi.offset - 0x10) / 4;
        if (bar_id != -1 && bar_id != expected_bar_id) {
            continue;
        }

        uint64_t base_address = header.base_address[expected_bar_id];
        if (bi.is_64bit) {
            if (expected_bar_id == 6) {
                SC_REPORT_ERROR(
                    PCIE_MAPPING_INTERCONNECT_TAG,
                    "Expansion ROM BAR can't support 64-bit address");
            } else {
                base_address |= static_cast<uint64_t>(
                    header.base_address[expected_bar_id + 1]) << 32;
            }
        }

        bool enable = header.command & 1;
        types::pcie_type_t type = types::PCIE_Type_IO;
        if (bi.is_memory) {
            type = types::PCIE_Type_Mem;
            if (bi.offset == 0x30) {
                enable = base_address & 1;
            } else {
                enable = (header.command >> 1) & 1;
            }
        }
        base_address &= ~((1ULL << bi.size_bits) - 1);
        size_t socket_id = fn_bar_id_map_.at(
            std::make_pair(bi.function, bi.offset));
        delMap(socket_id, type);
        if (enable) {
            // The `start` field in map_info_t is set to base,
            // so later when receiving the transaction, the address
            // can be used to route the transaction to the right socket
            types::map_info_t info(base_address, base_address,
                                   1 << bi.size_bits, function_id);
            auto address_range = std::make_pair(info.base,
                                                info.base + info.length);

            if (bi.is_memory) {
                address_id_mem_map_[address_range] = socket_id;
                std::ostringstream os;
                os << "Memory address for function " << bi.function
                   << " and bar ID " << expected_bar_id
                   << " in range of [0x" << std::hex << info.base
                   << "-0x" << info.base + info.length << "] maps to socket ID "
                   << socket_id;
                SC_REPORT_INFO(PCIE_MAPPING_INTERCONNECT_TAG, os.str().c_str());
            } else {
                address_id_io_map_[address_range] = socket_id;
                std::ostringstream os;
                os << "IO address for function " << bi.function
                   << " and bar ID " << expected_bar_id
                   << " in range of [0x" << std::hex << info.base
                   << "-0x" << info.base + info.length << "] maps to socket ID "
                   << socket_id;
                SC_REPORT_INFO(PCIE_MAPPING_INTERCONNECT_TAG, os.str().c_str());
            }
            addMap(info, type);
        }
    }
}

template <unsigned int BUSWIDTH, typename TYPES>
void PcieMappingInterconnect<BUSWIDTH, TYPES>::
updateCommand(uint16_t function_id, uint16_t old_command) {
    if (function_data_[function_id].command != old_command) {
        updateMappings(function_id);
    }
}

template <unsigned int BUSWIDTH, typename TYPES>
void PcieMappingInterconnect<BUSWIDTH, TYPES>::
interceptCfgHeaderAccess(size_t socket_id,
                         tlm::tlm_generic_payload &trans) {  // NOLINT
    unsigned int size = trans.get_data_length();
    // Check assumptions (but leave it up to target device to report errors)
    if (trans.get_byte_enable_ptr() || trans.get_streaming_width() != size) {
        return;
    }

    // We are only interested in write access to BARn or Command regs
    if (!trans.is_write()) {
        return;
    }

    // Maximum size is 8
    uint64_t *data = reinterpret_cast<uint64_t *>(trans.get_data_ptr());

    auto it = function_data_.begin();
    std::advance(it, socket_id);
    uint16_t function_id = it->first;
    auto &header = it->second;
    sc_dt::uint64 offset = trans.get_address();
    switch (offset) {
    case 0x04: {  // command & status
        uint16_t old_command = header.command;
        retrieveFunctionData(socket_id, -1);
        header.command = (*data) & 0xffff;  // ignore status
        updateCommand(function_id, old_command);
        break;
    }
    case 0x10:  // BAR0-BAR5
    case 0x14:
    case 0x18:
    case 0x1c:
    case 0x20:
    case 0x24: {
        int bar_id = (offset - 0x10) / 4;
        retrieveFunctionData(socket_id, bar_id);
        // NOTE: the IO, Mem32, Mem64 bits are ignored by
        // updateMapping, no need to filter them out here. In
        // addition, any bits less than the size of the BAR as defined
        // by the BarInfo will also be ignored/truncated by
        // updateMapping; making sure we always send a naturally
        // aligned address to Simics
        header.base_address[bar_id] = *data & 0xffffffff;
        if (size == 8 && !(offset % 8)) {
            header.base_address[bar_id + 1] = *data >> 32;
        }
        updateMappings(function_id, bar_id);
        break;
    }
    case 0x30:  // Expansion ROM BAR
        int bar_id = 6;
        retrieveFunctionData(socket_id, bar_id);
        header.base_address[bar_id] = *data & 0xffffffff;
        updateMappings(function_id, bar_id);
        break;
    }
}

template <unsigned int BUSWIDTH, typename TYPES>
size_t PcieMappingInterconnect<BUSWIDTH, TYPES>::
findSocketId(types::pcie_type_t type, uint16_t function_id,
             tlm::tlm_generic_payload &trans) const {  // NOLINT
    if (type == types::PCIE_Type_Cfg) {
        auto found = function_data_.find(function_id);
        if (found == function_data_.end()) {
            std::ostringstream os;
            os << "Function id(" << function_id
               << ") not found for this device";
            throw std::invalid_argument {
                os.str()
            };
        }
        return std::distance(function_data_.begin(), found);
    } else if (type == types::PCIE_Type_Mem) {
        size_t address = trans.get_address();
        for (const auto &entry : address_id_mem_map_) {
            auto startAddress = entry.first.first;
            auto endAddress = entry.first.second;
            auto id = entry.second;

            if (address >= startAddress && address < endAddress) {
                if (enable_base_address_subtraction) {
                    trans.set_address(address - startAddress);
                }
                return id;
            }
        }
        throw std::invalid_argument {
            "Unable to find the initiator_socket to forward the MEM transaction"
        };
    } else if (type == types::PCIE_Type_IO) {
        size_t address = trans.get_address();
        for (const auto &entry : address_id_io_map_) {
            auto startAddress = entry.first.first;
            auto endAddress = entry.first.second;
            auto id = entry.second;

            if (address >= startAddress && address < endAddress) {
                if (enable_base_address_subtraction) {
                    trans.set_address(address - startAddress);
                }
                return id;
            }
        }
        throw std::invalid_argument {
            "Unable to find the initiator_socket to forward the IO transaction"
        };
    } else if (type == types::PCIE_Type_Msg) {
        return function_data_.size() + bar_info_.size();
    }
    throw std::invalid_argument {
        "Unsupported PCIe type"
    };
}

template <unsigned int BUSWIDTH, typename TYPES>
void PcieMappingInterconnect<BUSWIDTH, TYPES>::
createCfgMapHelper() {
    auto *map_helper = SIM_get_class("pcie_map_helper_cpp");
    if (map_helper == nullptr) {
        SIM_LOG_ERROR(
            simulation_obj_, Log_Configuration,
            "Simics class pcie_map_helper_cpp is required."
            "Try load the module pcie-map-helper-c++ first.");
        return;
    }

    if (SIM_c_get_interface(simulation_obj_, "pcie_device") == nullptr) {
        // Make sure simulation_obj_ is ready to be used by other objects
        if (!SIM_object_is_configured(simulation_obj_)) {
            SIM_LOG_ERROR(
                simulation_obj_, Log_Configuration,
                "Simulation object is not configured yet, cannot get"
                " its gasket_list attribute");
            return;
        }

        // For PCIe gasket object, the interface is not implemented
        // on the simulation object, but rather the gasket object
        auto gasket_list = SIM_get_attribute(simulation_obj_, "gasket_list");
        int gasket_count = SIM_attr_list_size(gasket_list);
        for (int i = 0; i < gasket_count; ++i) {
            auto obj = SIM_attr_object(SIM_attr_list_item(gasket_list, i));
            if (SIM_c_get_interface(obj, "pcie_device")) {
                gasket_obj_ = obj;
                break;
            }
        }
        if (gasket_obj_.object() == nullptr) {
            SIM_LOG_ERROR(
                simulation_obj_, Log_Configuration,
                "No gasket object implemented the required"
                " pcie_device interface");
            return;
        }
    } else {
        gasket_obj_ = simulation_obj_.object();
    }

    for (const auto &fd : function_data_) {
        auto pcie_type = std_to_attr<>(
            std::pair<std::string, int>("pcie_type", types::PCIE_Type_Cfg));
        auto forward_target = std_to_attr<>(
            std::pair<std::string, ConfObjectRef>("forward_target",
                                                  gasket_obj_));
        auto function_id = std_to_attr<>(
            std::pair<std::string, int>("function_id", fd.first));
        attr_value_t attr = SIM_make_attr_list(3, pcie_type, forward_target,
                                               function_id);

        auto obj_name = gasket_obj_.name() + ".port.cfg" +  \
            std::to_string(fd.first);
        auto *o = SIM_get_object(obj_name.c_str());
        if (o == nullptr) {
            (void)SIM_clear_exception();
            o = SIM_create_object(map_helper, obj_name.c_str(), attr);
        }
        assert(o);
        SIM_attr_free(&attr);
    }
}

template <unsigned int BUSWIDTH, typename TYPES>
conf_object_t *PcieMappingInterconnect<BUSWIDTH, TYPES>::
getCfgMapHelper(uint16_t device_id) {
    std::string name = "port.cfg";
    if (function_number_has_8bits_) {
        name += std::to_string(device_id & 0xff);
    } else {
        name += std::to_string(device_id & 7);
    }
    auto map_helper = SIM_object_descendant(gasket_obj_, name.c_str());
    assert(map_helper);
    return map_helper;
}

template <unsigned int BUSWIDTH, typename TYPES>
size_t PcieMappingInterconnect<BUSWIDTH, TYPES>::
validateTransaction(tlm::tlm_generic_payload &trans) {  // NOLINT
    PcieTlmExtension *pcie_ext = nullptr;
    trans.get_extension<PcieTlmExtension>(pcie_ext);

    if (pcie_ext == nullptr) {
        trans.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        SC_REPORT_ERROR(PCIE_MAPPING_INTERCONNECT_TAG,
                        "PcieTlmExtension is required but not found");
        return 0;
    }

    std::ostringstream os;
    os << "Received a PCIe transaction with type "
       << pcieTypeName(pcie_ext->type);
    if (pcie_ext->device_id_set) {
        os << ", " << pcieDeviceIdStr(pcie_ext->device_id);
    }
    os << ", address 0x" << std::hex << trans.get_address() << std::endl;
    SC_REPORT_INFO(PCIE_MAPPING_INTERCONNECT_TAG, os.str().c_str());

    if (pcie_ext->type == types::PCIE_Type_Not_Set
        || pcie_ext->type == types::PCIE_Type_Other) {
        SC_REPORT_INFO(PCIE_MAPPING_INTERCONNECT_TAG,
                       "Only support following PCIe types: MEM/IO/CFG/MSG");
        trans.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return 0;
    }

    if (pcie_ext->type == types::PCIE_Type_Cfg
        && !pcie_ext->device_id_set) {
        SC_REPORT_INFO(PCIE_MAPPING_INTERCONNECT_TAG,
                       "PCIe device ID ATOM is required");
        trans.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return 0;
    }

    size_t socket_id = 0;
    try {
        socket_id = findSocketId(pcie_ext->type, pcie_ext->device_id & 7,
                                 trans);
    } catch (const std::exception &e) {
        SC_REPORT_INFO(PCIE_MAPPING_INTERCONNECT_TAG, e.what());
        trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
        return 0;
    }

    if (socket_id >= initiator_sockets_.size()) {
        os.clear();
        os << "Initiator socket ID " << socket_id << " not created yet";
        SC_REPORT_INFO(PCIE_MAPPING_INTERCONNECT_TAG, os.str().c_str());
        trans.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return 0;
    }

    if (pcie_ext->type == types::PCIE_Type_Cfg) {
        interceptCfgHeaderAccess(socket_id, trans);
    }

    return socket_id;
}

template <unsigned int BUSWIDTH, typename TYPES>
void PcieMappingInterconnect<BUSWIDTH, TYPES>::
transaction_b_transport(tlm::tlm_generic_payload &trans,  // NOLINT: SystemC API
                        sc_core::sc_time &t) {  // NOLINT: SystemC API
    trans.set_response_status(tlm::TLM_OK_RESPONSE);
    size_t socket_id = validateTransaction(trans);
    if (trans.get_response_status() == tlm::TLM_OK_RESPONSE) {
        initiator_sockets_.at(socket_id)->b_transport(trans, t);
    }
}

template <unsigned int BUSWIDTH, typename TYPES>
unsigned int PcieMappingInterconnect<BUSWIDTH, TYPES>::
transaction_transport_dbg(tlm::tlm_generic_payload &trans) {  // NOLINT
    trans.set_response_status(tlm::TLM_OK_RESPONSE);
    size_t socket_id = validateTransaction(trans);
    if (trans.get_response_status() == tlm::TLM_OK_RESPONSE) {
        return initiator_sockets_.at(socket_id)->transport_dbg(trans);
    } else {
        return 0;
    }
}

/*
 * For pcie-map, just relay the incoming BW transaction to target socket
 */
template <unsigned int BUSWIDTH, typename TYPES>
void PcieMappingInterconnect<BUSWIDTH, TYPES>::
pcie_map_invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                                   sc_dt::uint64 end_range) {
    pcie_map_target_socket_->invalidate_direct_mem_ptr(start_range,
                                                       end_range);
}

/*
 * For pcie-map, just relay the incoming transaction to outgoing socket
 */
template <unsigned int BUSWIDTH, typename TYPES>
void PcieMappingInterconnect<BUSWIDTH, TYPES>::pcie_map_b_transport(
    tlm::tlm_generic_payload &trans,  // NOLINT
    sc_core::sc_time &t) {  // NOLINT
    pcie_map_initiator_socket->b_transport(trans, t);
}

/*
 * For pcie-map, just relay the incoming transaction to outgoing socket
 */
template <unsigned int BUSWIDTH, typename TYPES>
unsigned int PcieMappingInterconnect<BUSWIDTH, TYPES>::pcie_map_transport_dbg(
    tlm::tlm_generic_payload &trans) {  // NOLINT
    return pcie_map_initiator_socket->transport_dbg(trans);
}

/*
 * For pcie-map, just relay the incoming transaction to outgoing socket
 */
template <unsigned int BUSWIDTH, typename TYPES>
bool PcieMappingInterconnect<BUSWIDTH, TYPES>::pcie_map_get_direct_mem_ptr(
    tlm::tlm_generic_payload &trans,  // NOLINT
    tlm::tlm_dmi &dmi_data) {  // NOLINT
    return pcie_map_initiator_socket->get_direct_mem_ptr(trans, dmi_data);
}

template <unsigned int BUSWIDTH, typename TYPES>
void PcieMappingInterconnect<BUSWIDTH, TYPES>::
castToTarget(sc_core::sc_object *object, supported_target_socket_t *target) {
    if (object == nullptr) {
        SC_REPORT_ERROR(PCIE_MAPPING_INTERCONNECT_TAG,
                        "Unable to castToTarget from a nullptr");
        return;
    }

    auto *tlm_target = dynamic_cast<
        tlm::tlm_target_socket<BUSWIDTH, TYPES> *>(object);
    if (tlm_target) {
        target->tlm = tlm_target;
        return;
    }

    auto *multi_target = dynamic_cast<
        tlm_utils::multi_target_base<BUSWIDTH, TYPES> *>(object);
    if (multi_target) {
        target->multi = multi_target;
        return;
    }

    SC_REPORT_ERROR(PCIE_MAPPING_INTERCONNECT_TAG,
                    "Unable to dynamic-cast PCIe target socket");
}

}  // namespace composite
}  // namespace systemc
}  // namespace simics

#endif
