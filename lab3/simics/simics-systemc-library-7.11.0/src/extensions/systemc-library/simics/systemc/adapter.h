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

#ifndef SIMICS_SYSTEMC_ADAPTER_H
#define SIMICS_SYSTEMC_ADAPTER_H

#include <systemc>

#include <simics/conf-object.h>
#include <simics/systemc/awareness/attributes.h>
#include <simics/systemc/awareness/proxy_interface.h>
#include <simics/systemc/awareness/proxy_builder.h>
#include <simics/systemc/awareness/tlm_spy_factory_registry.h>
#include <simics/systemc/event.h>
#include <simics/systemc/gasket_info.h>
#include <simics/systemc/gasket_tag.h>
#include <simics/systemc/init_gasket_classes.h>
#include <simics/systemc/internal.h>
#include <simics/systemc/late_binder.h>
#include <simics/systemc/module_loaded.h>
#include <simics/systemc/process_profiler_control.h>
#include <simics/systemc/sc_memory_profiler_control.h>
#include <simics/systemc/sc_signal_access.h>
#include <simics/systemc/simcontext.h>
#include <simics/systemc/simulation.h>
#include <simics/systemc/state_handler.h>
#include <simics/systemc/version.h>

#if INTC_EXT && USE_SIMICS_CHECKPOINTING
#include <simics/systemc/checkpoint.h>
#include <simics/systemc/iface/checkpoint_simics_adapter.h>
#include <simics/systemc/iface/temporal_state_simics_adapter.h>
#endif

#include <simics/systemc/iface/concurrency_group_simics_adapter.h>
#include <simics/systemc/iface/concurrency_mode_simics_adapter.h>
#include <simics/systemc/iface/event_delta_simics_adapter.h>
#include <simics/systemc/iface/execute_simics_adapter.h>
#include <simics/systemc/iface/execute_control_simics_adapter.h>
#include <simics/systemc/iface/frequency_simics_adapter.h>
#include <simics/systemc/iface/register_view_interface.h>
#include <simics/systemc/iface/register_view_simics_adapter.h>
#include <simics/systemc/iface/sc_gasket_info_simics_adapter.h>
#include <simics/systemc/iface/sc_memory_profiler_control_interface.h>
#include <simics/systemc/iface/sc_memory_profiler_control_simics_adapter.h>
#include <simics/systemc/iface/sc_process_profiler_control_simics_adapter.h>
#include <simics/systemc/iface/sc_version_simics_adapter.h>
#include <simics/systemc/iface/simcontext_interface.h>
#include <simics/systemc/iface/simcontext_simics_adapter.h>
#include <simics/systemc/iface/simics_adapter.h>
#include <simics/systemc/simics2tlm/io_memory_gasket_adapter.h>
#include <simics/systemc/simics2tlm/transaction_gasket_adapter.h>

#include <systemc-kernel-compiler-version-check.h>
#include <systemc-library-version-checks.h>

#if INTC_EXT && USE_SIMICS_CCI
#include <cci_utils/broker.h>
#endif

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <type_traits>
#include <utility>  // pair

