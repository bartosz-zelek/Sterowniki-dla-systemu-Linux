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

#include <simics/systemc/adapter.h>

#include <simics/simulator-api.h>
#include <simics/build-id.h>

#ifndef _WIN32
#include <dlfcn.h>
#endif

#include <simics/systemc/argument_registry.h>
#include <simics/systemc/awareness/connection_end_point.h>
#include <simics/systemc/awareness/proxy_name.h>
#include <simics/systemc/awareness/tlm2_base_protocol_checker.h>
#include <simics/systemc/context.h>
#include <simics/systemc/cci_configuration.h>
#include <simics/systemc/cci_global.h>
#include <simics/systemc/gasket_class_interface.h>
#include <simics/systemc/iface/sc_memory_profiler_control_interface.h>
#include <simics/systemc/internals.h>
#include <simics/systemc/memory_profiling.h>
#include <simics/systemc/null_simics_object_ref.h>
#include <simics/systemc/report_handler.h>
#include <simics/systemc/simics2systemc/null_signal.h>
#include <simics/systemc/simics2tlm/null_gasket.h>
#include <simics/systemc/systemc2simics/null_signal.h>
#include <simics/systemc/tlm2simics/gasket_dispatcher.h>
#include <simics/systemc/tlm2simics/null_gasket.h>
#include <simics/systemc/tlm2simics/null_transaction_handler.h>
#include <simics/systemc/tools/systemc_tools.h>
#include <simics/systemc/trace_event_all_dynamic.h>
#include <simics/systemc/trace_process_all_dynamic.h>
#include <simics/systemc/adapter_log_groups.h>

#include <tlm>

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace {
void callStopBeforeEndOfSimulation
    (const simics::systemc::iface::SimulationInterface *sim) {
    simics::systemc::Context ctxt(sim);
    if (!sc_core::sc_end_of_simulation_invoked()) {
        sc_core::sc_stop();
    }
}
void disableMemoryProfilingBeforeEndOfSimulation() {
    getMemoryProfiler()->stopRecording();
}

class SystemCToolsModuleLoaded : public simics::systemc::ModuleLoaded {
  public:
    virtual void loaded(const char *module_name) {
        simics::systemc::tools::initScTools(module_name);
    }
};

static SystemCToolsModuleLoaded systemc_tools_module_loaded_;

}  // namespace

extern "C" {
static void atExit(lang_void *systemc_simics_object,
                   conf_object_t * /* obj */ ) {
    disableMemoryProfilingBeforeEndOfSimulation();
    simics::systemc::Adapter *adapter
       = static_cast<simics::systemc::Adapter *>(systemc_simics_object);
    callStopBeforeEndOfSimulation(adapter);
}
}  // extern "C"

namespace {
void fatalErrorIfSomethingMissing() {
    using namespace simics::systemc;  // NOLINT(build/namespaces)
    FATAL_ERROR_IF(simics2tlm::NullGasket::instances() > 0,
                   "Missing set_gasket for simics2tlm gasket in Adapter");
    FATAL_ERROR_IF(tlm2simics::NullGasket::instances() > 0,
                   "Missing set_gasket for tlm2simics gasket in Adapter");
    // the NullTransactionHandler is always replaced by calling set_gasket,
    // so this check is probably not very useful to keep around. I.e. the user
    // is not expected to call set_transaction_handler() directly.
    FATAL_ERROR_IF(tlm2simics::NullTransactionHandler::instances() > 0,
                   "Missing set_transaction_handler for tlm2simics gasket in"
                   " Adapter");
    FATAL_ERROR_IF(simics2systemc::NullSignal::instances() > 0,
                   "Missing set_pin for simics2systemc Signal gasket in"
                   " Adapter");
    FATAL_ERROR_IF(systemc2simics::NullSignal::instances() > 0,
                   "Missing set_pin for systemc2simics Signal gasket in"
                   " Adapter");
    FATAL_ERROR_IF(NullConfObjectRef::instances() > 0,
                   "Missing ConfObjectRef assignment in Adapter");
}

}  // namespace

