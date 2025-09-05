// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/simulator-api.h>
#include <simics/systemc/adapter.h>
#include <simics/systemc/memory_profiler.h>
#include <simics/malloc_allocator.h>

#include <sysc/kernel/sc_simcontext.h>

namespace simics {
namespace systemc {

sc_core::sc_simcontext *activeContext(const AdapterMap &adapters) {
    for (AdapterMap::const_iterator it = adapters.begin();
         it != adapters.end(); ++it) {
        if (!it->second) {
            continue;
        }

        sc_core::sc_simcontext *ctx_simics =
            Adapter::simulation_from_conf_obj(it->first).context();
        sc_core::sc_simcontext *ctx_system = sc_core::sc_get_curr_simcontext();
        if (ctx_simics == ctx_system) {
            return ctx_simics;
        }
    }

    return NULL;
}

sc_core::sc_process_b *activeProcess(const AdapterMap &adapters) {
#ifndef RISC_SIMICS
    sc_core::sc_simcontext *context = activeContext(adapters);
    if (!context) {
        return NULL;
    }

    // We don't keep track of the allocations made by SystemC C-threads as of
    // yet. While we might want to do this in the future, we're currently not
    // sure if we need to support them.
    sc_core::sc_curr_proc_handle info = context->get_curr_proc_info();
    if (!info->process_handle)
        return NULL;

    sc_core::sc_curr_proc_kind kind = info->process_handle->proc_kind();
    if (kind != sc_core::SC_METHOD_PROC_ && kind != sc_core::SC_THREAD_PROC_)
        return NULL;

    return info->process_handle;
#else
    return NULL;
#endif
}

}  // namespace systemc
}  // namespace simics