namespace simics {
namespace systemc {

/**
 * Entry point for creating a Simics conf-object that adapts a SystemC model to
 * Simics, by using Simics attributes and Simics interfaces. If there is no
 * intent to communicate with Simics, please see the RegisterModel class
 * instead.
 */
class AdapterBase : public ConfObject,
                    public Simulation,
                    public SimContext,
                    public ScMemoryProfilerControl,
                    public ProcessProfilerControl,
                    public Version,
                    public GasketInfo {
  public:
    typedef AdapterBase *is_systemc_adapter;
    explicit AdapterBase(ConfObjectRef o);
    virtual ~AdapterBase();

    /**
     * Override this method to elaborate the SystemC hierarchy.
     *
     * This is necessary if the creation of the hierarchy depends on any of the
     * Simics attributes. If not, the elaboration can be done in the
     * constructor instead.
     */
    virtual void elaborate() {}

    /**
     * Override this method to create tag based gaskets.
     *
     * This is necessary if the gaskets are not compiled into the Adapter
     * integration but created as isolated Simics objects.
     */
    virtual void tagGaskets(simics::systemc::GasketTagInterface *gasket_tag) {}

    /**
     * Override this method to bind Simics interfaces to SystemC after
     * elaboration.
     *
     * This is necessary if the creation of the hierarchy depends on any of the
     * Simics attributes. If not, the elaboration and binding can be done in
     * the constructor instead.
     */
    virtual void bindGaskets() {}

    /**
     * Called during the finalize phase. Will setup a context and call
     * elaborate() followed by bindGaskets().
     *
     * Any class overriding the finalize method must call the finalize method
     * in this base class.
     *
     * NOTE: it is recommended to _not_ override finalize() in the derived
     * class, but rather use one of the elaborate() or bindGaskets() methods
     * instead.
     */
    virtual void finalize();

    template <typename C>
    static void initClassInternal(ConfClass *conf_class) {
        static iface::SimContextSimicsAdapter<C> simcontext_simics_adapter;
        conf_class->add(simcontext_simics_adapter);
        static iface::ScMemoryProfilerControlSimicsAdapter<C>
            memory_profiler_control_simics_adapter;
        conf_class->add(memory_profiler_control_simics_adapter);
        static iface::ScProcessProfilerControlSimicsAdapter<C>
            ppc_simics_adapter;
        conf_class->add(ppc_simics_adapter);
        static iface::ScVersionSimicsAdapter<C> version_simics_adapter;
        conf_class->add(version_simics_adapter);
        static iface::EventDeltaSimicsAdapter<C> event_delta_simics_adapter;
        conf_class->add(event_delta_simics_adapter);
        static iface::ExecuteSimicsAdapter<C> execute_simics_adapter;
        conf_class->add(execute_simics_adapter);
        static iface::FrequencySimicsAdapter<C> frequency_simics_adapter;
        conf_class->add(frequency_simics_adapter);
        static iface::ConcurrencyGroupSimicsAdapter<C>
            concurrency_group_simics_adapter;
        conf_class->add(concurrency_group_simics_adapter);
        static iface::ConcurrencyModeSimicsAdapter<C>
            concurrency_mode_simics_adapter;
        conf_class->add(concurrency_mode_simics_adapter);
        static iface::ExecuteControlSimicsAdapter<C>
            execute_control_simics_adapter;
        conf_class->add(execute_control_simics_adapter);
        static iface::ScGasketInfoSimicsAdapter<C> gasket_info_simics_adapter;
        conf_class->add(gasket_info_simics_adapter);

        if ((std::is_base_of<simics2tlm::IoMemoryGasketAdapter, C>::value ||
             std::is_base_of<simics2tlm::TransactionGasketAdapter, C>::value) &&
            std::is_base_of<iface::RegisterViewInterface, C>::value) {
            static iface::RegisterViewSimicsAdapter<C>
                register_view_simics_adapter;
            conf_class->add(register_view_simics_adapter);
        }

        initOnce();
        initScAttributes(conf_class);
        conf_class_t *cls = *conf_class;

        struct Invoker {
            bool operator()(iface::SimicsAdapterInterface* adapter) {
                if (adapter->map_adapter()) {
                    adapter->set_map_adapter(false);
                    adapter->set_simics_class(cls);
                }
                return false;
            }
            conf_class_t *cls;
        };

        Invoker invoker{cls};
        Registry<iface::SimicsAdapterInterface>::instance()->iterate(&invoker);

        const char *const log_groups[] = {
            "Scheduling",
            "TLM",
            "Configuration",
            "Awareness",
            "Other",
            NULL };

        SIM_log_register_groups(cls, log_groups);

        conf_class_t *clock = SIM_get_class("clock-extension");
        if (SIM_clear_exception() != SimExc_No_Exception) {
            FATAL_ERROR("Adapter::initClass: %s", SIM_last_error());
        }
        SIM_extend_class(cls, clock);

        SIM_register_notifier(cls, Sim_Notify_Frequency_Change, NULL);

        conf_class_t *co_execute = SIM_get_class("co-execute");
        if (SIM_clear_exception() != SimExc_No_Exception) {
            FATAL_ERROR("Adapter::initClass: %s", SIM_last_error());
        }
        SIM_register_port(cls, "engine", co_execute, "executor");
    }

    /** \internal Used to transform conf_object_t into Simulation */
    static SimulationInterface &simulation_from_conf_obj(conf_object_t *obj);

    /** \internal Used by awareness logic to register more proxy factories */
    static awareness::ProxyBuilder *proxy_builder() {
        return &proxy_builder_;
    }

    /** \internal */
    InternalInterface *internal() {
        return &internal_;
    }

#if INTC_EXT
    /** \internal Used by awareness logic to register more proxy factories */
    static awareness::TlmSpyFactoryRegistry *spy_factory_registry() {
        static awareness::TlmSpyFactoryRegistry registry;
        return &registry;
    }
#endif

    std::vector<simics::ConfObjectRef> gaskets_;

    /** \internal Called by Simics from Sim_Notify_Object_Delete notifier */
    virtual void deinit();

  private:
    /** \internal Attribute getters and setters */
    bool allowUnconnectedPorts() const;
    void setAllowUnconnectedPorts(const bool &val);
    bool createProxyObjects() const;
    void setCreateProxyObjects(const bool &val);
    void set_argv(const std::vector<std::string> &val);
    std::vector<std::string> get_argv() const;
    void set_cci_preset_values(
            const std::vector<std::pair<std::string, std::string>> &attr);
    simics::ConfObjectRef get_same_cell_as() const;

    conf_object_t *createObject(conf_class_t *cls,
                                std::string full_name);
    void createGaskets(const std::vector<simics::ConfObjectRef> &objs);

    static void initOnce();
    static void initScAttributes(ConfClass *conf_class);

    bool allow_unconnected_ports_;
    bool create_proxy_objects_;
#if INTC_EXT
    LateBinder late_binder_;
#endif
    static awareness::ProxyBuilder proxy_builder_;
    std::map<sc_core::sc_object *, awareness::ProxyInterface *> links_;
    awareness::Attributes attributes_;
    conf_object_t *dynamic_events_object_;
    conf_object_t *dynamic_processes_object_;
    StateHandler *state_handler_;
    void *dbg_library_;
    ScSignalAccess signal_access_;
    GasketTag gasket_tag_;
    Internal internal_;
#if INTC_EXT
    awareness::TlmSpyFactoryRegistry spy_factory_registry_;
#endif
#if INTC_EXT && USE_SIMICS_CCI
    cci_utils::broker broker_;
    conf_object_t *cci_global_;
#endif
};

class AdapterNonCheckpointable : public AdapterBase {
  public:
    using AdapterBase::AdapterBase;

