/*                            -*- mode: C++; c-file-style: "virtutech-c++" -*-

  © 2024 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

/*
 * Used by device that simulates a PCIe endpoint/multifunciton endpoint with
 * a Type 0 header.
 * It implements the following Simics interfaces:
 * - `pcie_device` interface which handles the PCIe connection
 * - `pcie_transaction` interface which receives PCIe transactions
 * - `signal` interface which receives warm reset
 * It implements the following Simics connectors:
 * - `pcie_map` connector which controls the map/unmap bar memory, and
 *   enable/disable PCI functions
 * - `pcie_transaction` connector which sends PCIe transactions
 */

#ifndef SIMICS_SYSTEMC_COMPOSITE_PCIE_GASKET_H
#define SIMICS_SYSTEMC_COMPOSITE_PCIE_GASKET_H

#include <tlm>

#include <simics/cc-api.h>  // Attribute
#include <simics/systemc/connector.h>
#include <simics/systemc/composite/pcie_mapping_interconnect.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/iface/pcie_device_simics_adapter.h>
#include <simics/systemc/iface/signal_simics_adapter.h>
#include <simics/systemc/iface/transaction_simics_adapter.h>
#include <simics/systemc/simics2systemc/signal.h>
#include <simics/systemc/simics2systemc/signal_gasket_adapter.h>
#include <simics/systemc/simics2tlm/pcie_device.h>
#include <simics/systemc/simics2tlm/pcie_device_gasket_adapter.h>
#include <simics/systemc/simics2tlm/pcie_transaction.h>
#include <simics/systemc/simics2tlm/transaction_gasket_adapter.h>
#include <simics/systemc/tlm2simics/pcie_map.h>
#include <simics/systemc/tlm2simics/pcie_transaction.h>

#include <iostream>
#include <map>
#include <utility>

namespace simics {
namespace systemc {
namespace composite {

class PcieGasketBase {
  public:
    typedef PcieGasketBase *is_composite_pcie_gasket;
    template <typename C>
    static void initClassInternal(ConfClass *cls) {
        static iface::PcieDeviceSimicsAdapter<C> pcie_device_simics_adapter;
        static iface::TransactionSimicsAdapter<C> transaction_simics_adapter;
        static iface::SignalSimicsAdapter<C> signal_simics_adapter;
        cls->add(Attribute("upstream_target", "o|n",
                           "The PCIe upstream target to connect to.",
                           ATTR_CLS_VAR(C, simics_transaction_target_)));
        cls->add(Attribute("address_id_mem_map", "[[[ii]i]*]",
                           "Internal. The map from MEM address range to"
                           " socket ID",
                           ATTR_GETTER(C, addressIdMemMap), nullptr));
        cls->add(Attribute("address_id_io_map", "[[[ii]i]*]",
                           "Internal. The map from IO address range to"
                           " socket ID",
                           ATTR_GETTER(C, addressIdIoMap), nullptr));
        cls->add(Attribute("enable_base_address_subtraction", "b",
                           "If set, MEM/IO transaction will receive only the"
                           " offset to the base address. Otherwise, it receives"
                           " the whole base address + offset. Default enabled",
                           ATTR_GETTER(C, enableBaseAddressSubtraction),
                           ATTR_SETTER(C, setEnableBaseAddressSubtraction)));
        cls->add(pcie_device_simics_adapter);
        cls->add(transaction_simics_adapter);
        cls->add(signal_simics_adapter);

        // Depends on the pcie_map_helper_cpp class
        auto *map_helper = SIM_get_class("pcie_map_helper_cpp");
        if (SIM_clear_exception() != SimExc_No_Exception) {
            std::cerr << "ERROR: Simics class pcie_map_helper_cpp is not"
                      << " registered. Please build module"
                      << " pcie-map-helper-c++ first."
                      << std::endl;
        }
        assert(map_helper);
        SIM_register_port(*cls, "port.mem", map_helper,
                          " The help object maps to the PCIe MEM space");
        SIM_register_port(*cls, "port.io", map_helper,
                          "The help object maps to the PCIe IO space");
        SIM_register_port(*cls, "port.msg", map_helper,
                          "The help object mapes to the PCIe MSG space");
    }

