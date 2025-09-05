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

#include <cci/serialization/string.hpp>
#include <cci/serialization/split_member.hpp>

#include <systemc-checkpoint/all_sc_objects.h>
#include <systemc-checkpoint/iarchive.h>
#include <systemc-checkpoint/kernel.h>
#include <systemc-checkpoint/oarchive.h>
#include <systemc-checkpoint/sim_context_proxy_checkpoint.h>
#include <systemc-checkpoint/serialization/smd.h>
#include <systemc-checkpoint/serialization/vector.h>

#include <set>
#include <vector>

#include "sysc/kernel/sc_method_process.h"
#include "sysc/kernel/sc_runnable_int.h"

using sc_core::sc_event;
using sc_core::sc_method_process;
using sc_core::sc_object;
using sc_core::sc_prim_channel;
using sc_core::sc_process_b;
using sc_core::sc_process_handle;
using sc_core::sc_thread_process;
using sc_core::simContextProxyCheckpoint;

// Aligned with macros SC_NO_METHODS, SC_NO_THREADS defines in sc_runnable_int.h
#define SC_NO_HANDLE (reinterpret_cast<T *>(0xdb))
#define SC_NO_METHODS (reinterpret_cast<sc_method_process *>(0xdb))
#define SC_NO_THREADS (reinterpret_cast<sc_thread_process *>(0xdb))

