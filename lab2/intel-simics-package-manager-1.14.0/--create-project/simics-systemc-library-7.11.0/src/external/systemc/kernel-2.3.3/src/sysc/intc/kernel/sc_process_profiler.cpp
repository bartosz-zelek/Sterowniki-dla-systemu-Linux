/***                                                                       ***/
/***   INTEL CORPORATION PROPRIETARY INFORMATION                           ***/
/***                                                                       ***/
/***   This software is supplied under the terms of a license              ***/
/***   agreement or nondisclosure agreement with Intel Corporation         ***/
/***   and may not be copied or disclosed except in accordance with        ***/
/***   the terms of that agreement.                                        ***/
/***   Copyright 2015-2021 Intel Corporation                               ***/
/***                                                                       ***/

#if INTC_EXT  // This file is a NOP unless feature is enabled

#include "sysc/intc/kernel/sc_process_profiler.h"
#include "sysc/kernel/sc_simcontext.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <ctime>
#endif

#include <algorithm>
#include <cstdlib>
#include <limits>

namespace {
sc_dt::uint64 now() {
#ifdef _WIN32
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    return (((static_cast<sc_dt::uint64>(ft.dwHighDateTime) << 32)
             | ft.dwLowDateTime) - 116444736000000000ULL) * 100;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<sc_dt::uint64>(ts.tv_sec) * 1000000000ULL
        + ts.tv_nsec;
#endif
}
}  // namespace

namespace intc {

// Precondition: get_process_profiler_enabled() should return true before calling it
void
sc_process_profiler::start_profiling() {
    m_start = now();
}

// Precondition: get_process_profiler_enabled() should return true before calling it
void
sc_process_profiler::stop_profiling() {
    if (m_start <= 0) {
        using namespace sc_core;  // NOLINT(build/namespaces)
        SC_REPORT_ERROR(SC_ID_PROFILER_STOPPED_BEFORE_STARTED_, "");
    }
    m_duration += now() - m_start;
    m_min_time = (std::min)(m_min_time, m_duration);
    m_max_time = (std::max)(m_max_time, m_duration);
    m_total_time += m_duration;
    m_number_of_calls += 1;
    m_start = 0;
    m_duration = 0;
}

// Precondition: get_process_profiler_enabled() should return true before calling it
void
sc_process_profiler::pause_profiling() {
    if (m_start <= 0) {
        using namespace sc_core;  // NOLINT(build/namespaces)
        SC_REPORT_ERROR(SC_ID_PROFILER_STOPPED_BEFORE_STARTED_, "");
    }
    m_duration = now() - m_start;
    m_start = 0;
}

void
sc_process_profiler::reset_profiling() {
    m_start = 0;
    m_duration = 0;
    m_min_time = (std::numeric_limits<sc_dt::uint64>::max)();
    m_max_time = 0;
    m_total_time = 0;
    m_number_of_calls = 0;
}
        
}  // namespace intc

#endif
