// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <boost/test/unit_test.hpp>

#include <simics/malloc_allocator.h>

#include <cstdlib>
#include <map>
#include <utility>
#include <vector>

using simics::MallocAllocator;

static std::map<void *, std::size_t> allocations;
static std::map<void *, std::size_t>::const_iterator latest;
static std::size_t num_allocations = 0;

template <class T>
class MockAllocator {
  public:
    static void *allocate(std::size_t num) {
        const std::size_t size = num * sizeof(T);
        void *p = malloc(size);
        assert(p);

        latest = allocations.emplace(p, size).first;
        num_allocations++;
        return p;
    }

    static void deallocate(T * const addr) {
        allocations.erase(addr);
        free(addr);
    }
};

struct ResetAllocationsFixture {
    ResetAllocationsFixture() {
        BOOST_REQUIRE_EQUAL(allocations.size(), 0);
    }
    ~ResetAllocationsFixture() {
        num_allocations = 0;
        BOOST_CHECK_EQUAL(allocations.size(), 0);
    }
};

BOOST_FIXTURE_TEST_SUITE(TestAllocator, ResetAllocationsFixture)

BOOST_AUTO_TEST_CASE(Reserve) {
    std::vector<int, MallocAllocator<int, MockAllocator> > v(10);
    BOOST_CHECK_EQUAL(allocations.size(), 1);
    BOOST_CHECK_EQUAL(latest->second, 10 * sizeof(int));
    BOOST_CHECK_EQUAL(num_allocations, 1);
}

template <template <class, class> class Vector, class T, class A>
void grow_to(Vector<T, A> *v, std::size_t size) {
    while (v->size() < size) {
        v->push_back(T());
    }
}

// Growth factor is implementation dependent
BOOST_AUTO_TEST_CASE(DoublingGrowthFactor) {
    std::vector<int> v(0);
    BOOST_CHECK_EQUAL(v.capacity(), 0);

    int capacities[] = {1, 2, 4, 8, 16, 32, 64};
    for (std::size_t capacity = 0;
         capacity < sizeof(capacities) / sizeof(*capacities); ++capacity) {
        grow_to(&v, capacity > 0 ? capacities[capacity - 1] + 1 : 1);
        BOOST_CHECK_EQUAL(v.capacity(), capacities[capacity]);
    }
}

BOOST_AUTO_TEST_CASE(Grow) {
    std::vector<int, MallocAllocator<int, MockAllocator> > v(0);
    BOOST_CHECK_EQUAL(allocations.size(), 0);

    int capacities[] = {1, 2, 4, 8, 16, 32, 64};
    std::size_t num_capacities = sizeof(capacities) / sizeof(*capacities);
    for (std::size_t capacity = 0; capacity < num_capacities; ++capacity) {
        grow_to(&v, capacity > 0 ? capacities[capacity - 1] + 1 : 1);
        BOOST_CHECK_EQUAL(allocations.size(), 1);
        BOOST_CHECK_EQUAL(latest->second, sizeof(int) * capacities[capacity]);  // NOLINT
    }
    BOOST_CHECK_EQUAL(num_allocations, num_capacities);
}

BOOST_AUTO_TEST_SUITE_END()
