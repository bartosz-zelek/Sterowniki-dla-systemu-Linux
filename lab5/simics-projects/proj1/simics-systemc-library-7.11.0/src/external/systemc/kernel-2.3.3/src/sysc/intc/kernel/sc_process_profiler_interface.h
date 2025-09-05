/***                                                                       ***/
/***   INTEL CORPORATION PROPRIETARY INFORMATION                           ***/
/***                                                                       ***/
/***   This software is supplied under the terms of a license              ***/
/***   agreement or nondisclosure agreement with Intel Corporation         ***/
/***   and may not be copied or disclosed except in accordance with        ***/
/***   the terms of that agreement.                                        ***/
/***   Copyright 2015-2021 Intel Corporation                               ***/
/***                                                                       ***/

#ifndef KERNEL_SC_PROCESS_PROFILER_INTERFACE_H
#define KERNEL_SC_PROCESS_PROFILER_INTERFACE_H

#include "sysc/datatypes/int/sc_nbdefs.h"

namespace intc {

class sc_process_profiler_interface {
  public:
    virtual ~sc_process_profiler_interface() {}
    
    virtual void start_profiling() = 0;
    virtual void stop_profiling() = 0;
    virtual void pause_profiling() = 0;

    virtual sc_dt::uint64 get_min_time() const = 0;
    virtual sc_dt::uint64 get_max_time() const = 0;
    virtual sc_dt::uint64 get_total_time() const = 0;
    virtual sc_dt::uint64 get_number_of_calls() const = 0; 
};
    
}  // namespace intc

#endif
