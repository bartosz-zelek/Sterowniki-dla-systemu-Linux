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

#ifndef SYSTEMC_CHECKPOINT_SIM_CONTEXT_PROXY_CHECKPOINT_H
#define SYSTEMC_CHECKPOINT_SIM_CONTEXT_PROXY_CHECKPOINT_H

#include <systemc>
#include <vector>
#include <map>
#include "sysc/kernel/sc_process.h"
#include "tlm_utils/peq_with_get.h"
#include "tlm_utils/tlm_quantumkeeper.h"

// Helper class to access private methods and data of SystemC kernel.
// This relies on a patched SystemC kernel with the simContextProxyCheckpoint
// marked as a "friend" to get access to some of the internals.
namespace sc_core {

/** \internal */
class simContextProxyCheckpoint {
  public:
    static void set_curr_time(sc_time *time);
    static std::vector<sc_event_timed *> timed_events();
    static void clear_timed_events();
    static void set_timed_events(std::vector<sc_event_timed *> events);
    static std::vector<sc_event *> delta_events();
    static void clear_delta_events();
    static void set_delta_events(std::vector<sc_event *> events);
    static sc_time *notify_time(sc_event_timed *timed);
    static sc_event *event(sc_event_timed *timed);
    static int notify_type(sc_event *event);
    static void set_notify_type(sc_event *event, int notify_type);
    static sc_event_timed *create(sc_event *event, sc_time *time);
    static sc_process_b *next_runnable(const sc_process_b *process);
    static void set_next_runnable(sc_process_b *process,
                                  sc_process_b *runnable);
    //  Returns additionally detached methods_push_head process.
    static sc_object *sc_find_object(const char *name);
    static sc_method_process *methods_push_head();
    static void set_methods_push_head(sc_method_process *method);
    static sc_method_process *methods_push_tail();
    static void set_methods_push_tail(sc_method_process *method);
    static sc_method_process *methods_pop();
    static void set_methods_pop(sc_method_process *method);
    static sc_thread_process *threads_push_head();
    static void set_threads_push_head(sc_thread_process *thread);
    static sc_thread_process *threads_push_tail();
    static void set_threads_push_tail(sc_thread_process *thread);
    static sc_thread_process *threads_pop();
    static void set_threads_pop(sc_thread_process *thread);
    static std::vector<sc_thread_process *> threads_static(sc_event *event);
    static void set_threads_static(sc_event *event,
                                   std::vector<sc_thread_process *> threads);
    static std::vector<sc_method_process *> methods_static(sc_event *event);
    static void set_methods_static(sc_event *event,
                                   std::vector<sc_method_process *> methods);
    static std::vector<sc_thread_process *> threads_dynamic(sc_event *event);
    static void set_threads_dynamic(sc_event *event,
                                    std::vector<sc_thread_process *> threads);
    static std::vector<sc_method_process *> methods_dynamic(sc_event *event);
    static void set_methods_dynamic(sc_event *event,
                                    std::vector<sc_method_process *> methods);
    static sc_event *event_p(sc_process_b *process);
    static void push_runnable_thread_front(sc_thread_process *process);
    static int current_state(const sc_process_b *process);
    static void set_current_state(sc_process_b *process, int state);
    static sc_prim_channel *update_next_p(const sc_prim_channel *channel);
    static void set_update_next_p(sc_prim_channel *channel,
                                  sc_prim_channel *next_p);
    static sc_prim_channel *channel_registry_update_next_p();
    static void set_channel_registry_update_next_p(sc_prim_channel *next_p);

    template<class T, sc_writer_policy POL>
    static T sc_signal_cur_val(const sc_signal<T, POL> *signal) {
        return signal->m_cur_val;
    }
    template<class T, sc_writer_policy POL>
    static void set_sc_signal_cur_val(sc_signal<T, POL> *signal, T value) {
        signal->m_cur_val = value;
    }
    template<class T, sc_writer_policy POL>
    static sc_dt::uint64 sc_signal_change_stamp(
            const sc_signal<T, POL> *signal) {
        return signal->m_change_stamp;
    }
    template<class T, sc_writer_policy POL>
    static void set_sc_signal_change_stamp(sc_signal<T, POL> *signal,
                                           sc_dt::uint64 time_stamp) {
        signal->m_change_stamp = time_stamp;
    }
    template<class T, sc_writer_policy POL>
    static T sc_signal_new_val(const sc_signal<T, POL> *signal) {
        return signal->m_new_val;
    }
    template<class T, sc_writer_policy POL>
    static void set_sc_signal_new_val(sc_signal<T, POL> *signal, T value) {
        signal->m_new_val = value;
    }

