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

#ifndef SYSTEMC_CHECKPOINT_KERNEL_H
#define SYSTEMC_CHECKPOINT_KERNEL_H

#include <cci/serialization/split_member.hpp>

#include <systemc>

#include <systemc-checkpoint/time_information_interface.h>

#include <string>
#include <vector>

namespace sc_checkpoint {

class Time {
  public:
    Time();
    explicit Time(sc_core::sc_time *time);
    void init(const sc_core::sc_time *time);
    void operator=(sc_core::sc_time *time);
    operator sc_core::sc_time *();
    template<class Archive>
    void serialize(Archive &ar,  // NOLINT(runtime/references)
                   const unsigned int version);

  private:
    sc_core::sc_time::value_type time_value_;
    sc_core::sc_time time_;
};

template<class T> class Process;

class Event {
  public:
    Event();
    Event(sc_core::sc_event *event);  // NOLINT(runtime/explicit)
    operator sc_core::sc_event *();
    bool isDynamic();
    template<class Archive>
    void serialize(Archive &ar,  // NOLINT(runtime/references)
                   const unsigned int version);

  private:
    sc_core::sc_event *find();
    void restore(sc_core::sc_event *event);

    std::string event_name_;
    int notify_type_;
    bool isNull_;
    std::vector<Process<sc_core::sc_method_process> > methods_static_;
    std::vector<Process<sc_core::sc_thread_process> > threads_static_;
    std::vector<Process<sc_core::sc_method_process> > methods_dynamic_;
    std::vector<Process<sc_core::sc_thread_process> > threads_dynamic_;
};

class EventTimed {
  public:
    EventTimed();
    EventTimed(sc_core::sc_event_timed *timed);  // NOLINT(runtime/explicit)
    operator sc_core::sc_event_timed *();
    bool isDynamic();
    template<class Archive>
    void serialize(Archive &ar,  // NOLINT(runtime/references)
                   const unsigned int version);

  private:
    Event event_;
    Time time_;
};

class Object {
  public:
    void operator=(const sc_core::sc_object *object);
    void init(const sc_core::sc_object *object);
    operator sc_core::sc_object *();
    template<class Archive>
    void serialize(Archive &ar,  // NOLINT(runtime/references)
                   const unsigned int version);

  protected:
    std::string object_name_;
};

template<class T>
class Process : public Object {
  public:
    Process() : end_of_runnable_(false), not_set_(false),
                state_(0), ended_(false),
                trigger_(sc_core::sc_process_b::STATIC) {}
    Process(const T *process);  // NOLINT
    void operator=(const T *process);
    void init(const T *process);
    operator T *() const;
    int state();
    bool ended();
    template<class Archive>
    void save(Archive &ar,  // NOLINT(runtime/references)
              const unsigned int version) const;
    template<class Archive>
    void load(Archive &ar,  // NOLINT(runtime/references)
              const unsigned int version);
    CCI_SERIALIZATION_SPLIT_MEMBER();

  protected:
    Object next_runnable_;
    bool end_of_runnable_;
    bool not_set_;
    int state_;
    bool ended_;
    sc_core::sc_process_b::trigger_t trigger_;
};

class PrimChannel : public Object {
  public:
    PrimChannel() : not_set_(false), end_of_list_(false) {}
    PrimChannel(const sc_core::sc_prim_channel *channel);  // NOLINT
    void operator=(const sc_core::sc_prim_channel *channel);
    void init(const sc_core::sc_prim_channel *channel);
    operator sc_core::sc_prim_channel *();
    template<class Archive>
    void save(Archive &ar,  // NOLINT(runtime/references)
              const unsigned int version) const;
    template<class Archive>
    void load(Archive &ar,  // NOLINT(runtime/references)
              const unsigned int version);
    CCI_SERIALIZATION_SPLIT_MEMBER();

  protected:
    bool not_set_;
    bool end_of_list_;
    Object update_next_;
};

class Kernel : public ExternalTimeInformationInterface {
  public:
    Kernel()
        : push_heads_created_(false), time_information_high_(0),
          time_information_low_(0), reverse_execution_(false) {}
    template<class Archive>
    void serialize(Archive &ar,  // NOLINT(runtime/references)
                   const unsigned int version);
    template<class Archive>
    void save(Archive &ar,  // NOLINT(runtime/references)
              const unsigned int version);
    template<class Archive>
    void load(Archive &ar,  // NOLINT(runtime/references)
              const unsigned int version);
    virtual void set_time_information(int64_t high, uint64_t low);
    virtual void time_information(int64_t *high, uint64_t *low);
    void set_reverse_execution(bool isReverseExecution);

  private:
    void init();
    bool push_heads_created_;
    Time time_;
    std::vector<EventTimed> timed_;
    std::vector<Event> delta_;
    std::vector<Process<sc_core::sc_method_process> > methods_;
    std::vector<Process<sc_core::sc_thread_process> > threads_;
    Process<sc_core::sc_method_process> methods_head_;
    Process<sc_core::sc_method_process> methods_tail_;
    Process<sc_core::sc_method_process> methods_pop_;
    Process<sc_core::sc_thread_process> threads_head_;
    Process<sc_core::sc_thread_process> threads_tail_;
    Process<sc_core::sc_thread_process> threads_pop_;
    int64_t time_information_high_;
    uint64_t time_information_low_;
    std::vector<PrimChannel> channels_;
    PrimChannel channel_registry_;
    sc_dt::uint64 delta_count_;
    bool reverse_execution_;
};

}  // namespace sc_checkpoint

#endif
