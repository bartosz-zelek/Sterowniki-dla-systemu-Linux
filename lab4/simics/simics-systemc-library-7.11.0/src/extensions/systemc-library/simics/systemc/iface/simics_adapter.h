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

#ifndef SIMICS_SYSTEMC_IFACE_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_SIMICS_ADAPTER_H

#include <simics/iface/interface-info.h>
#include <simics/devs/io-memory.h>
#include <simics/systemc/description_interface.h>
#include <simics/systemc/iface/simics_adapter_interface.h>
#include <simics/systemc/registry.h>
#include <simics/systemc/simics_lock.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/**
 * Base class for mapping Simics interface to a C++ interface.
 *
 * Simics interfaces rely on pointers to free functions. The SimicsAdapter
 * classes maps a static function to a corresponding C++ interface function.
 *
 * This class is not intended to be instantiated, but should be inherited from
 * when creating new interface adapters.
 *
 * @see IoMemorySimicsAdapter
 * @see createAdapter()
 */
template<typename TSimicsInterface>
class SimicsAdapter : public simics::iface::InterfaceInfo,
                      public Registrant<SimicsAdapterInterface> {
  public:
    SimicsAdapter(const char *name, TSimicsInterface iface)
        : name_(name),
          iface_(iface),
          conf_class_(NULL),
          map_adapter_(true) {
    }

    // simics::iface::InterfaceInfo
    std::string name() const override {
        return name_;
    }
    const interface_t *cstruct() const override {
        return &iface_;
    }

    void set_simics_class(conf_class_t *conf_class) override {
        if (!conf_class_)
            conf_class_ = conf_class;
    }
    conf_class_t *simics_class() const override {
        return conf_class_;
    }
    void set_map_adapter(bool map) override {
        map_adapter_ = map;
    }
    bool map_adapter() const override {
        return map_adapter_;
    }

  protected:
    template<typename TBase, typename TInterface>
    static TInterface *adapterWithoutLocking(conf_object_t *obj) {
        FATAL_ERROR_IF(!obj, "NULL object passed to Simics interface");
        TBase *so = static_cast<TBase*>(SIM_object_data(obj));
        TInterface *adapter = dynamic_cast<TInterface*>(so);
        FATAL_ERROR_IF(adapter == NULL, "Unable to locate GasketAdapter");
        return adapter;
    }
    // This version is mainly used by the tool_simics_adapter that the lock
    // is not implemented on obj but on the provider/connection object.
    template<typename TBase, typename TInterface>
    static SimicsLock<TInterface> adapter(conf_object_t *obj,
                                          conf_object_t *obj_lock) {
        SimulationInterface *simulation = cached_simulation_;
        if (obj_lock != cached_obj_lock_
            || cached_obj_id_ != SIM_object_id(obj_lock)) {
            ConfObject *lock_base =
                static_cast<ConfObject*>(SIM_object_data(obj_lock));
            simulation = dynamic_cast<SimulationInterface*>(lock_base);
            if (simulation == nullptr) {
                conf_object_t *parent = SIM_port_object_parent(obj_lock);
                if (parent) {
                    // Check port's parent
                    lock_base = static_cast<ConfObject*>(
                            SIM_object_data(parent));
                    simulation = dynamic_cast<SimulationInterface*>(lock_base);
                }
            }
            FATAL_ERROR_IF(simulation == NULL, "Unable to locate Simulation");
            cached_obj_lock_ = obj_lock;
            cached_obj_id_ = SIM_object_id(obj_lock);
            cached_simulation_ = simulation;
        }
        return {simulation, adapterWithoutLocking<TBase, TInterface>(obj)};
    }
    template<typename TBase, typename TInterface>
    static SimicsLock<TInterface> adapter(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj, obj);
    }
    template<typename TBase, typename TInterface>
    std::vector<std::string> descriptionBase(conf_object_t *obj,
                                             DescriptionType type) {
        if (SIM_object_class(obj) != conf_class_ )
            return {};

        TInterface *iface = adapterWithoutLocking<TBase, TInterface>(obj);
        auto *adapter = dynamic_cast<DescriptionInterface<TInterface> *>(iface);
        if (!adapter)
            return {};

        std::vector<std::string> description = adapter->description(type);
        if (description.empty())
            return {};

        description.insert(description.begin(), name_);
        description.insert(description.begin(), SIM_object_name(obj));
        return description;
    }

  private:
    std::string name_;
    TSimicsInterface iface_;
    conf_class_t *conf_class_;
    bool map_adapter_;
    // Under extreme scenarios (100M consecutive interface calls) the
    // dynamic_cast within adapter() becomes a problem. By caching the
    // result, this is avoided. This was highlighted (and verified) by VTune
    static conf_object_t *cached_obj_lock_;
    static std::string cached_obj_id_;
    static SimulationInterface* cached_simulation_;
};

template<typename TSimicsInterface> conf_object_t*
SimicsAdapter<TSimicsInterface>::cached_obj_lock_ = nullptr;
template<typename TSimicsInterface> std::string
SimicsAdapter<TSimicsInterface>::cached_obj_id_ = "";
template<typename TSimicsInterface> SimulationInterface*
SimicsAdapter<TSimicsInterface>::cached_simulation_ = nullptr;

/// \addtogroup Factories
/// @{

/**
 * Create a SimicsAdapter that can be registered with a ConfClass.
 *
 * A SimicsAdapter maps a static function to a corresponding C++ interface
 * function. The adapter is then registered with the ConfClass in init_local to
 * register the interface on the Simics class.
 *
 * The InterfaceSimicsAdapter template parameter is the actual SimicsAdapter
 * type (e.g., IoMemorySimicsAdapter<Adapter>)
 */
template <typename InterfaceSimicsAdapter>
InterfaceSimicsAdapter& createAdapter() {
    static InterfaceSimicsAdapter adapter;
    return adapter;
}

/// @}

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
