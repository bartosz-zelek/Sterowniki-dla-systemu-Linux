// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2019 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/base/configuration.h>
#include <simics/systemc/thread_pool.h>
#include <simics/systemc/sim_context_proxy.h>
#include <simics/systemc/kernel_state_modifier.h>

#include <sysc/kernel/sc_thread_process.h>
#include <sysc/kernel/sc_simcontext_int.h>

namespace simics {
namespace systemc {

Thread::Thread()
    : cb_(NULL),
      thread_process_(NULL),
      wait_for_work_(sc_core::sc_event::kernel_event, "wait_for_work"),
      finished_(true) {
}

void Thread::spawn() {
    sc_core::sc_spawn_options opts;
    opts.dont_initialize();
    opts.set_sensitivity(&wait_for_work_);

    process_handle_ = sc_core::sc_spawn(
        sc_bind(&Thread::thread_function, this), thread_name(), &opts);

    thread_process_ = dynamic_cast<sc_core::sc_thread_process *>(
        static_cast<sc_core::sc_process_b *>(process_handle_));
    ASSERT(thread_process_ != NULL);
}

ThreadInterface::CallReturn Thread::call(ThreadCallbackInterface *cb) {
    if (finished_ == false)
        return CALL_RETURN_ERROR_IN_USE;

    cb_ = cb;

    {
        KernelStateModifier m(cb_->simulation(this));
        if (sc_core::sc_get_status() == sc_core::SC_RUNNING)
            wait_for_work_.notify();
        else
            wait_for_work_.notify(sc_core::SC_ZERO_TIME);
    }

    if (sc_core::sc_get_status() == sc_core::SC_PAUSED)
        cb_->simulation(this)->runDeltaPhase(0);

    if (process_handle_.terminated())
        return CALL_RETURN_TERMINATED;

    if (!finished_)
        cb_->block(this);

    return finished_ ? CALL_RETURN_FINISHED : CALL_RETURN_WAITING;
}

const char *Thread::thread_name() {
    return 0;
}

void Thread::thread_function() {
    while (true) {
        try {
            if (!SIM_is_restoring_state(
                    cb_->simulation(this)->simics_object())) {
                finished_ = false;
                cb_->run(this);
                finished_ = true;
                cb_->finish(this);
            }
            sc_core::wait();
        } catch(const sc_core::sc_unwind_exception &e) {
            if (SIM_is_restoring_state(
                    cb_->simulation(this)->simics_object())) {
                // Checkpoint restore - Thread reset
                throw;
            }
        } catch(...) {
            cb_->exception(this);
            return;
        }
    }
}

}  // namespace systemc
}  // namespace simics
