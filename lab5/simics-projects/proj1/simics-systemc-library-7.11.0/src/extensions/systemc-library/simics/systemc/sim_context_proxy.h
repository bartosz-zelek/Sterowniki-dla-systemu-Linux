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

#ifndef SIMICS_SYSTEMC_SIM_CONTEXT_PROXY_H
#define SIMICS_SYSTEMC_SIM_CONTEXT_PROXY_H



#include <simics/systemc/event.h>
#include <tlm>
#include <algorithm>    // std::sort
#include <string>
#include <vector>

// Helper class to access private methods and general useful event data of
// sc_simcontext. This relies on a patched SystemC kernel with the
// simContextProxy marked as a "friend" to get access to some of the internals.
namespace sc_core {

/** \internal */
class simContextProxy {
  public:
    typedef simics::systemc::EventVector EventVector;
    static bool is_time_less(simics::systemc::Event i,
                             simics::systemc::Event j) {
        return (i.when() < j.when());
    }

    static EventVector get_timed_event_list(sc_core::sc_object* filter);

    static int get_timed_event_list_size();
    static const char *get_typename(sc_export_base *object);
    static const char *get_typename(sc_port_base *object);
    static sc_bind_info *get_bind_info(sc_port_base *object);
    static void trigger_static(sc_process_b *object);
    static void set_simulation_status(sc_status status);
    static sc_time sc_time_to_pending_activity_ignore_async();
    static bool get_paused();
    static bool get_pause_pending();
    static void set_pause_pending(bool pause);

  private:
    static void add_events(sc_event_timed *timed_event,
                           sc_object* filter,
                           EventVector *evs);

    static void add_process_events(const sc_time &t,
                                   const char *kind,
                                   sc_object* filter,
                                   sc_object* object,
                                   EventVector *evs);
};

}  // namespace sc_core

#endif
