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

#ifndef SIMICS_SYSTEMC_THREAD_POOL_H
#define SIMICS_SYSTEMC_THREAD_POOL_H

#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/kernel_state_modifier.h>

#include <systemc>

#include <queue>
#include <unordered_map>

namespace simics {
namespace systemc {

class ThreadInterface;

/** \internal */
class ThreadCallbackInterface {
  public:
    virtual ~ThreadCallbackInterface() {}
    virtual void run(ThreadInterface *call) = 0;
    virtual void block(ThreadInterface *call) = 0;
    virtual void finish(ThreadInterface *call) = 0;
    virtual void exception(ThreadInterface *call) = 0;
    virtual iface::SimulationInterface *simulation(ThreadInterface *call) = 0;
};

/** \internal */
class ThreadInterface {
  public:
    enum CallReturn {
        CALL_RETURN_FINISHED,
        CALL_RETURN_WAITING,
        CALL_RETURN_TERMINATED,
        CALL_RETURN_ERROR_IN_USE
    };
    virtual ~ThreadInterface() {}
    virtual CallReturn call(ThreadCallbackInterface *cb) = 0;
};

/** \internal */
class Thread : public ThreadInterface {
  public:
    Thread();
    void spawn();
    virtual CallReturn call(ThreadCallbackInterface *cb);

  protected:
    virtual const char *thread_name();

  private:
    void thread_function();

    ThreadCallbackInterface *cb_;
    sc_core::sc_process_handle process_handle_;
    sc_core::sc_thread_process *thread_process_;
    sc_core::sc_event wait_for_work_;
    bool finished_;
};

/** \internal */
template<class TThread = Thread>
class ThreadPool : public ThreadInterface, private ThreadCallbackInterface {
  public:
    // ThreadInterface
    virtual CallReturn call(ThreadCallbackInterface *cb) {
        if (idle_.empty()) {
            KernelStateModifier m(cb->simulation(this));
            allocateThread();
        }

        ThreadInterface *c = idle_.front();
        idle_.pop();
        active_[c] = cb;
        return c->call(this);
    }

  private:
    void allocateThread() {
        TThread *t = new TThread;
        t->spawn();
        idle_.push(t);
    }
    // ThreadCallbackInterface
    virtual void run(ThreadInterface *call) {
        active_[call]->run(call);
    }
    virtual void block(ThreadInterface *call) {
        active_[call]->block(call);
    }
    virtual void finish(ThreadInterface *call) {
        auto it = active_.find(call);
        ASSERT(it != active_.end());
        it->second->finish(call);
        // A micro benchmark (10M MMIO read/write) shows the erase
        // from the map affects performance (>7%)
        it->second = nullptr;
        idle_.push(call);
    }
    virtual void exception(ThreadInterface *call) {
        if (active_[call])
            active_[call]->exception(call);
    }
    virtual iface::SimulationInterface *simulation(ThreadInterface *call) {
        return active_[call]->simulation(call);
    }

    std::queue<ThreadInterface *> idle_;
    std::unordered_map<ThreadInterface *, ThreadCallbackInterface *> active_;
};

}  // namespace systemc
}  // namespace simics

#endif
