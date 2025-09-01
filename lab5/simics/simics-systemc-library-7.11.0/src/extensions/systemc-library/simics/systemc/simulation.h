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

#ifndef SIMICS_SYSTEMC_SIMULATION_H
#define SIMICS_SYSTEMC_SIMULATION_H

#include <systemc>

#include <simics/base/iface-call.h>

#include <simics/devs/frequency.h>
#include <simics/model-iface/event-delta.h>
#include <simics/model-iface/co-execute.h>
#include <simics/systemc/iface/concurrency_group_interface.h>
#include <simics/systemc/iface/concurrency_mode_interface.h>
#include <simics/systemc/iface/event_delta_interface.h>
#include <simics/systemc/iface/execute_control_interface.h>
#include <simics/systemc/iface/execute_interface.h>
#include <simics/systemc/iface/frequency_interface.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/internal_interface.h>
#include <simics/systemc/kernel.h>
#include <simics/systemc/simulation_deleter.h>

#include <vector>

namespace simics {
namespace systemc {

/**
 * Provides the interface to the SystemC kernel.
 * Handles running the SystemC scheduler on entry from Simics.
 * The class also maintains the simulation context, making it possible to
 * create multiple instances of the same module. */
class Simulation
    : public iface::SimulationInterface,
      public iface::EventDeltaInterface,
      public iface::ExecuteInterface,
      public iface::FrequencyInterface,
      public iface::ConcurrencyGroupInterface,
      public iface::ConcurrencyModeInterface,
      public iface::ExecuteControlInterface {
  public:
    Simulation(ConfObjectRef o, InternalInterface *internal);
    virtual ~Simulation();

    // ExecuteInterface
    virtual void run();
    virtual void stop();
    virtual void switch_in();
    virtual void switch_out();

    // EventDeltaInterface
    virtual uint64 set_delta(conf_object_t *NOTNULL event_handler_obj,
                             const event_class_t *next_event_ec,
                             uint64 delta);
    virtual uint64 get_delta(conf_object_t *NOTNULL event_handler_obj);

    // FrequencyInterface
    virtual double get() const;

    // ConcurrencyGroupInterface
    virtual std::vector<conf_object_t *> serialized_memory_group(
            unsigned group_index);
    virtual std::vector<conf_object_t *> execution_group(unsigned group_index);

    // ConcurrencyModeInterface
    virtual concurrency_mode_t supported_modes();
    virtual concurrency_mode_t current_mode();
    virtual void switch_mode(concurrency_mode_t mode);

    // ExecuteControlInterface
    virtual void message_pending();
    virtual void yield_request();

    // SimulationInterface
    int runDeltaPhase(int count); /* override */
    bool runSimulation(sc_core::sc_time t); /* override */
    void stopSimulation(); /* override */
    sc_core::sc_simcontext* context() const; /* override */
    ConfObjectRef simics_object() const; /* override */

    void set_run_next_delta(const int &count);

  protected:
    // methods called from Adapter
    void finalize();

    virtual void callHandleEvent();

    const std::vector<conf_object_t*>& instances() const;

  private:
    int runDeltaPhaseNoStateCheck(int count);
    bool isPausePending();
    bool hasPendingAsyncActivity() const;
    bool runSimulationNoCallHandleEvent(sc_core::sc_time t);

    static void atSimulationStopped(lang_void *simulation, conf_object_t *obj);
    static std::vector<conf_object_t*> instancesNotInCell(conf_object_t *cell);
    static void onCellChange(lang_void *callback_data, conf_object_t *obj,
                             conf_object_t *old_cell, conf_object_t *new_cell);

    // Helper to group all instances in the same TD
    struct InstanceTracker {
        explicit InstanceTracker(conf_object_t *NOTNULL object);
        virtual ~InstanceTracker();
        conf_object_t *object_;
    };
    static std::vector<conf_object_t*> instances_;

    class YieldRequest : public sc_core::sc_prim_channel {
      public:
        YieldRequest(conf_object_t *engine, const char* name)
            : sc_core::sc_prim_channel(name) {
            INIT_IFACE(&execute_, execute, engine);
        }
        virtual void update(void) {
            if (execute_.obj)
                CALL(execute_, stop)();
        }
        IFACE_OBJ(execute) execute_;
    };

    ConfObjectRef simics_object_;
    SimulationDeleter *simulation_deleter_;
    Kernel kernel_;
    uint64 delta_;
    sc_core::sc_time delta_time_;
    IFACE_OBJ(event_handler) event_handler_;
    concurrency_mode_t concurrency_mode_;
    YieldRequest yield_request_;
    bool break_pending_;
    InternalInterface *internal_;
    InstanceTracker instance_tracker_;
};

}  // namespace systemc
}  // namespace simics

#endif
