// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/adapter_log_groups.h>
#include <simics/systemc/context.h>
#include <simics/systemc/internals.h>
#include <simics/systemc/kernel_state_modifier.h>
#include <simics/systemc/simulation.h>
#include <simics/systemc/sim_context_proxy.h>

#include <simics/simulator/conf-object.h>
#include <simics/simulator-api.h>

#include <exception>
#include <sstream>
#include <vector>

namespace simics {
namespace systemc {

using sc_core::sc_pending_activity_at_current_time;
using sc_core::sc_time;
using sc_core::sc_time_stamp;
using sc_core::sc_start;

std::vector<conf_object_t*> Simulation::instances_ {};

Simulation::Simulation(ConfObjectRef o, InternalInterface *internal)
    : simics_object_(o),
      // coverity[ctor_dtor_leak], destroyed by Simics HAP
      simulation_deleter_(new SimulationDeleter(o)),
      kernel_(simulation_deleter_->context()),
      delta_(0),
      concurrency_mode_(Sim_Concurrency_Mode_Serialized_Memory),
      yield_request_(SIM_object_descendant(o, "engine"), "yield_request"),
      break_pending_(false), internal_(internal),
      instance_tracker_(o) {
    // will be fully initialized by finalize()
    INIT_IFACE(&event_handler_, event_handler, 0);

    sc_core::sc_allow_process_control_corners = true;

    // disable the direct schedule by the Simics scheduler. It is
    // scheduled via co-execute instead
    SIM_set_attribute_default(simics_object_, "do_not_schedule",
                              SIM_make_attr_boolean(true));

    SIM_hap_add_callback("Core_Simulation_Stopped",
                         reinterpret_cast<obj_hap_func_t>(atSimulationStopped),
                         this);
}

Simulation::~Simulation() {
    SIM_hap_delete_callback(
            "Core_Simulation_Stopped",
            reinterpret_cast<obj_hap_func_t>(atSimulationStopped),
            this);
}

void Simulation::finalize() {
    conf_object_t *vtime = SIM_object_descendant(simics_object_, "vtime");
    INIT_IFACE(&event_handler_, event_handler, vtime);

    // Initialize SC world
    KernelStateModifier modifier(this);
    try {
        context()->initialize(true);  // no initial crunch!
    } catch(const sc_core::sc_report &e) {
        // The SC kernel transforms all exception types to sc_report object,
        // thus there are no other catch blocks here
        SIM_LOG_CRITICAL(simics_object_, Log_Configuration,
                         "exception caught in finalize(): %s", e.what());
        stopSimulation();
    }

    // Listen to cell changes
    SIM_hap_add_callback_obj("Core_Conf_Clock_Change_Cell", simics_object_, 0,
                             reinterpret_cast<obj_hap_func_t>(onCellChange),
                             NULL);
    conf_object_t *engine = SIM_object_descendant(simics_object_, "engine");
    SIM_hap_add_callback_obj("Core_Conf_Clock_Change_Cell", engine, 0,
                             reinterpret_cast<obj_hap_func_t>(onCellChange),
                             NULL);
}

uint64 Simulation::set_delta(conf_object_t *NOTNULL event_handler_obj,
                             const event_class_t *next_event_ec,
                             uint64 delta) {
    Context ctxt(this);
    if (delta < delta_)
        internal_->pauseSimulationNoBreak();

    delta_ = delta;
    delta_time_ = sc_core::sc_time_stamp();
    return delta_;
}

uint64 Simulation::get_delta(conf_object_t *NOTNULL event_handler_obj) {
    Context ctxt(this);
    uint64 dt = (sc_core::sc_time_stamp() - delta_time_).value();
    return delta_ > dt ? (delta_ - dt) : 0;
}

void Simulation::run() {
    Context ctxt(this);
    if (sc_core::sc_get_status() == sc_core::SC_STOPPED) {
        callHandleEvent();
        return;
    }

    uint64 dt = get_delta(NULL);
    runSimulationNoCallHandleEvent(sc_core::sc_time::from_value(dt));
    internal_->process_stack()->runUntilStackEmpty();

    // callHandleEvent() may release the lock held by the surrounding
    // co-execute box. From here, other threads may re-enter run() again.
    if (get_delta(NULL) == 0)
        callHandleEvent();
}

int Simulation::runDeltaPhaseNoStateCheck(int count) {
    unsigned int delta_count = 0;
    SIM_LOG_INFO(4, simics_object_, Log_Scheduling, "running delta cycles");
    try {
        while (sc_pending_activity_at_current_time()
               && sc_core::sc_get_status() == sc_core::SC_PAUSED) {
            sc_start(sc_core::SC_ZERO_TIME);
            if (isPausePending())
               break;

            ++delta_count;
            if (delta_count > 10000000) {
                SIM_LOG_ERROR(simics_object_, 0, "More than 10 million delta"
                              " cycles within a single cycle detected. This"
                              " indicates potential problems in the SystemC"
                              " model.");
                break;
            }
            if (--count == 0)
                break;
        }
    } catch(const sc_core::sc_report &e) {
        // The SC kernel transforms all exception types to sc_report object,
        // thus there are no other catch blocks here
        SIM_LOG_CRITICAL(simics_object_, Log_Scheduling,
                         "exception caught in runDeltaPhase(): %s",
                         e.what());
        stopSimulation();
        // log before exit
    }
    SIM_LOG_INFO(4, simics_object_, Log_Scheduling,
                 "finished running delta cycles");
    return delta_count;
}

int Simulation::runDeltaPhase(int count) {
    KernelStateModifier modifier(this);
    return runDeltaPhaseNoStateCheck(count);
}

bool Simulation::runSimulationNoCallHandleEvent(sc_core::sc_time t) {
    bool succeeded = true;
    if (sc_core::sc_get_status() != sc_core::SC_PAUSED)
        return false;

    KernelStateModifier modifier(this);
    uint64 dt = get_delta(NULL);
    if (dt != 0) {
        sc_core::sc_time run_to =
                dt < t.value() ? sc_core::sc_time::from_value(dt) : t;
        SIM_LOG_INFO(4, simics_object_, Log_Scheduling,
                     "running sc_start for: %lli ps. delta: %lli",
                     run_to.value(), dt);
        try {
            // avoid no activity or clock movement for sc_start() invocation
            if (run_to != sc_core::SC_ZERO_TIME)
                sc_start(run_to);

            if (!isPausePending() || hasPendingAsyncActivity()) {
                runDeltaPhaseNoStateCheck(0);
            }
        } catch(const sc_core::sc_report &e) {
            // The SC kernel transforms all exception types to sc_report object,
            // thus there are no other catch blocks here
            SIM_LOG_CRITICAL(simics_object_, Log_Scheduling,
                             "exception caught in runSimulation(): %s",
                             e.what());
            stopSimulation();
            callHandleEvent();
            succeeded = false;
        }
        SIM_LOG_INFO(4, simics_object_, Log_Scheduling,
                     "finished running sc_start new delta: %lli",
                     get_delta(NULL));
    }

    return succeeded;
}

bool Simulation::runSimulation(sc_core::sc_time t) {
    bool ret = runSimulationNoCallHandleEvent(t);
    if (get_delta(NULL) == 0)
        callHandleEvent();

    return ret;
}

void Simulation::stopSimulation() {
    if (sc_core::sc_is_running())
        sc_core::sc_stop();

    if (sc_core::sc_get_status() == sc_core::SC_RUNNING)
        sc_core::simContextProxy::set_simulation_status(sc_core::SC_STOPPED);
}

void Simulation::stop() {
    Context ctxt(this);
    internal_->pauseSimulationNoBreak();
    if (event_handler_.obj)
        CALL(event_handler_, stop)();
}

void Simulation::switch_in() {
}

void Simulation::switch_out() {
}

double Simulation::get() const {
    return 1e12;
}

concurrency_mode_t Simulation::supported_modes() {
#ifdef SYSTEMC_SERIAL_TD
    return Sim_Concurrency_Mode_Serialized;
#else
    return static_cast<concurrency_mode_t>(
            Sim_Concurrency_Mode_Serialized_Memory |
            Sim_Concurrency_Mode_Serialized);
#endif
}

concurrency_mode_t Simulation::current_mode() {
    return concurrency_mode_;
}

void Simulation::switch_mode(concurrency_mode_t mode) {
    if (!(Simulation::supported_modes() & mode)) {
        SIM_LOG_CRITICAL(simics_object_, Log_Configuration,
                         "mode %i not supported", mode);
    }
    concurrency_mode_ = mode;
}

std::vector<conf_object_t *> Simulation::serialized_memory_group(
        unsigned group_index) {
    return {};
}

std::vector<conf_object_t *> Simulation::execution_group(unsigned group_index) {
    if (group_index != 0) {
        return {};
    } else {
        return Simulation::instances_;
    }
}

// Following two methods belongs to the execute_control interface
// handling async calls. Message_pending is called when a direct memory
// message is pending to be delivered to this thread domain. Yield_request
// is called when a different thread wants to acquire this thread domain.
// Both calls will notify the SystemC kernel and call stop eventually.
// NOTE: there is no need to acquire the Context here as the primary channel
// updates the correct "pending queue" through internal pointer. There is also
// no need to check if simulation is stopped as this is tested elsewhere.
void Simulation::message_pending() {
    yield_request_.async_request_update();
}
void Simulation::yield_request() {
    yield_request_.async_request_update();
}

sc_core::sc_simcontext *Simulation::context() const {
    return simulation_deleter_->context();
}

ConfObjectRef Simulation::simics_object() const {
    return simics_object_;
}

bool Simulation::isPausePending() {
    bool pending = sc_core::simContextProxy::get_paused();

    if (pending && !break_pending_) {
        SIM_LOG_INFO(4, simics_object_, Log_Scheduling,
                     "sc_pause() intercepted, calling SIM_break_simulation()");
        SIM_break_simulation("SystemC sc_pause intercepted");

        // If SystemC is run from CLI without running Simics, the hap for
        // simulation stopped will not be invoked. The flag break_pending_ will
        // not be set for this situations to avoid suppression of logging when
        // Simics is in running state and sc_pause would be invoked the next
        // time.
        if (SIM_simics_is_running())
            break_pending_ = true;
    }

    return pending;
}

bool Simulation::hasPendingAsyncActivity() const {
    return context()->get_prim_channel_registry()->pending_async_updates();
}

void Simulation::atSimulationStopped(lang_void *simulation,
                                     conf_object_t *obj) {
    Simulation *that = static_cast<Simulation *>(simulation);
    that->break_pending_ = false;
}

void Simulation::set_run_next_delta(const int &count) {
    // Set_run_next_delta is invoked by setting the value of the attribute
    // run_next_delta on the adapter. The context has not been set before.
    Context ctxt(this);
    runDeltaPhase(count);
}

void Simulation::callHandleEvent() {
    if (event_handler_.obj)
        CALL(event_handler_, handle_event)();
}

const std::vector<conf_object_t*>& Simulation::instances() const {
    return Simulation::instances_;
}

Simulation::InstanceTracker::InstanceTracker(conf_object_t *NOTNULL object)
    : object_(object) {
    conf_object_t *engine = SIM_object_descendant(object, "engine");
    if (!engine) {
        SIM_LOG_CRITICAL(object, Log_Configuration,
                         "engine not available");
    } else {
        Simulation::instances_.push_back(object);
        Simulation::instances_.push_back(engine);
    }
}

Simulation::InstanceTracker::~InstanceTracker() {
    auto toErase = std::find(Simulation::instances_.begin(),
                             Simulation::instances_.end(),
                             object_);
    if (toErase != Simulation::instances_.end()) {
        Simulation::instances_.erase(toErase);
        // Remove the engine object next to simulation object
        Simulation::instances_.erase(toErase);
    }
}

std::vector<conf_object_t*>
Simulation::instancesNotInCell(conf_object_t *cell) {
    std::vector<conf_object_t*> instances;
    for (auto i : Simulation::instances_) {
        attr_value_t val = SIM_get_attribute(i, "cell");
        if (!SIM_attr_is_nil(val) && SIM_attr_object(val) != cell) {
            instances.push_back(i);
        }
    }
    return instances;
}

void Simulation::onCellChange(lang_void *callback_data, conf_object_t *obj,
                              conf_object_t *old_cell,
                              conf_object_t *new_cell) {
    const std::vector<conf_object_t*>& instances = \
        Simulation::instancesNotInCell(new_cell);
    if (!instances.empty()) {
        std::ostringstream os;
        os << SIM_object_name(obj) << " is not placed in the same cell with [";
        for (auto i : instances) {
            os << SIM_object_name(i) << ", ";
        }
        os << "]. The simulation may run into errors or even segfault in"
           << " multi-threading mode.";
        SIM_LOG_ERROR(obj, Log_Scheduling, "%s", os.str().c_str());
    }
}

}  // namespace systemc
}  // namespace simics
