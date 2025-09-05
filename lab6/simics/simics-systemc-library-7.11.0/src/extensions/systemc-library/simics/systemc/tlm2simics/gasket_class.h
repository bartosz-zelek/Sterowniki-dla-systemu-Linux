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

#ifndef SIMICS_SYSTEMC_TLM2SIMICS_GASKET_CLASS_H
#define SIMICS_SYSTEMC_TLM2SIMICS_GASKET_CLASS_H

#include <systemc>


#include <simics/simulator/sim-get-class.h>
#include <simics/systemc/connector.h>
#include <simics/systemc/gasket_class_interface.h>
#include <simics/systemc/simulation_interface_proxy.h>
#include <simics/systemc/tlm2simics/gasket_factory.h>
#include <simics/systemc/version.h>

#include <string>

namespace simics {
namespace systemc {
namespace tlm2simics {

template<class TGasket>
class GasketClass
    : public ConfObject, public GasketClassInterface, public Version {
  public:
    explicit GasketClass(ConfObjectRef o) : ConfObject(o) {}
    virtual void finalize() {
        // Avoid recursive calls to finalize_instance
        SIM_set_object_configured(obj());

        // Make sure the Adapter is configured
        simulation_ref_.require();
    }
    virtual iface::ScVersionInterface *version() {
        return this;
    }
    virtual void createGasket(iface::SimulationInterface *simulation) {
        object_->set_gasket(createGasketByName(socket_name_,
                                               simulation->simics_object()));
    }
    static conf_class_t *registerGasketClass(const char* class_name) {
        conf_class_t *cls = SIM_get_class(class_name);
        if (SIM_clear_exception() == SimExc_No_Exception) {
            return cls;
        }

        auto new_cls = make_class<GasketClass>(
                class_name,
                "model of SystemC tlm2simics gasket class",
                "Class for binding Simics interface to SystemC Gasket.");

        new_cls->add(Attribute("initiator", "s",
                               "Name of the initiator socket the gasket"
                               " is connected to.",
                               ATTR_CLS_VAR(GasketClass, socket_name_),
                               Sim_Attr_Required));
        new_cls->add(Attribute("simulation", "o",
                               "Simics object implementing the SystemC"
                               " Simulation interface.",
                               ATTR_CLS_VAR(GasketClass, simulation_ref_),
                               Sim_Attr_Required));
        new_cls->add(Attribute("object", "o|n",
                               "Simics object implementing the corresponding"
                               " interface of this gasket.",
                               ATTR_CLS_VAR(GasketClass, object_)));

        return *new_cls.get();
    }

  private:
    std::string socket_name_;
    ConfObjectRef simulation_ref_;
    Connector<TGasket> object_;
};
}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics

#endif
