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

#ifndef SIMICS_SYSTEMC_COMPOSITE_PCI_GASKET_H
#define SIMICS_SYSTEMC_COMPOSITE_PCI_GASKET_H

#if defined SIMICS_5_API || defined SIMICS_6_API

#include <systemc>
#include <tlm>

#include <simics/systemc/connector.h>
#include <simics/systemc/device.h>

#include <simics/systemc/composite/pci_mapping_interconnect.h>

#include <simics/systemc/iface/io_memory_simics_adapter.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/simics2tlm/gasket_factory.h>
#include <simics/systemc/simics2tlm/io_memory.h>
#include <simics/systemc/simics2tlm/io_memory_gasket_adapter.h>

#include <simics/systemc/iface/pci_device_simics_adapter.h>
#include <simics/systemc/simics2tlm/pci_device.h>
#include <simics/systemc/simics2tlm/pci_device_gasket_adapter.h>

#include <simics/systemc/iface/pci_express_simics_adapter.h>
#include <simics/systemc/simics2tlm/pci_express.h>
#include <simics/systemc/simics2tlm/pci_express_gasket_adapter.h>

#include <simics/systemc/tlm2simics/gasket_factory.h>
#include <simics/systemc/tlm2simics/pci_bus.h>

namespace simics {
namespace systemc {

namespace composite {

class PciGasketBase {
  public:
    typedef PciGasketBase *is_composite_pci_gasket;
    template <typename C>
    static void initClassInternal(ConfClass *cls);

  protected:
    Connector<tlm2simics::PciBus> simics_pci_;
};

template <typename C>
void PciGasketBase::initClassInternal(ConfClass *cls) {
    static iface::IoMemorySimicsAdapter<C> io_memory_simics_adapter;
    static iface::PciDeviceSimicsAdapter<C> pci_device_simics_adapter;
    static iface::PciExpressSimicsAdapter<C> pci_express_simics_adapter;
    cls->add(simics::Attribute(
                 "pci_bus", "o|n",
                 "The PCI-bus to connect to.",
                 ATTR_CLS_VAR(C, simics_pci_)));
    cls->add(io_memory_simics_adapter);
    cls->add(pci_device_simics_adapter);
    cls->add(pci_express_simics_adapter);
}

/**
 * Composite Pci Gasket to help the wrapping of SystemC PCI devices in Simics.
 *
 * Provides an interconnect module that snoops the transactions to config space
 * and handles the Simics specific mapping required to forward transactions on
 * pci-bus io or memory space to the corresponding BAR target socket.
 */
template<unsigned int BUSWIDTH = 32,
         typename TYPES = tlm::tlm_base_protocol_types>
class PciGasket : public PciGasketBase,
                  public simics2tlm::IoMemoryGasketAdapter,
                  public simics2tlm::PciDeviceGasketAdapter,
                  public simics2tlm::PciExpressGasketAdapter {
  public:
    explicit PciGasket(iface::SimulationInterface *simulation)
        : IoMemoryGasketAdapter(&systemc_mmio_, simulation),
          PciDeviceGasketAdapter(&systemc_pci_, simulation),
          PciExpressGasketAdapter(&systemc_pci_express_, simulation),
          simulation_(simulation) {
    }

    // PciDeviceGasketAdapter
    virtual void bus_reset() { /* override */
        Context context(simulation_);
        interconnect_.busReset();
        PciDeviceGasketAdapter::bus_reset();
    }

    // NOTE: the template parameter is required for the
    // PciMappingInterconnect::connect*<> template methods, where it is
    // needed for the multi_passthrough_target_socket<> template class. By
    // using the TPciDevice as method parameter, the compiler can deduce the
    // template parameter.
    template<typename TPciDevice>
    void connect(TPciDevice *device) {
        connect(dynamic_cast<iface::PciDeviceQueryInterface *>(device),
                dynamic_cast<iface::BaseAddressRegisterQueryInterface *>(
                    device),
                simulation_->simics_object());
    }

    void connect(iface::PciDeviceQueryInterface *pci,
                 iface::BaseAddressRegisterQueryInterface *bar,
                 ConfObjectRef gasket) {
        ConfObjectRef o = simulation_->simics_object();

        if (!pci) {
            SIM_LOG_ERROR(gasket, Log_Configuration,
                            "The PCI device does not implement the"
                            " PciDeviceQueryInterface interface");
            return;
        }

        if (!bar) {
            SIM_LOG_ERROR(gasket, Log_Configuration,
                            "The PCI device does not implement the"
                            " BaseAddressRegisterQueryInterface interface");
            return;
        }

        // Connect the device's config space target socket and pci-bus
        // initiator socket via the interconnect to support snooping
        interconnect_.connect(pci, o);

        // Connecting the interconnect to the pci-bus
        simics_pci_->set_device(gasket);
        simics_pci_->set_gasket(
            tlm2simics::createGasket(
                &interconnect_.pci_bus_initiator_socket, o));

        // Connect device's pci-device target socket
        // The bus_reset() is intercepted by the composite class and handled
        // by busReset() before the transaction is forwarded to the device.
        // This allows the IC to unmap all mappings upon bus_reset.
        systemc_pci_.set_gasket(
            simics2tlm::createGasket(
                &interconnect_.simics_target_socket, o));
        systemc_pci_express_.set_gasket(
            simics2tlm::createGasket(
                &interconnect_.simics_target_socket, o));

        // Connect IC's config space target sockets (snooping)
        interconnect_.connectConfig(bar, &systemc_mmio_, o);

        // Connect device's io/mem space target socket(s)
        interconnect_.connectMmio(bar, &systemc_mmio_, o);
    }

  private:
    iface::SimulationInterface *simulation_;

    // Snoop PCI transactions and handle Simics mappings
    PciMappingInterconnect<BUSWIDTH, TYPES> interconnect_;

    // Gaskets required for wrapping a PCI device
    simics2tlm::IoMemory systemc_mmio_;  // incoming MMIO (conf/mem/io spaces)
    simics2tlm::PciDevice systemc_pci_;  // Simics pci-bus specific interface
    simics2tlm::PciExpress systemc_pci_express_;  // Simics PCIe interface
};

}  // namespace composite

template <class T>
void SCLCompositePciInit(typename T::is_composite_pci_gasket,
                         ConfClass *cls) {
    systemc::composite::PciGasketBase::initClassInternal<T>(cls);
}

}  // namespace systemc
}  // namespace simics

#else
static_assert(false, "pci_express interface is deprecated. Use SIMICS_API:=6.");
#endif
#endif