  protected:
    Connector<tlm2simics::PcieTransaction> simics_transaction_target_;
    ConnectorProxy<tlm2simics::PcieMap> simics_pcie_map_target_ {
        &simics_transaction_target_
    };
};

/**
 * Composite Pcie Gasket to help the wrapping of a SystemC PCIe
 * (multifunction) endpoint in Simics
 */
template<unsigned int BUSWIDTH = 32,
         typename TYPES = tlm::tlm_base_protocol_types>
class PcieGasket : public PcieGasketBase,
                   public simics2tlm::PcieDeviceGasketAdapter,
                   public simics2tlm::TransactionGasketAdapter,
                   public simics2systemc::SignalGasketAdapter {
  public:
    explicit PcieGasket(iface::SimulationInterface *simulation)
        : PcieDeviceGasketAdapter(&systemc_pcie_, simulation),
          TransactionGasketAdapter(&systemc_transaction_, simulation),
          SignalGasketAdapter(&systemc_signal_, simulation),
          simulation_(simulation) {
        if (simulation_->simics_object().object()) {
            setPcieTypeAndForwardTarget(simulation_->simics_object());
        }
    }

    PcieGasket(iface::SimulationInterface *simulation,
               simics::ConfObjectRef obj)
        : PcieDeviceGasketAdapter(&systemc_pcie_, simulation),
          TransactionGasketAdapter(&systemc_transaction_, simulation),
          SignalGasketAdapter(&systemc_signal_, simulation),
          simulation_(simulation),
          obj_(obj) {
        if (obj_.object()) {
            setPcieTypeAndForwardTarget(obj_);
        }
    }

    /*
     * Intercepts PcieDeviceGasketAdapter to connect/disconnect the simics
     * upstream target connector before pass it down to the IC
     */
    void connected(conf_object_t *port_obj, uint16_t id) override {
        Context context(simulation_);

        if (port_obj == nullptr) {
            SIM_LOG_ERROR(simulation_->simics_object(), 0,
                          "can't connect to NULL");
            return;
        }

        if (simics_transaction_target_) {
            if (simics_transaction_target_ != port_obj) {
                SIM_LOG_ERROR(
                        simulation_->simics_object(), 0,
                        "can't connect to '%s', currently connected to '%s'",
                        SIM_object_name(port_obj),
                        SIM_object_name(simics_transaction_target_));
                return;
            }
        }

        simics_transaction_target_.set(port_obj);
        interconnect_.connected(id);
    }

    void disconnected(conf_object_t *port_obj, uint16_t id) override {
        Context context(simulation_);

        if (port_obj == nullptr) {
            SIM_LOG_ERROR(simulation_->simics_object(), 0,
                          "can't disconnect from NULL");
            return;
        }

        if (simics_transaction_target_ != port_obj) {
            SIM_LOG_ERROR(simulation_->simics_object(), 0,
                          "can't disconnect from '%s', currently connected"
                          " to '%s'",
                          SIM_object_name(port_obj),
                          SIM_object_name(simics_transaction_target_));
            return;
        }

        if (simics_transaction_target_) {
            // Delete mapping
            interconnect_.disconnected(id);
            simics_transaction_target_.set(nullptr);
        }
    }

    void hot_reset() override {
        Context context(simulation_);
        interconnect_.hotReset();
    }

    // NOTE: By using the TPciDevice as method parameter, the compiler can
    // deduce the template parameter.
    template<typename TPcieDevice>
    void connect(TPcieDevice *device) {
        auto o = simulation_->simics_object();

        // Connect IC's pcie_device target socket
        // No payload is sent to the socket since PcieDevice is completely
        // handled here
        systemc_pcie_.set_gasket(
            simics2tlm::createGasket(
                    &interconnect_.pcie_device_target_socket, o));

        // Connect IC's transaction target socket (snooping)
        systemc_transaction_.set_gasket(
            simics2tlm::createGasket(
                    &interconnect_.transaction_target_socket, o));

        // Connect IC's warm reset target socket
        systemc_signal_.set_pin(&interconnect_.warm_reset_pin, false, o);

        // Connect IC's pcie_map initiator socket to the pcie upstream target
        simics_transaction_target_->set_gasket(
            tlm2simics::createMultiGasket(
                &interconnect_.pcie_map_initiator_socket, o));
        simics_pcie_map_target_->set_gasket(
            tlm2simics::createMultiGasket(
                &interconnect_.pcie_map_initiator_socket, o));

        // Retrieve the required information from the device to configure
        // the IC to prepare the connection with the device
        interconnect_.connect(
                dynamic_cast<iface::PcieDeviceQueryInterface *>(device),
                dynamic_cast<iface::PcieBaseAddressRegisterQueryInterface *>(
                        device),
                dynamic_cast<iface::PcieResetInterface *>(device),
                obj_ ? obj_ : o);
    }

    // Return the current map from address range to socket ID (Memory)
    std::map<std::pair<size_t, size_t>, size_t> addressIdMemMap() const {
        return interconnect_.addressIdMemMap();
    }
    // Return the current map from address range to socket ID (IO)
    std::map<std::pair<size_t, size_t>, size_t> addressIdIoMap() const {
        return interconnect_.addressIdIoMap();
    }

    bool enableBaseAddressSubtraction() const {
        return interconnect_.enable_base_address_subtraction;
    }
    void setEnableBaseAddressSubtraction(const bool &val) {
        interconnect_.enable_base_address_subtraction = val;
    }
    void setPcieTypeAndForwardTarget(ConfObjectRef obj) {
        SIM_set_attribute_default(
                SIM_object_descendant(obj, "port.mem"), "pcie_type",
                SIM_make_attr_int64(types::PCIE_Type_Mem));
        SIM_set_attribute_default(
                SIM_object_descendant(obj, "port.mem"), "forward_target",
                SIM_make_attr_object(obj));
        SIM_set_attribute_default(
                SIM_object_descendant(obj, "port.io"), "pcie_type",
                SIM_make_attr_int64(types::PCIE_Type_IO));
        SIM_set_attribute_default(
                SIM_object_descendant(obj, "port.io"), "forward_target",
                SIM_make_attr_object(obj));
        SIM_set_attribute_default(
                SIM_object_descendant(obj, "port.msg"), "pcie_type",
                SIM_make_attr_int64(types::PCIE_Type_Msg));
        SIM_set_attribute_default(
                SIM_object_descendant(obj, "port.msg"), "forward_target",
                SIM_make_attr_object(obj));
    }

    // Create map helpers which are mapped on the cfg space. It contains
    // PCI information like PCIe type and function id which is used to
    // route the transaction to the right TLM2 socket
    void createCfgMapHelper() {
        interconnect_.createCfgMapHelper();
    }

  private:
    iface::SimulationInterface *simulation_;

    // Snoop PCI transactions and handle Simics mappings
    PcieMappingInterconnect<BUSWIDTH, TYPES> interconnect_;

    // Gaskets required for wrapping a PCIe device
    simics2tlm::PcieDevice systemc_pcie_;
    simics2tlm::PcieTransaction systemc_transaction_;
    simics2systemc::Signal systemc_signal_;

    simics::ConfObjectRef obj_;
};

}  // namespace composite

template <class T>
void SCLCompositePcieInit(typename T::is_composite_pcie_gasket,
                          ConfClass *cls) {
    composite::PcieGasketBase::initClassInternal<T>(cls);
}

}  // namespace systemc
}  // namespace simics

#endif
