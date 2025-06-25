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

#include <boost/test/unit_test.hpp>

#include <simics/systemc/memory_profiler.h>

#include "mock/mock_memory_profiler_control.h"

using simics::systemc::MemoryProfiler;

const bool SINGLE_ALLOCATION = false;
const bool ARRAY_ALLOCATION = true;

sc_core::sc_process_b *mock_process =
    reinterpret_cast<sc_core::sc_process_b *>(1);

namespace simics {
namespace systemc {
sc_core::sc_process_b *activeProcess(const AdapterMap &adapters) {
    return mock_process;
}
}  // namespace systemc
}  // namespace simics

struct MemoryProfilerFixture {
    MemoryProfiler mp;
};

BOOST_FIXTURE_TEST_SUITE(TestMemoryProfiler, MemoryProfilerFixture)

void expect_allocation(MemoryProfiler *mp, uint64_t addr, std::size_t size,
                       bool array_form) {
    using simics::systemc::AllocationMap;
    AllocationMap *allocations = mp->allocations(mock_process);
    typename AllocationMap::const_iterator allocation = allocations->find(
        reinterpret_cast<void *>(addr));

    BOOST_REQUIRE_MESSAGE(allocation != allocations->end(),
                          "Expected allocation not found (" << addr << ", "
                          << size << ", " << array_form << ")");
    BOOST_CHECK_EQUAL(allocation->second.addr, reinterpret_cast<void *>(addr));
    BOOST_CHECK_EQUAL(allocation->second.size, size);
    BOOST_CHECK_EQUAL(allocation->second.array_form, array_form);
}

void expect_num_allocations(MemoryProfiler *mp, std::size_t len) {
    BOOST_CHECK_EQUAL(len, mp->allocations(mock_process)->size());
}

void allocate(
    MemoryProfiler *mp, uint64_t addr, std::size_t size, bool array_form) {
    mp->recordAllocation(reinterpret_cast<void *>(addr), size, array_form);
}

void free(MemoryProfiler *mp, uint64_t addr, bool array_form) {
    mp->recordDeletion(reinterpret_cast<void *>(addr), array_form);
}

BOOST_AUTO_TEST_CASE(Empty) {
    expect_num_allocations(&mp, 0);
}

BOOST_AUTO_TEST_CASE(Disabled) {
    allocate(&mp, 100, 10, SINGLE_ALLOCATION);
    expect_num_allocations(&mp, 0);
}

BOOST_AUTO_TEST_CASE(Enable) {
    allocate(&mp, 100, 10, SINGLE_ALLOCATION);
    expect_num_allocations(&mp, 0);

    mp.setProfilingForAdapter(NULL, true);

    allocate(&mp, 100, 10, SINGLE_ALLOCATION);
    expect_allocation(&mp, 100, 10, SINGLE_ALLOCATION);
}

BOOST_AUTO_TEST_CASE(UnrecordedAllocation) {
    allocate(&mp, 100, 100, SINGLE_ALLOCATION);
    allocate(&mp, 200, 100, ARRAY_ALLOCATION);
    expect_num_allocations(&mp, 0);

    mp.setProfilingForAdapter(NULL, true);

    free(&mp, 100, SINGLE_ALLOCATION);
    free(&mp, 200, ARRAY_ALLOCATION);
    expect_num_allocations(&mp, 0);
}

BOOST_AUTO_TEST_CASE(UnrecordedFree) {
    mp.setProfilingForAdapter(NULL, true);

    allocate(&mp, 100, 100, SINGLE_ALLOCATION);
    allocate(&mp, 200, 100, ARRAY_ALLOCATION);
    expect_num_allocations(&mp, 2);

    mp.setProfilingForAdapter(NULL, false);

    free(&mp, 100, SINGLE_ALLOCATION);
    free(&mp, 200, ARRAY_ALLOCATION);
    expect_num_allocations(&mp, 2);
}