namespace simics {
namespace systemc {

static void deinit(conf_object_t *obj, conf_object_t *s_obj, void *arg) {
    ConfObjectRef r(obj);
    static_cast<Adapter *>(&r.as_conf_object())->deinit();
}

awareness::ProxyBuilder AdapterBase::proxy_builder_;

AdapterBase::AdapterBase(ConfObjectRef o)
    : ConfObject(o), Simulation(o, &internal_),
      SimContext(this),
      ProcessProfilerControl(this),
      GasketInfo(o),
      allow_unconnected_ports_(false),
      create_proxy_objects_(true),
      dynamic_events_object_(NULL),
      dynamic_processes_object_(NULL),
      state_handler_(NULL),
      dbg_library_(NULL),
      gasket_tag_(o),
#if INTC_EXT && USE_SIMICS_CCI
      internal_(this),
      broker_("Adapter"),
      cci_global_(NULL) {
    cci::cci_register_broker(&broker_);
#else
      internal_(this) {
#endif
    SIM_hap_add_callback("Core_At_Exit",
                         reinterpret_cast<obj_hap_func_t>(atExit), this);
    if (SIM_get_init_arg_boolean("qt-dbg", false)) {
#if !defined(_WIN32) && INTC_EXT
        // TODO(ah): right now we can use the linux-specific dlopen, but it
        //           would have been nicer to use the Simics provided
        //           os_dlopen/os_describe_last_dlerror calls here.
        dbg_library_ = dlopen("libsystemc-quickthreads-db.so",
                              RTLD_NOW | RTLD_GLOBAL);
        if (!dbg_library_) {
            // For some reason, we sometimes fail to load the QT support module
            // unless LD_LIBRARY_PATH has been specified. Try once more, but
            // with a project/package relative path
            char *soname = SIM_lookup_file(
                    "%simics%/linux64/lib/libsystemc-quickthreads-db.so");
            if (soname) {
                dbg_library_ = dlopen(soname, RTLD_NOW | RTLD_GLOBAL);
                MM_FREE(soname);
            }
        }
        if (!dbg_library_) {
            SIM_LOG_ERROR(obj(), Log_Configuration,
                          "Unable to load libsystemc-quickthreads-db.so: %s",
                          dlerror());
        }
#endif
    }

    // Some action like sc_stop() is needed before deleting SystemC objects
    SIM_add_notifier(o, Sim_Notify_Object_Delete, o, simics::systemc::deinit,
                     NULL);
}

AdapterBase::~AdapterBase() {
    Context ctxt(this);
    for (auto &link : links_) {
        awareness::ProxyInterface *p = link.second;
        if (p)
            p->simulationEnded();
    }

    delete state_handler_;
#if !defined(_WIN32) && INTC_EXT
    if (dbg_library_)
        dlclose(dbg_library_);
#endif
#if INTC_EXT && USE_SIMICS_CCI
    cci::cci_unregister_brokers();
#endif
    CciConfiguration::cleanCache();
    tlm2simics::GasketDispatcherBase::cleanCache();

    // If attribute assignment fails neither finalize nor pre_delete are called.
    SIM_hap_delete_callback("Core_At_Exit",
                            reinterpret_cast<obj_hap_func_t>(atExit), this);
}

/** Returns a conf_object if successful, NULL on error */
conf_object_t *AdapterBase::createObject(conf_class_t *cls,
                                     std::string full_name) {
    attr_value_t attrs = SIM_make_attr_list(0);
    conf_object_t *object = SIM_create_object(cls, full_name.c_str(), attrs);
    SIM_attr_free(&attrs);
    if (SIM_clear_exception() != SimExc_No_Exception) {
        return NULL;
    }

    return object;
}

void AdapterBase::createGaskets(
        const std::vector<simics::ConfObjectRef> &objs) {
    SIM_set_object_configured(obj());
    for (std::vector<ConfObjectRef>::const_iterator i = objs.begin();
         i != objs.end(); ++i) {
        i->require();
        ConfObject *o = static_cast<ConfObject *>(SIM_object_data(*i));
        GasketClassInterface *gasket = dynamic_cast<GasketClassInterface *>(o);
        if (!gasket) {
            SIM_LOG_ERROR(obj(), Log_Configuration,
                          "Object %s does not implement GasketClassInterface",
                          i->name().c_str());
            continue;
        }

        if (*this != *gasket->version()) {
            SIM_LOG_ERROR(obj(), Log_Configuration,
                          "Incompatible gasket (%s) connected; "
                          "version mismatch",
                          i->name().c_str());
            continue;
        }

        gasket->createGasket(this);
    }
}

namespace {
static bool init_once = false;
}

void AdapterBase::finalize() {
    Context ctxt(this);
    elaborate();

#if INTC_EXT && USE_SIMICS_CCI
    CciConfiguration configuration;
    cci_global_ = createObject(CciGlobal::initClass(
                                       ModuleLoaded::module_name(), obj()),
                               obj().name() + ".cci_global");
    FATAL_ERROR_IF(!cci_global_,
                   "Unable to create CCI global SystemC event Object");
    static_cast<CciGlobal *>(SIM_object_data(cci_global_))->init(&attributes_);

    configuration.logUnconsumedPresetValues(obj());
#endif

    // Create gaskets of conf objects
    createGaskets(gaskets_);

    // Create gaskets of conf objects based on tagged information.
    if (!SIM_is_restoring_state(obj())) {
        tagGaskets(&gasket_tag_);
        std::vector<ConfObjectRef> tagged_gasket_objects
            = gasket_tag_.createGasketObjects();
        createGaskets(tagged_gasket_objects);
        // Extend 'gasket_list' attribute with tagged gaskets so they are
        // re-created when restoring from checkpoint
        gaskets_.insert(gaskets_.end(),
                        tagged_gasket_objects.begin(),
                        tagged_gasket_objects.end());
    }

    bindGaskets();
    fatalErrorIfSomethingMissing();
    awareness::ProxyName::set_error_prefix_name("renamed.");

    proxy_builder_.suppressProxyBuild(sc_core::sc_find_object("yield_request"));
    if (create_proxy_objects_) {
        proxy_builder_.buildProxies(obj(), this, &links_);
    }
#if INTC_EXT
    spy_factory_registry_ = *spy_factory_registry();
    spy_factory_registry_.traverseAll();
#else
    SIM_LOG_INFO(1, obj(), 0,
                 "Kernel not built with INTC_EXT=1; many features including"
                 " tracing/breaking/profiling/checkpointing and the ability to"
                 " instantiate multiple instances will not be available");
#endif

    // Create these objects _after_ the traversal so that these internal
    // utility objects does not end up as proxy objects
    state_handler_ = new StateHandler();

    proxy_builder_.suppressProxyBuild(
        sc_core::sc_find_object("eventNotification"));
    proxy_builder_.suppressProxyBuild(
        sc_core::sc_find_object("state_handler"));

    dynamic_events_object_ = createObject(TraceEventAllDynamic::initClass(),
        obj().name() + ".sc_event_all_dynamic");
    FATAL_ERROR_IF(!dynamic_events_object_,
                   "Unable to create dynamic SystemC event Object");
    static_cast<TraceEventAllDynamic *>(
        SIM_object_data(dynamic_events_object_))->init(this);

    dynamic_processes_object_ = createObject(
        TraceProcessAllDynamic::initClass(),
        obj().name() + ".sc_process_all_dynamic");
    FATAL_ERROR_IF(!dynamic_processes_object_,
                   "Unable to create dynamic SystemC process Object");
    static_cast<TraceProcessAllDynamic *>(
        SIM_object_data(dynamic_processes_object_))->init(this);

    if (allow_unconnected_ports_) {
#if INTC_EXT
        sc_core::sc_get_curr_simcontext()->set_late_binder(&late_binder_);
#endif
    }

    Simulation::finalize();

    // Build proxies for objects created between the spy_factory_registry() and
    // finalize().
    if (create_proxy_objects_) {
        proxy_builder_.buildProxies(obj(), this, &links_);
        proxy_builder_.setProxyAttributes(&links_, &attributes_);
    }

    for (auto &link : links_) {
        awareness::ProxyInterface *p = link.second;
        if (p)
            p->simulationStarted();
    }
}

void AdapterBase::deinit() {
    links_.clear();

    SIM_hap_delete_callback("Core_At_Exit",
                            reinterpret_cast<obj_hap_func_t>(atExit), this);
    // Since the atExit callback is no longer registered:
    disableMemoryProfilingBeforeEndOfSimulation();
    callStopBeforeEndOfSimulation(this);
}

bool AdapterBase::allowUnconnectedPorts() const {
    return allow_unconnected_ports_;
}

void AdapterBase::setAllowUnconnectedPorts(const bool &val) {
    // Setting this attribute _after_ elaboration has no effect, but I don't
    // see why we should produce an error or even a warning/info about this
    allow_unconnected_ports_ = val;
}

bool AdapterBase::createProxyObjects() const {
    return create_proxy_objects_;
}

void AdapterBase::setCreateProxyObjects(const bool &val) {
    // Setting this attribute _after_ elaboration has no effect, but I don't
    // see why we should produce an error or even a warning/info about this
    create_proxy_objects_ = val;
}

void AdapterBase::set_argv(const std::vector<std::string> &val) {
    Context ctxt(this);

    if (SIM_object_is_configured(obj())
        && !SIM_is_restoring_state(obj())) {
        const char *msg = ("The argument vector may not be changed once"
                           " the object has been instantiated.");
        throw std::runtime_error(msg);
    }

    std::vector<std::string> argv {"simics"};
    argv.insert(argv.end(), val.cbegin(), val.cend());

    ArgumentRegistry::Instance()->set_argv(argv);
}

std::vector<std::string> AdapterBase::get_argv() const {
    Context ctxt(this);
    int argc = (std::max)(ArgumentRegistry::Instance()->argc() - 1, 0);
    const char *const *argv = ArgumentRegistry::Instance()->argv();

    std::vector<std::string> v;
    for (int i = 0; i < argc; ++i) {
        v.push_back(argv[i + 1]);
    }
    return v;
}

void AdapterBase::set_cci_preset_values(
        const std::vector<std::pair<std::string, std::string>> &val) {
    Context ctxt(this);

    // Valid only for instantiating
    if (SIM_object_is_configured(obj())
        && !SIM_is_restoring_state(obj())) {
        const char *msg = ("The cci_preset_values may not be changed once"
                           " the object has been instantiated.");
        throw std::runtime_error(msg);
    }

    CciConfiguration configuration;
    configuration.setPresetValues(&val);
}

simics::ConfObjectRef AdapterBase::get_same_cell_as() const {
    std::vector<conf_object_t *> vec_objs = instances();
    FATAL_ERROR_IF(vec_objs.empty(), "No simulation instances are tracked");
    return vec_objs[0];
}

/** simulation_from_conf_obj can only be called after finalize, when the
    instance_data has been set by Simics */
iface::SimulationInterface &AdapterBase::simulation_from_conf_obj(
        conf_object_t *obj) {
    return *static_cast<Adapter *>(SIM_object_data(obj));
}

void AdapterBase::initOnce() {
    if (!init_once) {
        ReportHandler::init();
        init_once = true;
    }
}

void AdapterBase::initScAttributes(ConfClass *conf_class) {
    conf_class->add(Attribute("allow_unconnected_ports", "b",
                              "This attribute is used for allowing ports to be"
                              " left unconnected/unbound after elaboration."
                              " They will then automatically be bound to a"
                              " dedicated object that will log whenever the"
                              " port it used.",
                              ATTR_GETTER(Adapter, allowUnconnectedPorts),
                              ATTR_SETTER(Adapter, setAllowUnconnectedPorts)));
    conf_class->add(Attribute("create_proxy_objects", "b",
                              "This attribute is used to enable (=default) or"
                              " disable the creation of SystemC proxy objects."
                              " Without proxy objects the user cannot interact"
                              " with the SystemC objects to enable tracing, set"
                              " breakpoints, collect profiling data, etc.",
                              ATTR_GETTER(Adapter, createProxyObjects),
                              ATTR_SETTER(Adapter, setCreateProxyObjects)));
    conf_class->add(Attribute("argv", "[s*]",
                              "Argument vector to pass to SystemC setup",
                              ATTR_GETTER(Adapter, get_argv),
                              ATTR_SETTER(Adapter, set_argv)));
    conf_class->add(Attribute("gasket_list", "[o*]",
                              "The connected gaskets.",
                              ATTR_CLS_VAR(Adapter, gaskets_)));
    conf_class->add(Attribute("run_next_delta", "i",
                              "Run the next delta cycle by invoking"
                              " sc_start(0). Multiple delta cycles can be run"
                              " at once by setting a value greater than one.",
                              nullptr,
                              ATTR_SETTER(Adapter, set_run_next_delta)));
    conf_class->add(Attribute("cci_preset_values", "[[ss]*]",
                              "List containing key-value tuples of CCI preset"
                              " values.",
                              nullptr,
                              ATTR_SETTER(Adapter, set_cci_preset_values)));
    conf_class->add(Attribute("same_cell_as", "o",
                              "This adapter should have same cell as the"
                              " object assigned. For internal use only.",
                              ATTR_GETTER(Adapter, get_same_cell_as),
                              nullptr));
}

}  // namespace systemc
}  // namespace simics
