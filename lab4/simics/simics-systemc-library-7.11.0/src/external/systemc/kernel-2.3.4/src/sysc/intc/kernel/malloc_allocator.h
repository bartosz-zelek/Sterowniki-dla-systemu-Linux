/***                                                                       ***/
/***   INTEL CORPORATION PROPRIETARY INFORMATION                           ***/
/***                                                                       ***/
/***   This software is supplied under the terms of a license              ***/
/***   agreement or nondisclosure agreement with Intel Corporation         ***/
/***   and may not be copied or disclosed except in accordance with        ***/
/***   the terms of that agreement.                                        ***/
/***   Copyright 2016-2021 Intel Corporation                               ***/
/***                                                                       ***/

#ifndef KERNEL_MALLOC_ALLOCATOR_H
#define KERNEL_MALLOC_ALLOCATOR_H

#include <cstdlib>
#include <new>
#include <memory>

namespace intc {

template <class T>
class MallocAllocator {
  public:
    virtual T *allocate(const size_t num_elements) const {
        if (num_elements == 0) {
            return NULL;
        }
        if (num_elements > max_size()) {
            throw std::bad_alloc();
        }

        void *addr = malloc(num_elements * sizeof(T));
        if (!addr) {
            throw std::bad_alloc();
        }
        return static_cast<T*>(addr);
    }
    T *allocate(const size_t num_elements, const void *hint) const {
        return allocate(num_elements);
    }
    void deallocate(T * const addr, const size_t n) const {
        free(addr);
    }

    void construct(T * const uninitialized, const T &val) const {
        void * const initialized = static_cast<void*>(uninitialized);
        new(initialized) T(val);
    }
    void destroy(T * const p) const {
        p->~T();
    }

    size_t max_size() const {
        return static_cast<size_t>(-1) / sizeof(T);
    }

    T *address(T &x) const {
        return &x;
    }
    const T *address(const T &x) const {
        return &x;
    }

    bool operator==(const MallocAllocator &other) const {
        return true;
    }
    bool operator!=(const MallocAllocator &other) const {
        return !(*this == other);
    }

    MallocAllocator() {}
    MallocAllocator(const MallocAllocator&) {}
    ~MallocAllocator() {}

    template <typename U>
    MallocAllocator(const MallocAllocator<U> &) {}

    typedef T *pointer;
    typedef const T *const_pointer;

    typedef T &reference;
    typedef const T &const_reference;

    typedef T value_type;
    typedef size_t size_type;
    typedef std::ptrdiff_t difference_type;

    template <typename U> struct rebind {
        typedef MallocAllocator<U> other;
    };
};

}  // namespace intc

#endif
