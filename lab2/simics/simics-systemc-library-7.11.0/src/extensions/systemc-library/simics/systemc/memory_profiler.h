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

#ifndef SIMICS_SYSTEMC_MEMORY_PROFILER_H
#define SIMICS_SYSTEMC_MEMORY_PROFILER_H

#include <simics/base/types.h>  // conf_object_t
#include <simics/malloc_allocator.h>

#include <sysc/kernel/sc_process.h>

#include <map>
#include <cstddef>  // size_t
#include <utility>  // pair
#include <functional>  // less

namespace simics {
namespace systemc {

struct Allocation {
    Allocation(void *addr, std::size_t size, bool array_form)
        : addr(addr), size(size), array_form(array_form) {}

    void *addr;
    size_t size;
    bool array_form;
};

// adapter -> profiling enabled
typedef MallocAllocator<std::pair<const conf_object_t*, bool> >
        AdapterAllocator;
typedef std::map<conf_object_t*, bool, std::less<conf_object_t*>,
                 AdapterAllocator> AdapterMap;

// address -> allocation (allocations)
typedef MallocAllocator<std::pair<const void *, Allocation> >
        AllocationAllocator;
typedef std::map<void *, Allocation, std::less<void *>,
                 AllocationAllocator> AllocationMap;

// process -> allocations
typedef MallocAllocator<std::pair<const sc_core::sc_process_b *,
                                            AllocationMap> > ProcessAllocator;
typedef std::map<sc_core::sc_process_b *, AllocationMap,
                 std::less<sc_core::sc_process_b *>,
                 ProcessAllocator> ProcessMap;

class MemoryProfiler {
  public:
    MemoryProfiler();

    void recordAllocation(void *addr, size_t size, bool array_form);
    void recordDeletion(void *addr, bool array_form);

    AllocationMap *allocations(sc_core::sc_process_b *process);
    void stopRecording();

    bool profilingForAdapter(conf_object_t *obj);
    void setProfilingForAdapter(conf_object_t *obj, bool enable);

  private:
    AdapterMap adapters_;
    ProcessMap processes_;
    bool is_recording_;

    AdapterMap::iterator adapter(conf_object_t *obj);
    bool hasAllocation(void *addr,
                       AllocationMap::iterator *out_allocation,
                       AllocationMap **out_process);
};

}  // namespace systemc
}  // namespace simics

#endif  // SIMICS_SYSTEMC_MEMORY_PROFILER_H
