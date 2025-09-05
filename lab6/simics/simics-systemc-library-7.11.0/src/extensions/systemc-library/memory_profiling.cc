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
#include <simics/systemc/memory_profiling.h>

#include <new>  // bad_alloc
#include <cstdlib>  // size_t, malloc

using simics::systemc::MemoryProfiler;

MemoryProfiler *getMemoryProfiler() {
    static simics::systemc::MemoryProfiler memory_profiler;
    return &memory_profiler;
}

void *malloc_or_throw(std::size_t size) {
    void *addr = std::malloc(size);
    if (!addr) {
        throw std::bad_alloc();
    }
    return addr;
}

void *operator new(std::size_t size) {
    static MemoryProfiler *memory_profiler = getMemoryProfiler();

    size = size > 0 ? size : 1;
    void *addr = malloc_or_throw(size);
    memory_profiler->recordAllocation(addr, size, false);
    return addr;
}

void *operator new[](std::size_t size) {
    static MemoryProfiler *memory_profiler = getMemoryProfiler();

    size = size > 0 ? size : 1;
    void *addr = malloc_or_throw(size);
    memory_profiler->recordAllocation(addr, size, true);
    return addr;
}

void operator delete(void *addr) throw() {
    if (addr) {
        static MemoryProfiler *memory_profiler = getMemoryProfiler();
        memory_profiler->recordDeletion(addr, false);
        std::free(addr);
    }
}

void operator delete[](void *addr) throw() {
    if (addr) {
        static MemoryProfiler *memory_profiler = getMemoryProfiler();
        memory_profiler->recordDeletion(addr, true);
        std::free(addr);
    }
}