    template <typename C>
    static void initClassInternal(ConfClass *conf_class) {
        AdapterBase::initClassInternal<C>(conf_class);
#ifdef MODULE_NAME
        simics::systemc::ModuleLoaded::set_module_name(MODULE_NAME);
#ifdef REGISTER_SYSTEMC_GASKET_CLASSES
        simics::systemc::initGasketClasses(MODULE_NAME);
#endif
#endif
    }
};

#if INTC_EXT && USE_SIMICS_CHECKPOINTING
class AdapterCheckpointable : public AdapterBase, public Checkpoint {
  public:
    explicit AdapterCheckpointable(ConfObjectRef o);

    template <typename C>
    static void initClassInternal(ConfClass *conf_class) {
        static iface::CheckpointSimicsAdapter<C> checkpoint_simics_adapter;
        conf_class->add(checkpoint_simics_adapter);
        static iface::TemporalStateSimicsAdapter<C> temporal_simics_adapter;
        conf_class->add(temporal_simics_adapter);
        AdapterBase::initClassInternal<C>(conf_class);
        conf_class->add(Attribute("sc_state", "[s*]",
                                  "When this attribute is set the SystemC "
                                  "state is restored from disk",
                                  ATTR_GETTER(AdapterCheckpointable,
                                              systemcState),
                                  ATTR_SETTER(AdapterCheckpointable,
                                              setSystemcState)));
#ifdef MODULE_NAME
        simics::systemc::ModuleLoaded::set_module_name(MODULE_NAME);
#ifdef REGISTER_SYSTEMC_GASKET_CLASSES
        simics::systemc::initGasketClassesCheckpointable(MODULE_NAME);
#endif
#endif
    }

    virtual void finalize();
};
using Adapter = AdapterCheckpointable;
#else
using Adapter = AdapterNonCheckpointable;
#endif

}  // namespace systemc
}  // namespace simics

#endif