namespace sc_checkpoint {

Time::Time() : time_value_(0) {}

Time::Time(sc_core::sc_time *time) : time_value_(time->value()) {}

void Time::init(const sc_core::sc_time *time) {
    time_value_ = time->value();
}

void Time::operator=(sc_core::sc_time *time) {
    time_value_ = time->value();
}

Time::operator sc_core::sc_time *() {
    time_ = sc_core::sc_time::from_value(time_value_);
    return &time_;
}

template<class Archive>
void Time::serialize(Archive &ar,
                     const unsigned int version) {
    ar & SMD(time_value_);
}

Event::Event() : notify_type_(0), isNull_(true) {}

Event::Event(sc_event *event) : notify_type_(0) {
    isNull_ = event == NULL;
    if (isNull_)
        return;

    event_name_ = event->name();
    notify_type_ = simContextProxyCheckpoint::notify_type(event);

    std::vector<sc_method_process *> methods;
    methods = simContextProxyCheckpoint::methods_static(event);
    methods_static_.assign(methods.begin(), methods.end());

    methods = simContextProxyCheckpoint::methods_dynamic(event);
    methods_dynamic_.assign(methods.begin(), methods.end());

    std::vector<sc_thread_process *> threads;
    threads = simContextProxyCheckpoint::threads_static(event);
    threads_static_.assign(threads.begin(), threads.end());

    threads = simContextProxyCheckpoint::threads_dynamic(event);
    threads_dynamic_.assign(threads.begin(), threads.end());
}

Event::operator sc_event *() {
    if (isNull_)
        return NULL;

    sc_event *event = find();
    if (event)
        restore(event);

    return event;
}

bool Event::isDynamic() {
    sc_event *event = find();
    return event == NULL;
}

template<class Archive>
void Event::serialize(Archive &ar,
                      const unsigned int version) {
    ar & SMD(event_name_);
    ar & SMD(notify_type_);
    ar & SMD(isNull_);
    ar & SMD(methods_static_);
    ar & SMD(threads_static_);
    ar & SMD(methods_dynamic_);
    ar & SMD(threads_dynamic_);
}

sc_core::sc_event *Event::find() {
    if (sc_event *event = sc_core::sc_find_event(event_name_.c_str()))
        return event;

    if (sc_event *event = simContextProxyCheckpoint::sc_find_kernel_event(
            event_name_.c_str()))
        return event;

    //  Dynamic event resolving
    std::vector<Process<sc_thread_process> >::iterator i;
    for (i = threads_dynamic_.begin(); i != threads_dynamic_.end(); ++i) {
        if (sc_event *event = simContextProxyCheckpoint::event_p(*i)) {
            return event;
        }
    }

    return NULL;
}

void Event::restore(sc_event *event) {
    simContextProxyCheckpoint::set_notify_type(event, notify_type_);

    std::vector<sc_method_process *> methods;
    methods.assign(methods_static_.begin(), methods_static_.end());
    simContextProxyCheckpoint::set_methods_static(event, methods);

    methods.assign(methods_dynamic_.begin(), methods_dynamic_.end());
    simContextProxyCheckpoint::set_methods_dynamic(event, methods);

    std::vector<sc_thread_process *> threads;
    threads.assign(threads_static_.begin(), threads_static_.end());
    simContextProxyCheckpoint::set_threads_static(event, threads);

    threads.assign(threads_dynamic_.begin(), threads_dynamic_.end());
    simContextProxyCheckpoint::set_threads_dynamic(event, threads);
}

EventTimed::EventTimed() {}

EventTimed::EventTimed(sc_core::sc_event_timed *timed)
    : event_(simContextProxyCheckpoint::event(timed)),
      time_(simContextProxyCheckpoint::notify_time(timed))
{}

EventTimed::operator sc_core::sc_event_timed *() {
    return simContextProxyCheckpoint::create(event_, time_);
}

bool EventTimed::isDynamic() {
    return static_cast<sc_event *>(event_) == NULL;
}

template<class Archive>
void EventTimed::serialize(Archive &ar,
                           const unsigned int version) {
    ar & SMD(event_);
    ar & SMD(time_);
}

void Object::operator=(const sc_object *object) {
    init(object);
}

void Object::init(const sc_object *object) {
    if (object)
        object_name_ = object->name();
}

Object::operator sc_object *() {
    return simContextProxyCheckpoint::sc_find_object(object_name_.c_str());
}

template<class Archive>
void Object::serialize(Archive &ar,
                       const unsigned int version) {
    ar & SMD(object_name_);
}

template<class T>
Process<T>::Process(const T *process)
    : end_of_runnable_(false), not_set_(false), state_(0),
      ended_(false), trigger_(sc_core::sc_process_b::STATIC) {
    init(process);
}

template<class T>
void Process<T>::operator=(const T *process) {
    init(process);
}

template<class T>
void Process<T>::init(const T *process) {
    end_of_runnable_ = false;
    not_set_ = false;
    state_ = 0;
    ended_ = false;
    trigger_ = sc_core::sc_process_b::STATIC;

    if (process == SC_NO_HANDLE) {
        end_of_runnable_ = true;
        return;
    }

    if (process == 0) {
        not_set_ = true;
        return;
    }

    Object::init(process);

    if (simContextProxyCheckpoint::next_runnable(process) == SC_NO_HANDLE) {
        end_of_runnable_ = true;
    } else {
        next_runnable_.init(simContextProxyCheckpoint::next_runnable(process));
    }

    state_ = simContextProxyCheckpoint::current_state(process);
    trigger_ = simContextProxyCheckpoint::trigger_type(process);
}

template<class T>
Process<T>::operator T *() const {
    T *process = dynamic_cast<T *>(
        simContextProxyCheckpoint::sc_find_object(object_name_.c_str()));
    if (process)
        return process;

    if (not_set_)
        return NULL;

    return SC_NO_HANDLE;
}

template<class T>
int Process<T>::state() {
    return state_;
}

template<class T>
bool Process<T>::ended() {
    return ended_;
}

template<class T>
template<class Archive>
void Process<T>::save(Archive &ar,  // NOLINT(runtime/references)
                      const unsigned int version) const {
    ar & SMD_BASE_OBJECT(Object);
    ar & SMD(next_runnable_);
    ar & SMD(end_of_runnable_);
    ar & SMD(not_set_);
    ar & SMD(state_);
    ar & SMD(trigger_);
}

template<class T>
template<class Archive>
void Process<T>::load(Archive &ar,  // NOLINT(runtime/references)
                      const unsigned int version) {
    ar & SMD_BASE_OBJECT(Object);
    ar & SMD(next_runnable_);
    ar & SMD(end_of_runnable_);
    ar & SMD(not_set_);
    ar & SMD(state_);
    ar & SMD(trigger_);

    T *process = dynamic_cast<T *>(
        simContextProxyCheckpoint::sc_find_object(object_name_.c_str()));
    T *runnable = SC_NO_HANDLE;

    if (!end_of_runnable_)
        runnable = dynamic_cast<T*>(static_cast<sc_object *>(next_runnable_));

    if (process)
        ended_ = simContextProxyCheckpoint::current_state(process) &
                 sc_process_b::ps_bit_zombie;

    simContextProxyCheckpoint::set_next_runnable(process, runnable);
    simContextProxyCheckpoint::set_current_state(process, state_);
    simContextProxyCheckpoint::set_trigger_type(process, trigger_);
}

PrimChannel::PrimChannel(const sc_prim_channel *channel) {
    init(channel);
}

void PrimChannel::operator=(const sc_prim_channel *channel) {
    init(channel);
}

void PrimChannel::init(const sc_prim_channel *channel) {
    sc_prim_channel *end =
        reinterpret_cast<sc_prim_channel *>(sc_prim_channel::list_end);

    if (channel == end) {
        not_set_ = true;
        return;
    }

    not_set_ = false;
    Object::init(channel);
    sc_prim_channel *next = simContextProxyCheckpoint::update_next_p(channel);
    end_of_list_ = next == end;
    if (!end_of_list_)
        update_next_ = next;
}

PrimChannel::operator sc_prim_channel *() {
    if (not_set_)
        return reinterpret_cast<sc_prim_channel *>(sc_prim_channel::list_end);

    return dynamic_cast<sc_prim_channel *>(static_cast<sc_object *>(*this));
}

template<class Archive>
void PrimChannel::save(Archive &ar,
                       const unsigned int version) const {
    ar & SMD_BASE_OBJECT(Object);
    ar & SMD(not_set_);
    ar & SMD(end_of_list_);
    ar & SMD(update_next_);
}

template<class Archive>
void PrimChannel::load(Archive &ar,
                       const unsigned int version) {
    ar & SMD_BASE_OBJECT(Object);
    ar & SMD(not_set_);
    ar & SMD(end_of_list_);
    ar & SMD(update_next_);

    if (not_set_)
        return;

    sc_prim_channel *channel = dynamic_cast<sc_prim_channel *>(
        static_cast<sc_object *>(*this));

    sc_prim_channel *next = end_of_list_ ?
        reinterpret_cast<sc_prim_channel *>(sc_prim_channel::list_end) :
        dynamic_cast<sc_prim_channel *>(static_cast<sc_object *>(update_next_));

    simContextProxyCheckpoint::set_update_next_p(channel, next);
}

template<class Archive>
void Kernel::serialize(Archive &ar,
                       const unsigned int version) {
    if (Archive::is_saving::value)
        save<Archive>(ar, version);
    else
        load<Archive>(ar, version);
}

template<class Archive>
void Kernel::save(Archive &ar,
                  const unsigned int version) {
    init();

    ar & SMD(push_heads_created_);
    ar & SMD(time_);
    ar & SMD(time_information_high_);
    ar & SMD(time_information_low_);
    ar & SMD(delta_count_);
    ar & sc_checkpoint::serialization::create_smd("threads_prepare", threads_);
    ar & SMD(timed_);
    ar & SMD(delta_);
    ar & SMD(methods_);
    ar & SMD(threads_);
    ar & SMD(methods_head_);
    ar & SMD(methods_tail_);
    ar & SMD(methods_pop_);
    ar & SMD(threads_head_);
    ar & SMD(threads_tail_);
    ar & SMD(threads_pop_);
    ar & SMD(channels_);
    ar & SMD(channel_registry_);
}

template<class Archive>
void Kernel::load(Archive &ar,
                  const unsigned int version) {
    ar & SMD(push_heads_created_);

    //  Creates detached methods_push_head process.
    if (push_heads_created_) {
        sc_core::sc_get_curr_simcontext()->initialize(true);
    }

    ar & SMD(time_);
    simContextProxyCheckpoint::set_curr_time(time_);

    ar & SMD(time_information_high_);
    ar & SMD(time_information_low_);
    ar & SMD(delta_count_);

    simContextProxyCheckpoint::clear_timed_events();
    simContextProxyCheckpoint::clear_delta_events();

    // The state flags of the threads need to be loaded for the stack tear down
    // and setup to work properly.
    ar & sc_checkpoint::serialization::create_smd("threads_prepare", threads_);

    // Is SystemC loaded at the time it was created earlier ?
    bool loadAtZero = delta_count_ == 0;

    // Reset the threads only if they have been run before.
    bool threadReset = !loadAtZero || reverse_execution_;

    if (sc_core::sc_get_curr_simcontext()->cor_pkg() != NULL && threadReset) {
        // Schedule the process for thread stack tear down and setup

        sc_process_handle handle =
            sc_core::sc_spawn(&simContextProxyCheckpoint::threadsInit);

        simContextProxyCheckpoint::set_next_runnable(
            simContextProxyCheckpoint::threads_push_head(), handle);
        simContextProxyCheckpoint::set_next_runnable(handle, SC_NO_THREADS);
        simContextProxyCheckpoint::set_threads_push_tail(handle);

        sc_core::sc_start(sc_core::SC_ZERO_TIME);
        sc_core::sc_get_curr_simcontext()->remove_ended_process(handle);
    }

    ar & SMD(timed_);
    ar & SMD(delta_);
    ar & SMD(methods_);
    ar & SMD(threads_);
    ar & SMD(methods_head_);
    ar & SMD(methods_tail_);
    ar & SMD(methods_pop_);
    ar & SMD(threads_head_);
    ar & SMD(threads_tail_);
    ar & SMD(threads_pop_);
    ar & SMD(channels_);
    ar & SMD(channel_registry_);

    std::vector<sc_core::sc_event_timed *> events;
    std::vector<EventTimed>::iterator i;
    for (i = timed_.begin(); i != timed_.end(); ++i) {
        if (!i->isDynamic())
            events.push_back(*i);
    }

    std::vector<sc_core::sc_event *> delta;
    std::vector<Event>::iterator j;
    for (j = delta_.begin(); j != delta_.end(); ++j) {
        if (!j->isDynamic())
            delta.push_back(*j);
    }

    simContextProxyCheckpoint::set_timed_events(events);
    simContextProxyCheckpoint::set_delta_events(delta);

    simContextProxyCheckpoint::set_methods_push_head(methods_head_);
    simContextProxyCheckpoint::set_methods_push_tail(methods_tail_);
    simContextProxyCheckpoint::set_methods_pop(methods_pop_);

    simContextProxyCheckpoint::set_threads_push_head(threads_head_);
    simContextProxyCheckpoint::set_threads_push_tail(threads_tail_);
    simContextProxyCheckpoint::set_threads_pop(threads_pop_);

    simContextProxyCheckpoint::set_channel_registry_update_next_p(
        channel_registry_);

    simContextProxyCheckpoint::set_delta_count(delta_count_);

    std::vector<Process<sc_core::sc_thread_process> >::iterator k;
    for (k = threads_.begin(); k != threads_.end(); ++k) {
        if (k->ended())
            simContextProxyCheckpoint::sc_runnable_remove_thread(*k);
    }
}

void Kernel::set_reverse_execution(bool isReverseExecution) {
    reverse_execution_ = isReverseExecution;
}

void Kernel::set_time_information(int64_t high, uint64_t low) {
    time_information_high_ = high;
    time_information_low_ = low;
}

void Kernel::time_information(int64_t *high, uint64_t *low) {
    *high = time_information_high_;
    *low = time_information_low_;
}

void Kernel::init() {
    push_heads_created_ =
        simContextProxyCheckpoint::methods_push_head() != NULL;
    time_.init(&sc_core::sc_time_stamp());
    std::vector<sc_core::sc_event_timed *> events =
        simContextProxyCheckpoint::timed_events();
    timed_.assign(events.begin(), events.end());
    std::vector<sc_core::sc_event *> delta =
        simContextProxyCheckpoint::delta_events();
    delta_.assign(delta.begin(), delta.end());
    methods_.clear();
    threads_.clear();

    AllScObjects objects;
    for (AllScObjects::iterator i = objects.begin(); i != objects.end(); ++i) {
        if (sc_method_process *p = dynamic_cast<sc_method_process *>(*i))
            methods_.push_back(p);

        if (sc_thread_process *p = dynamic_cast<sc_thread_process *>(*i))
            threads_.push_back(p);

        if (sc_prim_channel *c = dynamic_cast<sc_prim_channel *>(*i))
            channels_.push_back(c);
    }

    methods_head_ = simContextProxyCheckpoint::methods_push_head();
    methods_tail_ = simContextProxyCheckpoint::methods_push_tail();
    methods_pop_ = simContextProxyCheckpoint::methods_pop();
    threads_head_ = simContextProxyCheckpoint::threads_push_head();
    threads_tail_ = simContextProxyCheckpoint::threads_push_tail();
    threads_pop_ = simContextProxyCheckpoint::threads_pop();

    channel_registry_ =
        simContextProxyCheckpoint::channel_registry_update_next_p();
    delta_count_ = simContextProxyCheckpoint::delta_count();
}

template
void Time::serialize<OArchive>(OArchive&, unsigned int);

template
void Time::serialize<IArchive>(IArchive&, unsigned int);

template
void Event::serialize<OArchive>(OArchive&, unsigned int);

template
void Event::serialize<IArchive>(IArchive&, unsigned int);

template
void EventTimed::serialize<OArchive>(OArchive&, unsigned int);

template
void EventTimed::serialize<IArchive>(IArchive&, unsigned int);

template
void Object::serialize<OArchive>(OArchive&, unsigned int);

template
void Object::serialize<IArchive>(IArchive&, unsigned int);

template
void Process<sc_method_process>::save<OArchive>(OArchive&, unsigned int) const;

template
void Process<sc_method_process>::load<IArchive>(IArchive&, unsigned int);

template
void Process<sc_thread_process>::save<OArchive>(OArchive&, unsigned int) const;

template
void Process<sc_thread_process>::load<IArchive>(IArchive&, unsigned int);

template
void PrimChannel::save<OArchive>(OArchive&, unsigned int) const;

template
void PrimChannel::load<IArchive>(IArchive&, unsigned int);

template
void Kernel::serialize<OArchive>(OArchive&, unsigned int);

template
void Kernel::serialize<IArchive>(IArchive&, unsigned int);

template
void Kernel::save<OArchive>(OArchive&, unsigned int);

template
void Kernel::load<IArchive>(IArchive&, unsigned int);

}  // namespace sc_checkpoint