    static void sc_runnable_remove_thread(sc_thread_handle thread);
    static void sc_runnable_execute_thread_next(sc_process_handle handle);
    static bool sc_process_b_has_stack(sc_process_b *process);

    static sc_process_b *sc_mutex_owner(const sc_mutex *mutex);
    static void set_sc_mutex_owner(sc_mutex *mutex, sc_process_b *process);

    template<class T>
    static std::vector<T> ppq_transform(const sc_core::sc_ppq<T> *ppq) {
        std::vector<T> v;
        int size = ppq->size();
        for (int i = 1; i <= size; ++i)
            v.push_back(static_cast<T>(ppq->m_heap[i]));

        return v;
    }
    template<class T>
    static void ppq_transform(sc_core::sc_ppq<T> *queue, std::vector<T> v) {
        while (!queue->empty())
            delete queue->extract_top();

        for (typename std::vector<T>::iterator i = v.begin(); i != v.end(); ++i)
            queue->insert(*i);
    }
    template<class T>
    static std::multimap<const sc_core::sc_time, T*>
            peq_with_get_scheduled_events(
                const tlm_utils::peq_with_get<T> *peq) {
        return peq->m_scheduled_events;
    }
    template<class T>
    static void set_peq_with_get_scheduled_events(
            tlm_utils::peq_with_get<T> *peq,
            std::multimap<const sc_core::sc_time, T*> events) {
        peq->m_scheduled_events = events;
    }
    static sc_time::value_type sc_time_value(const sc_time *time);
    static void set_sc_time_value(sc_time *time, sc_time::value_type value);
    static sc_ppq<sc_time *> *event_queue_ppq(sc_event_queue *queue);

    template<class T>
    static std::vector<T> sc_fifo_buf(const sc_fifo<T> *fifo) {
        sc_fifo<T> *f = const_cast<sc_fifo<T> *>(fifo);
        std::vector<T> buf;
        T t;
        while (f->buf_read(t))
            buf.push_back(t);

        typename std::vector<T>::iterator i;
        for (i = buf.begin(); i != buf.end(); ++i)
            f->buf_write(*i);

        return buf;
    }
    template<class T>
    static void set_sc_fifo_buf(sc_fifo<T> *fifo, std::vector<T> buf) {
        T t;
        while (fifo->buf_read(t)) {}

        typename std::vector<T>::iterator i;
        for (i = buf.begin(); i != buf.end(); ++i)
            fifo->buf_write(*i);

        fifo->m_num_readable = buf.size();
        fifo->m_num_read = 0;
        fifo->m_num_written = 0;
    }

    static void set_sc_semaphore_value(sc_semaphore *semaphore, int value);
    static sc_event *sc_find_kernel_event(const char *name);

    static std::vector<sc_dt::sc_logic> sc_signal_resolved_val_vec(
        const sc_signal_resolved *signal);
    static void set_sc_signal_resolved_val_vec(sc_signal_resolved *signal,
        std::vector<sc_dt::sc_logic> values);
    template<class T>
    static std::vector<sc_process_b *> sc_signal_resolved_proc_vec(
        const T *signal) {
        return signal->m_proc_vec;
    }
    template<class T>
    static void set_sc_signal_resolved_proc_vec(T *signal,
        std::vector<sc_process_b *> values) {
        signal->m_proc_vec = values;
    }

    template<int W>
    static std::vector<sc_dt::sc_lv<W> *> sc_signal_rv_val_vec(
        const sc_signal_rv<W> *signal) {
        return signal->m_val_vec;
    }
    template<int W>
    static void set_sc_signal_rv_val_vec(sc_signal_rv<W> *signal,
        std::vector<sc_dt::sc_lv<W> *> values) {
        typename std::vector<sc_dt::sc_lv<W> *>::iterator i;
        for (i = signal->m_val_vec.begin(); i != signal->m_val_vec.end(); ++i)
            delete *i;

        signal->m_val_vec = values;
    }
    static int threadsInit();
    static sc_process_b::trigger_t trigger_type(const sc_process_b *process);
    static void set_trigger_type(sc_process_b *process,
                                 sc_process_b::trigger_t trigger);
    static sc_time *next_sync_point(
        tlm_utils::tlm_quantumkeeper *quantumkeeper);
    static sc_time *local_time(tlm_utils::tlm_quantumkeeper *quantumkeeper);
    static sc_dt::uint64 delta_count();
    static void set_delta_count(sc_dt::uint64 delta_count);

  private:
    template <class T>
    static T peek(sc_ppq<T> *ppq, int i);
};

}  // namespace sc_core

#endif
