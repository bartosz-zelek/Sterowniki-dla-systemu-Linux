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

#include <simics/systemc/sim_context_proxy.h>
#if INTC_EXT
#include <sysc/kernel/sc_method_process.h>
#include <sysc/kernel/sc_runnable_int.h>
#include <sysc/kernel/sc_simcontext_int.h>
#include <sysc/kernel/sc_thread_process.h>
#endif

#include <algorithm>
#include <string>
#include <vector>

using simics::systemc::EventVector;
using simics::systemc::Event;

namespace sc_core {

// The filter parameter expects a sc_process_b* as parameter. The
// parameter can be NULL, which means that a non-process event (i.e. an
// sc_event) has been processed.
// returns an events list string with the format (includes all timed events)
// time_of_event_#1;name_of_module_waiting_#1;type_of_module_waiting_#1;
// time_of_event_#2;name_of_module_waiting_#2;type_of_module_waiting_#2;


EventVector simContextProxy::get_timed_event_list(sc_object* filter) {
    EventVector evs;
#if INTC_EXT
    sc_ppq<sc_event_timed*> &timed_events
        = *sc_get_curr_simcontext()->m_timed_events;
    int size = timed_events.size();

    for (int i = 0; i < size; ++i) {
        // Peek into the priority event queue, using friend declaration
        sc_event_timed *e = static_cast<sc_event_timed *>(
                static_cast<sc_ppq_base*>(&timed_events)->m_heap[i + 1]);
        add_events(e, filter, &evs);
    }

    std::sort(evs.begin(), evs.end(), is_time_less);
#endif
    return evs;
}

int simContextProxy::get_timed_event_list_size() {
#if INTC_EXT
    sc_ppq<sc_event_timed*> &timed_events
        = *sc_get_curr_simcontext()->m_timed_events;
    return timed_events.size();
#else
    return 0;
#endif
}

const char *simContextProxy::get_typename(sc_export_base *object) {
#if INTC_EXT
    return object->if_typename();
#else
    return "";
#endif
}

const char *simContextProxy::get_typename(sc_port_base *object) {
#if INTC_EXT
    return object->if_typename();
#else
    return "";
#endif
}

sc_bind_info *simContextProxy::get_bind_info(sc_port_base *object) {
#if INTC_EXT
    return object->m_bind_info;
#else
    return NULL;
#endif
}

void simContextProxy::trigger_static(sc_process_b *object) {
#if INTC_EXT
    if (sc_thread_process *thread = dynamic_cast<sc_thread_process *>(object))
        thread->trigger_static();

    if (sc_method_process *method = dynamic_cast<sc_method_process *>(object))
        method->trigger_static();
#endif
}

void simContextProxy::set_simulation_status(sc_status status) {
#if INTC_EXT
    sc_get_curr_simcontext()->m_simulation_status = status;
#endif
}

sc_time simContextProxy::sc_time_to_pending_activity_ignore_async() {
#if INTC_EXT
    sc_simcontext *s = sc_get_curr_simcontext();

    if (s->m_delta_events.size() != 0)
        return SC_ZERO_TIME;

    if (s->m_runnable->is_initialized() && !s->m_runnable->is_empty())
        return SC_ZERO_TIME;

    sc_prim_channel_registry *r = s->get_prim_channel_registry();
#ifdef SYSTEMC_3_0_0
    if (r->m_update_list_p != r->m_update_list_end)
#else
    if (r->m_update_list_p != reinterpret_cast<sc_prim_channel *>(
            sc_prim_channel::list_end))
#endif
        return SC_ZERO_TIME;

    sc_time result = s->max_time();
    s->next_time(result);
    result -= sc_time_stamp();
    return result;
#else
    return sc_time_to_pending_activity();
#endif
}

bool simContextProxy::get_paused() {
#if INTC_EXT
    return sc_get_curr_simcontext()->m_paused;
#else
    return false;
#endif
}

bool simContextProxy::get_pause_pending() {
#if INTC_EXT
    return sc_get_curr_simcontext()->m_pause_pending;
#else
    return false;
#endif
}

void simContextProxy::set_pause_pending(bool pause) {
#if INTC_EXT
    sc_get_curr_simcontext()->m_pause_pending = pause;
#else
    if (pause)
        sc_core::sc_pause();
#endif
}

void simContextProxy::add_events(sc_event_timed *timed_event,
                                 sc_object* filter,
                                 EventVector *evs) {
#if INTC_EXT
    sc_event *event = timed_event->event();
    if (!event)
        return;

    // Add all process events
    sc_time t = timed_event->notify_time() - sc_time_stamp();
    for (sc_object* i : event->m_threads_dynamic)
        add_process_events(t, "dynamic thread", filter, i, evs);

    for (sc_object* i : event->m_threads_static)
        add_process_events(t, "static thread", filter, i, evs);

    for (sc_object* i : event->m_methods_dynamic)
        add_process_events(t, "dynamic method", filter, i, evs);

    for (sc_object* i : event->m_methods_static)
        add_process_events(t, "static method", filter, i, evs);

    // Add the "normal", non-kernel/internal, event to the list
    // NOTE: this is not a process event, so we pass NULL to filter
    std::string name(event->name());
    if (name.find(SC_KERNEL_EVENT_PREFIX) == std::string::npos) {
        if (filter == NULL) {
            evs->push_back(Event(t, name, simics::systemc::Event::EVENT));
        }
    }
#endif
}

void simContextProxy::add_process_events(const sc_time &t,
                                         const char *kind,
                                         sc_object* filter,
                                         sc_object* object,
                                         EventVector *evs) {
#if INTC_EXT
    if (filter != NULL && filter != object)
        return;

    evs->push_back(Event(t, object->name(), Event::EVENT, kind));
#endif
}

}  // namespace sc_core
