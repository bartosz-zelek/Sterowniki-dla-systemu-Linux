// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_COMPOSITE_PCI_GASKET_CLASS_H
#define SIMICS_SYSTEMC_COMPOSITE_PCI_GASKET_CLASS_H

#if defined SIMICS_5_API || defined SIMICS_6_API

#include <systemc>


#include <simics/simulator/sim-get-class.h>
#include <simics/systemc/composite/pci_gasket.h>
#include <simics/systemc/gasket_class_interface.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/simulation_interface_proxy.h>
#include <simics/systemc/version.h>

#include <string>

namespace simics {
namespace systemc {
namespace composite {

template<unsigned int BUSWIDTH = 32,
        typename TYPES = tlm::tlm_base_protocol_types>
class PciGasketClass
    : public ConfObject, public PciGasket<BUSWIDTH, TYPES>,
      public GasketClassInterface, public Version,
      public SimulationInterfaceProxy {
  public:
    explicit PciGasketClass(ConfObjectRef o)
        : ConfObject(o), PciGasket<BUSWIDTH, TYPES>(this) {}
    virtual void finalize() {
        // Avoid recursive calls to finalize_instance
        SIM_set_object_configured(obj());

        // Make sure the Adapter is configured
        simulation_ref_.require();
    }

    // GasketClassInterface
    virtual iface::ScVersionInterface *version() {
        return this;
    }
    virtual void createGasket(iface::SimulationInterface *simulation) {
        setSimulation(simulation);
        sc_core::sc_object *obj = sc_core::sc_find_object(
            sc_pci_device_name_.c_str());
        if (!obj) {
            SIM_LOG_ERROR(simulation->simics_object(), Log_Configuration,
                          "Invalid SystemC object name for "
                          "PciDeviceQueryInterface and "
                          "BaseAddressRegisterQueryInterface implementor");
            return;
        }

        iface::PciDeviceQueryInterface *pci =
            dynamic_cast<iface::PciDeviceQueryInterface *>(obj);
        iface::BaseAddressRegisterQueryInterface *bar =
            dynamic_cast<iface::BaseAddressRegisterQueryInterface *>(obj);
        this->connect(pci, bar, this->obj());
    }
    static conf_class_t *registerGasketClass(const char* class_name) {
        conf_class_t *cls = SIM_get_class(class_name);
        if (SIM_clear_exception() == SimExc_No_Exception) {
            return cls;
        }

        auto new_cls = make_class<PciGasketClass>(
                class_name,
                "model of SystemC compsite pci gasket class",
                "Class for binding Simics interface to SystemC Pci Gasket.",
                Sim_Class_Kind_Pseudo);

        new_cls->add(Attribute("device", "s",
                               "Name of the SystemC object that implements the"
                               " PciDeviceQueryInterface and"
                               " BaseAddressRegisterQueryInterface."
                               "All TLM2 sockets returned by the object must"
                               " have BUSWIDTH = 32 and TYPES ="
                               " tlm::tlm_base_protocol_types",
                               ATTR_CLS_VAR(PciGasketClass,
                                            sc_pci_device_name_),
                               Sim_Attr_Required));
        new_cls->add(Attribute("simulation", "o",
                               "Simics object implementing the SystemC"
                               " Simulation interface.",
                               ATTR_CLS_VAR(PciGasketClass, simulation_ref_),
                               Sim_Attr_Required));
        PciGasketBase::initClassInternal<PciGasketClass>(new_cls.get());

        return *new_cls.get();
    }

  private:
    std::string sc_pci_device_name_;
    ConfObjectRef simulation_ref_;
};
}  // namespace composite
}  // namespace systemc  // NOLINT
}  // namespace simics  // NOLINT

#endif
#endif
