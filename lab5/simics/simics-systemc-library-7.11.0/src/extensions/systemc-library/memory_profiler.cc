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

#include <simics/systemc/memory_profiler.h>
#include <simics/systemc/iface/sc_memory_profiler_control_interface.h>

#include <simics/device-api.h>  // FATAL_ERROR

#include <algorithm>
#include <utility>

namespace simics {
namespace systemc {
sc_core::sc_process_b *activeProcess(const AdapterMap &adapters);
}
}

namespace {
void errorUndefinedDelete(
    const char *op_new, const char *op_del, void *addr) {
    FATAL_ERROR(
        "Undefined behavior: attempting to delete memory allocated using"
        " operator '%s' with operator '%s' at %lld.\n", op_new, op_del,
        reinterpret_cast<unsigned long long>(addr));  // NOLINT (runtime/int)
}
}  // namespace

namespace simics {
namespace systemc {

MemoryProfiler::MemoryProfiler() : is_recording_(false) {}

// Provided that memory profiling is disabled when some block of memory is
// freed, it's possible that another block of memory originating at the same
// address is recorded twice when memory profiling is once again enabled. As
// per the current implementation such allocations are ignored.
void MemoryProfiler::recordAllocation(
    void *addr, size_t size, bool array_form) {
    if (!is_recording_) {
        return;
    }

    sc_core::sc_process_b *running_process = activeProcess(adapters_);
    if (running_process) {
        AllocationMap *allocations = &processes_[running_process];
        allocations->insert(
            std::make_pair(addr, Allocation(addr, size, array_form)));
    }
}

void MemoryProfiler::recordDeletion(void *addr, bool array_form) {
    if (!is_recording_) {
        return;
    }

    AllocationMap::iterator allocation;
    AllocationMap *allocations = NULL;
    if (!hasAllocation(addr, &allocation, &allocations)) {
        return;
    }

    if (allocation->second.array_form != array_form) {
        if (allocation->second.array_form) {
            errorUndefinedDelete("new[]", "delete", addr);
        } else {
            errorUndefinedDelete("new", "delete[]", addr);
        }
    }

    allocations->erase(allocation);
}

AllocationMap *MemoryProfiler::allocations(sc_core::sc_process_b *process) {
    return &processes_[process];
}

void MemoryProfiler::stopRecording() {
    is_recording_ = false;
}

AdapterMap::iterator MemoryProfiler::adapter(conf_object_t *obj) {
    AdapterMap::iterator it = adapters_.find(obj);
    if (it == adapters_.end()) {
        // profiling is disabled to start with
        std::pair<AdapterMap::iterator, bool> pair = adapters_.insert(
            std::pair<conf_object_t*, bool>(obj, false));
        it = pair.first;
    }
    return it;
}

bool MemoryProfiler::profilingForAdapter(conf_object_t *obj) {
    return adapter(obj)->second;
}

void MemoryProfiler::setProfilingForAdapter(conf_object_t *obj, bool enable) {
    adapter(obj)->second = enable;

    bool enabled_for_any = false;
    for (AdapterMap::const_iterator enabled_for_adapter = adapters_.begin();
         enabled_for_adapter != adapters_.end(); ++enabled_for_adapter) {
        enabled_for_any |= enabled_for_adapter->second;
    }
    is_recording_ = enabled_for_any;
}

typedef AllocationMap::iterator AllocationIt;
bool MemoryProfiler::hasAllocation(void *addr,
                                   AllocationIt *out_allocation,
                                   AllocationMap **out_allocations) {
    ProcessMap::iterator process = processes_.begin();
    for (; process != processes_.end(); ++process) {
        AllocationMap *allocations = &process->second;
        AllocationIt allocation = allocations->find(addr);
        if (allocation != allocations->end()) {
            *out_allocation = allocation;
            *out_allocations = allocations;
            return true;
        }
    }
    return false;
}

}  // namespace systemc
}  // namespace simics
