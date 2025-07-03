/***                                                                       ***/
/***   INTEL CORPORATION PROPRIETARY INFORMATION                           ***/
/***                                                                       ***/
/***   This software is supplied under the terms of a license              ***/
/***   agreement or nondisclosure agreement with Intel Corporation         ***/
/***   and may not be copied or disclosed except in accordance with        ***/
/***   the terms of that agreement.                                        ***/
/***   Copyright 2015-2021 Intel Corporation                               ***/
/***                                                                       ***/

#ifndef KERNEL_SC_PROCESS_PROFILER_H
#define KERNEL_SC_PROCESS_PROFILER_H

#include "sysc/intc/kernel/sc_process_profiler_interface.h"

namespace sc_core {
class sc_simcontext;
}

namespace intc {

class sc_process_profiler : public sc_process_profiler_interface {
  public:
    explicit sc_process_profiler(sc_core::sc_simcontext* context)
        : m_context(context) {
        reset_profiling();
    }

    void start_profiling();
    void stop_profiling();
    void pause_profiling();
    void reset_profiling();

    sc_dt::uint64 get_min_time() const {
        return m_min_time;
    }
    sc_dt::uint64 get_max_time() const {
        return m_max_time;
    }
    sc_dt::uint64 get_total_time() const {
        return m_total_time;
    }
    sc_dt::uint64 get_number_of_calls() const {
        return m_number_of_calls;
    }

  private:
    sc_core::sc_simcontext* m_context;

    sc_dt::uint64 m_start;
    sc_dt::uint64 m_duration;
    sc_dt::uint64 m_min_time;
    sc_dt::uint64 m_max_time;
    sc_dt::uint64 m_total_time;
    sc_dt::uint64 m_number_of_calls;
};

}  // namespace intc

#endif
