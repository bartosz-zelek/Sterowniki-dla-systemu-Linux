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

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_GASKET_CLASS_H
#define SIMICS_SYSTEMC_SIMICS2TLM_GASKET_CLASS_H

#include <systemc>


#include <simics/simulator/sim-get-class.h>
#include <simics/systemc/gasket_class_interface.h>
#include <simics/systemc/simics2tlm/gasket_factory.h>
#include <simics/systemc/simulation_interface_proxy.h>
#include <simics/systemc/iface/sc_initiator_gasket_simics_adapter.h>
#include <simics/systemc/iface/sc_initiator_gasket_interface.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/version.h>

#include <string>

namespace simics {
namespace systemc {
namespace simics2tlm {

template<class TGasket,
         class TGasketAdapter,
         template<class, class> class TSimicsAdapter,
         class TGasketInterface>
class GasketClass
    : public ConfObject, public TGasketAdapter, public GasketClassInterface,
      public iface::ScInitiatorGasketInterface, public Version,
      public SimulationInterfaceProxy {
  public:
    explicit GasketClass(ConfObjectRef o)
        : ConfObject(o), TGasketAdapter(&gasket_, this) {}
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
        gasket_.set_gasket(createGasketByName(socket_name_, simics_object()));
    }

    // ScInitiatorGasketInterface
    virtual void set_dmi(bool enable) {
        gasket_.gasket()->set_dmi(enable);
    }
    virtual bool is_dmi_enabled() {
        return gasket_.gasket()->is_dmi_enabled();
    }
    virtual char *print_dmi_table() {
        return MM_STRDUP(
            gasket_.gasket()->get_dmi_data_table()->print().c_str());
    }

    static conf_class_t *registerGasketClass(const char* class_name) {
        conf_class_t *cls = SIM_get_class(class_name);
        if (SIM_clear_exception() == SimExc_No_Exception) {
            return cls;
        }

        auto new_cls = make_class<GasketClass>(
                class_name,
                "model of SystemC simics2tlm gasket class",
                "Class for binding Simics interface to SystemC Gasket.");

        new_cls->add(iface::createAdapter<
                     TSimicsAdapter<GasketClass, TGasketInterface>>());
        new_cls->add(iface::createAdapter<
                     iface::ScInitiatorGasketSimicsAdapter<GasketClass>>());
        new_cls->add(Attribute("target", "s",
                               "Name of the target socket the gasket is"
                               " connected to.",
                               ATTR_CLS_VAR(GasketClass, socket_name_),
                               Sim_Attr_Required));
        new_cls->add(Attribute("simulation", "o",
                               "Simics object implementing the SystemC"
                               " Simulation interface.",
                               ATTR_CLS_VAR(GasketClass, simulation_ref_),
                               Sim_Attr_Required));

        return *new_cls.get();
    }

  private:
    std::string socket_name_;
    ConfObjectRef simulation_ref_;
    TGasket gasket_;
};
}  // namespace simics2tlm
}  // namespace systemc  // NOLINT
}  // namespace simics  // NOLINT

#endif
