// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*****************************************************************************

  Licensed to Accellera Systems Initiative Inc. (Accellera) under one or
  more contributor license agreements.  See the NOTICE file distributed
  with this work for additional information regarding copyright ownership.
  Accellera licenses this file to you under the Apache License, Version 2.0
  (the "License"); you may not use this file except in compliance with the
  License.  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
  implied.  See the License for the specific language governing
  permissions and limitations under the License.

 *****************************************************************************/

#include <systemc-checkpoint/sim_context_proxy_checkpoint.h>

#include <map>
#include <set>
#include <string>
#include <vector>

#include "sysc/kernel/sc_runnable_int.h"
#include "sysc/kernel/sc_simcontext_int.h"

namespace sc_core {

// Peek into the priority event queue, using friend declaration
template <class T>
T simContextProxyCheckpoint::peek(sc_ppq<T> *ppq, int i) {
    return static_cast<T>(dynamic_cast<sc_ppq_base*>(ppq)->m_heap[i + 1]);
}

void simContextProxyCheckpoint::set_curr_time(sc_time *time) {
    sc_get_curr_simcontext()->m_curr_time = *time;
}

std::vector<sc_event_timed *> simContextProxyCheckpoint::timed_events() {
    std::vector<sc_event_timed*> events;
    sc_core::sc_ppq<sc_core::sc_event_timed*> *timed_events =
        sc_get_curr_simcontext()->m_timed_events;
    for (int i = 0; i < timed_events->size(); ++i)
        events.push_back(peek(timed_events, i));

    return events;
}

void simContextProxyCheckpoint::clear_timed_events() {
    sc_core::sc_ppq<sc_core::sc_event_timed*> *timed_events =
        sc_get_curr_simcontext()->m_timed_events;

    while (!timed_events->empty()) {
        sc_core::sc_event_timed *e = timed_events->extract_top();
        // If a timed event is cancelled, m_event is NULL.
        if (e->m_event)
            e->m_event->reset();

        delete e;
    }
}

void simContextProxyCheckpoint::set_timed_events(
        std::vector<sc_event_timed *> events) {
    std::set<sc_event*> unique_events;
    std::vector<sc_event_timed *>::iterator i;
    for (i = events.begin(); i != events.end(); ++i)
        if ((*i)->m_event)
            unique_events.insert((*i)->m_event);

    sc_core::sc_ppq<sc_core::sc_event_timed*> *timed_events =
        sc_get_curr_simcontext()->m_timed_events;

    while (!timed_events->empty()) {
        sc_core::sc_event_timed *e = timed_events->extract_top();
        if (unique_events.find(e->m_event) != unique_events.end()) {
            e->m_event = NULL;
            delete e;
        } else {
            events.push_back(e);
        }
    }

    for (i = events.begin(); i != events.end(); ++i)
        timed_events->insert(*i);
}

std::vector<sc_event *> simContextProxyCheckpoint::delta_events() {
    return sc_get_curr_simcontext()->m_delta_events;
}

void simContextProxyCheckpoint::clear_delta_events() {
    std::vector<sc_event *> events;
    sc_get_curr_simcontext()->m_delta_events = events;
}

void simContextProxyCheckpoint::set_delta_events(
        std::vector<sc_event *> events) {
    sc_get_curr_simcontext()->m_delta_events = events;
}

sc_time *simContextProxyCheckpoint::notify_time(sc_event_timed *timed) {
    return &timed->m_notify_time;
}

sc_event *simContextProxyCheckpoint::event(sc_event_timed *timed) {
    return timed->m_event;
}

void simContextProxyCheckpoint::set_notify_type(sc_event *event,
                                                int notify_type) {
    event->m_notify_type = static_cast<sc_event::notify_t>(notify_type);
}

int simContextProxyCheckpoint::notify_type(sc_event *event) {
    return event->m_notify_type;
}

sc_event_timed *simContextProxyCheckpoint::create(sc_event *event,
                                                  sc_time *time) {
    if (event == NULL)
        return NULL;

    sc_event_timed *e = new sc_event_timed(event, *time);
    event->m_timed = e;
    return e;
}

sc_process_b *simContextProxyCheckpoint::next_runnable(
        const sc_process_b *process) {
    return process ? process->m_runnable_p : NULL;
}

void simContextProxyCheckpoint::set_next_runnable(sc_process_b *process,
                                        sc_process_b *runnable) {
    if (process)
        process->m_runnable_p = runnable;
}

sc_object *simContextProxyCheckpoint::sc_find_object(const char *name) {
    sc_object *object = sc_core::sc_find_object(name);
    if (object)
        return object;

    if (object == NULL && std::string("methods_push_head") == name)
        return methods_push_head();

    if (object == NULL && std::string("threads_push_head") == name)
        return threads_push_head();

    sc_process_table *pt = sc_get_curr_simcontext()->m_process_table;
#if SC_VERSION_MAJOR == 2 && SC_VERSION_MINOR == 3 && SC_VERSION_PATCH == 1
    sc_thread_handle t = pt->m_thread_q;
    sc_method_handle m = pt->m_method_q;
#else
    sc_thread_handle t = pt->m_threads.m_head;
    sc_method_handle m = pt->m_methods.m_head;
#endif
    for (; t; t = t->next_exist()) {
        sc_thread_process *p = t;
        if (std::string(p->name()) == name)
            return p;
    }
    for (; m; m = m->next_exist()) {
        sc_method_process *p = m;
        if (std::string(p->name()) == name)
            return p;
    }

    return NULL;
}

sc_method_process *simContextProxyCheckpoint::methods_push_head() {
    sc_runnable *runnable = sc_get_curr_simcontext()->m_runnable;
    return runnable->m_methods_push_head;
}

void simContextProxyCheckpoint::set_methods_push_head(
        sc_method_process *method) {
    // Simulation was not started when saved. Use the default push_head.
    if (!method)
        return;

    sc_runnable *runnable = sc_get_curr_simcontext()->m_runnable;
    runnable->m_methods_push_head = method;
}

sc_method_process *simContextProxyCheckpoint::methods_push_tail() {
    sc_runnable *runnable = sc_get_curr_simcontext()->m_runnable;
    return runnable->m_methods_push_tail;
}

void simContextProxyCheckpoint::set_methods_push_tail(
        sc_method_process *method) {
    sc_runnable *runnable = sc_get_curr_simcontext()->m_runnable;
    runnable->m_methods_push_tail = method;
}

sc_method_process *simContextProxyCheckpoint::methods_pop() {
    sc_runnable *runnable = sc_get_curr_simcontext()->m_runnable;
    return runnable->m_methods_pop;
}

void simContextProxyCheckpoint::set_methods_pop(sc_method_process *method) {
    sc_runnable *runnable = sc_get_curr_simcontext()->m_runnable;
    runnable->m_methods_pop = method;
}

sc_thread_process *simContextProxyCheckpoint::threads_push_head() {
    sc_runnable *runnable = sc_get_curr_simcontext()->m_runnable;
    return runnable->m_threads_push_head;
}

void simContextProxyCheckpoint::set_threads_push_head(
        sc_thread_process *thread) {
    // Simulation was not started when saved. Use the default push_head.
    if (!thread)
        return;
    sc_runnable *runnable = sc_get_curr_simcontext()->m_runnable;
    runnable->m_threads_push_head = thread;
}

sc_thread_process *simContextProxyCheckpoint::threads_push_tail() {
    sc_runnable *runnable = sc_get_curr_simcontext()->m_runnable;
    return runnable->m_threads_push_tail;
}

void simContextProxyCheckpoint::set_threads_push_tail(
        sc_thread_process *thread) {
    sc_runnable *runnable = sc_get_curr_simcontext()->m_runnable;
    runnable->m_threads_push_tail = thread;
}

sc_thread_process *simContextProxyCheckpoint::threads_pop() {
    sc_runnable *runnable = sc_get_curr_simcontext()->m_runnable;
    return runnable->m_threads_pop;
}

void simContextProxyCheckpoint::set_threads_pop(sc_thread_process *thread) {
    sc_runnable *runnable = sc_get_curr_simcontext()->m_runnable;
    runnable->m_threads_pop = thread;
}

std::vector<sc_thread_process *> simContextProxyCheckpoint::threads_static(
        sc_event *event) {
    return event->m_threads_static;
}

void simContextProxyCheckpoint::set_threads_static(sc_event *event,
        std::vector<sc_thread_process *> threads) {
    event->m_threads_static = threads;
}

std::vector<sc_method_process *> simContextProxyCheckpoint::methods_static(
        sc_event *event) {
    return event->m_methods_static;
}

void simContextProxyCheckpoint::set_methods_static(sc_event *event,
        std::vector<sc_method_process *> methods) {
    event->m_methods_static = methods;
}

std::vector<sc_thread_process *> simContextProxyCheckpoint::threads_dynamic(
        sc_event *event) {
    return event->m_threads_dynamic;
}

void simContextProxyCheckpoint::set_threads_dynamic(sc_event *event,
        std::vector<sc_thread_process *> threads) {
    event->m_threads_dynamic = threads;
}

std::vector<sc_method_process *> simContextProxyCheckpoint::methods_dynamic(
        sc_event *event) {
    return event->m_methods_dynamic;
}

void simContextProxyCheckpoint::set_methods_dynamic(sc_event *event,
        std::vector<sc_method_process *> methods) {
    event->m_methods_dynamic = methods;
}

sc_event *simContextProxyCheckpoint::event_p(sc_process_b *process) {
    return const_cast<sc_event *>(process->m_event_p);
}

void simContextProxyCheckpoint::push_runnable_thread_front(
        sc_thread_process *process) {
    sc_runnable *runnable = sc_get_curr_simcontext()->m_runnable;
    runnable->push_front_thread(process);
}

int simContextProxyCheckpoint::current_state(const sc_process_b *process) {
    return process->m_state;
}

void simContextProxyCheckpoint::set_current_state(sc_process_b *process,
                                                  int state) {
    if (process)
        process->m_state = state;
}

sc_prim_channel *simContextProxyCheckpoint::update_next_p(
        const sc_prim_channel *channel) {
    return channel->m_update_next_p;
}

void simContextProxyCheckpoint::set_update_next_p(sc_prim_channel *channel,
                                        sc_prim_channel *next_p) {
    if (channel)
        channel->m_update_next_p = next_p;
}

sc_prim_channel *simContextProxyCheckpoint::channel_registry_update_next_p() {
    return sc_get_curr_simcontext()->m_prim_channel_registry->m_update_list_p;
}

void simContextProxyCheckpoint::set_channel_registry_update_next_p(
        sc_prim_channel *next_p) {
    sc_get_curr_simcontext()->m_prim_channel_registry->m_update_list_p = next_p;
}

void simContextProxyCheckpoint::sc_runnable_remove_thread(
        sc_thread_handle thread) {
    sc_runnable *runnable = sc_get_curr_simcontext()->m_runnable;
    runnable->remove_thread(thread);
}

void simContextProxyCheckpoint::sc_runnable_execute_thread_next(
        sc_process_handle handle) {
    sc_runnable *runnable = sc_get_curr_simcontext()->m_runnable;
    sc_core::sc_thread_process *t = handle;
    runnable->execute_thread_next(t);
}

bool simContextProxyCheckpoint::sc_process_b_has_stack(sc_process_b *process) {
    return process->m_has_stack;
}

sc_process_b *simContextProxyCheckpoint::sc_mutex_owner(const sc_mutex *mutex) {
    return mutex->m_owner;
}

void simContextProxyCheckpoint::set_sc_mutex_owner(sc_mutex *mutex,
                                         sc_process_b *process) {
    mutex->m_owner = process;
}

sc_time::value_type simContextProxyCheckpoint::sc_time_value(
        const sc_time *time) {
    return time->m_value;
}

void simContextProxyCheckpoint::set_sc_time_value(sc_time *time,
                                        sc_time::value_type value) {
    time->m_value = value;
}

sc_ppq<sc_time *> *simContextProxyCheckpoint::event_queue_ppq(
        sc_event_queue *queue) {
    return &queue->m_ppq;
}

void simContextProxyCheckpoint::set_sc_semaphore_value(sc_semaphore *semaphore,
                                             int value) {
    semaphore->m_value = value;
}

sc_event *simContextProxyCheckpoint::sc_find_kernel_event(const char *name) {
    return sc_get_curr_simcontext()->find_kernel_event(name);
}

std::vector<sc_dt::sc_logic>
simContextProxyCheckpoint::sc_signal_resolved_val_vec(
        const sc_signal_resolved *signal) {
    return signal->m_val_vec;
}

void simContextProxyCheckpoint::set_sc_signal_resolved_val_vec(
        sc_signal_resolved *signal,
    std::vector<sc_dt::sc_logic> values) {
    signal->m_val_vec = values;
}

int simContextProxyCheckpoint::threadsInit() {
    sc_process_handle handle = sc_core::sc_get_current_process_handle();
    std::map<int, sc_thread_process *> sorted_threads;

    sc_process_table *pt = sc_get_curr_simcontext()->m_process_table;
    for (sc_thread_handle t = pt->thread_q_head(); t; t = t->next_exist()) {
        sc_thread_process *p = t;
        sc_process_handle h(p);
        if (!h.valid())
            continue;

        if (h == handle)
            continue;

        bool initialized = p->m_state & sc_process_b::ps_bit_initialized;
        bool has_stack = sc_process_b_has_stack(p);

        // Tear down without setup of the thread stacks
        if (!initialized && has_stack) {
            int state = p->m_state;
            // The following kill ends sc_thread_cor_fn and runs destructors of
            // stack allocated objects.
            h.kill();
            // Recreate the stack again.
            p->prepare_for_simulation();

            // Restore the thread state for the following reset or remove.
            p->m_state = state;
        }

        if (!has_stack) {
            // Allocate the stack.
            p->prepare_for_simulation();
            p->m_runnable_p = reinterpret_cast<sc_thread_process *>(0);
        }

        sorted_threads[p->proc_id] = p;
    }

    std::map<int, sc_thread_process *>::iterator i;
    for (i = sorted_threads.begin(); i != sorted_threads.end(); ++i) {
        sc_process_handle h(i->second);
        bool initialized =
            i->second->m_state & sc_process_b::ps_bit_initialized;

        if (initialized || !i->second->dont_initialize())
            h.reset();

        if (i->second->m_state & sc_process_b::ps_bit_zombie)
            sc_runnable_remove_thread(h);
    }

    throw sc_core::sc_halt();
    return 0;
}

sc_process_b::trigger_t simContextProxyCheckpoint::trigger_type(
        const sc_process_b *process) {
    return process->m_trigger_type;
}

void simContextProxyCheckpoint::set_trigger_type(sc_process_b *process,
    sc_process_b::trigger_t trigger) {
    if (process)
        process->m_trigger_type = trigger;
}

sc_time *simContextProxyCheckpoint::next_sync_point(
        tlm_utils::tlm_quantumkeeper *quantumkeeper) {
    return &quantumkeeper->m_next_sync_point;
}

sc_time *simContextProxyCheckpoint::local_time(
        tlm_utils::tlm_quantumkeeper *quantumkeeper) {
    return &quantumkeeper->m_local_time;
}

sc_dt::uint64 simContextProxyCheckpoint::delta_count() {
    return sc_get_curr_simcontext()->m_delta_count;
}

void simContextProxyCheckpoint::set_delta_count(sc_dt::uint64 delta_count) {
    sc_get_curr_simcontext()->m_delta_count = delta_count;
}

}  // namespace sc_core