BOOST_AUTO_TEST_CASE(Single) {
    mp.setProfilingForAdapter(NULL, true);

    allocate(&mp, 100, 10, SINGLE_ALLOCATION);
    allocate(&mp, 200, 20, SINGLE_ALLOCATION);
    allocate(&mp, 300, 30, SINGLE_ALLOCATION);

    expect_num_allocations(&mp, 3);
    expect_allocation(&mp, 100, 10, SINGLE_ALLOCATION);
    expect_allocation(&mp, 200, 20, SINGLE_ALLOCATION);
    expect_allocation(&mp, 300, 30, SINGLE_ALLOCATION);

    free(&mp, 200, SINGLE_ALLOCATION);

    expect_num_allocations(&mp, 2);
    expect_allocation(&mp, 100, 10, SINGLE_ALLOCATION);
    expect_allocation(&mp, 300, 30, SINGLE_ALLOCATION);

    free(&mp, 100, SINGLE_ALLOCATION);
    free(&mp, 300, SINGLE_ALLOCATION);

    expect_num_allocations(&mp, 0);
}

BOOST_AUTO_TEST_CASE(Array) {
    mp.setProfilingForAdapter(NULL, true);

    allocate(&mp, 9000, 900, ARRAY_ALLOCATION);
    allocate(&mp, 8000, 800, ARRAY_ALLOCATION);
    allocate(&mp, 7000, 700, ARRAY_ALLOCATION);

    expect_num_allocations(&mp, 3);
    expect_allocation(&mp, 9000, 900, ARRAY_ALLOCATION);
    expect_allocation(&mp, 8000, 800, ARRAY_ALLOCATION);
    expect_allocation(&mp, 7000, 700, ARRAY_ALLOCATION);

    free(&mp, 7000, ARRAY_ALLOCATION);

    expect_num_allocations(&mp, 2);
    expect_allocation(&mp, 9000, 900, ARRAY_ALLOCATION);
    expect_allocation(&mp, 8000, 800, ARRAY_ALLOCATION);

    free(&mp, 8000, ARRAY_ALLOCATION);
    free(&mp, 9000, ARRAY_ALLOCATION);

    expect_num_allocations(&mp, 0);
}

BOOST_AUTO_TEST_CASE(Leak) {
    mp.setProfilingForAdapter(NULL, true);

    allocate(&mp, 1000, 100, SINGLE_ALLOCATION);
    allocate(&mp, 2000, 100, ARRAY_ALLOCATION);

    expect_num_allocations(&mp, 2);
    expect_allocation(&mp, 1000, 100, SINGLE_ALLOCATION);
    expect_allocation(&mp, 2000, 100, ARRAY_ALLOCATION);
}

BOOST_AUTO_TEST_CASE(testEndOfSimulation) {
    mp.setProfilingForAdapter(NULL, true);

    // Simulation ends; recording is stopped and adapter overridden regardless
    mp.stopRecording();

    allocate(&mp, 100, 10, SINGLE_ALLOCATION);
    expect_num_allocations(&mp, 0);
}

BOOST_AUTO_TEST_CASE(incompatibleDeleteSingle) {
    mp.setProfilingForAdapter(NULL, true);

    // Expect an exception if an allocation made by the new operator is freed
    // by the delete[] operator
    allocate(&mp, 100, 4, SINGLE_ALLOCATION);
    BOOST_CHECK_THROW(free(&mp, 100, ARRAY_ALLOCATION),
                      std::exception);
}

BOOST_AUTO_TEST_CASE(incompatibleDeleteArray) {
    mp.setProfilingForAdapter(NULL, true);

    // Expect an exception if an allocation made by the new[] operator is freed
    // by the delete[] operator
    allocate(&mp, 100, 16, ARRAY_ALLOCATION);
    BOOST_CHECK_THROW(free(&mp, 100, SINGLE_ALLOCATION),
                      std::exception);
}

BOOST_AUTO_TEST_SUITE_END()
