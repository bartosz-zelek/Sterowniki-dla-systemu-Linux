/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#define _GNU_SOURCE
#include "pipemem.h"
#include "pipeos.h"
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

#if !defined __linux && !defined __FreeBSD__
#error ERROR: Neither __linux nor __FreeBSD__ is defined
#endif

unsigned pipemem_map_flags(int op, unsigned flag)
{
        static unsigned flags =
#if defined __linux
                MAP_PRIVATE | MAP_ANONYMOUS | MAP_LOCKED | MAP_POPULATE;
#elif defined __FreeBSD__
		/* FreeBSD can use MAP_PREFAULT_READ, which might map
		   to MAP_POPULATE. But there is no equivalent to MAP_LOCKED.
                 */
                MAP_PRIVATE | MAP_ANONYMOUS;
#endif
        if (op > 0)
                flags |= flag;
        else if (op < 0)
                flags &= ~flag;
        return flags;
}

int pipemem_map_populate(int op)
{
        static int value = 0;
        if (op)
                value = op > 0;
        return value;
}

size_t
pipemem_page_size()
{
        static size_t pagesize = 0;
        if (!pagesize)
#if defined __linux
                pagesize = sysconf(_SC_PAGE_SIZE);
#elif defined __FreeBSD__
                pagesize = getpagesize();
#endif
        return pagesize;
}

void
pipemem_populate(void *ptr, size_t siz, size_t offs)
{
        char *d = (char *)ptr;
        char *end = d + siz;
        size_t pg = pipemem_page_size();
        for (d += offs; d < end; d += pg) {
                volatile char c = *d;
                *d = c;
        }
}

void *
pipemem_alloc(size_t sz)
{
        static const int prot = PROT_READ | PROT_WRITE;
        int flags = (int)pipemem_map_flags(0, 0);

        void *ptr = mmap(NULL, sz, prot, flags, -1, 0);
        if (ptr == MAP_FAILED)
                return NULL;
        if (pipemem_map_populate(0))
                pipemem_populate(ptr, sz, 0);
        return ptr;
}

void *
pipemem_realloc(void *ptr, size_t sz, size_t new_sz)
{
#if defined __linux
        void *dst = mremap(ptr, sz, new_sz, MREMAP_MAYMOVE);
        if (dst == MAP_FAILED)
                return NULL;
#elif defined __FreeBSD__
        /* mremap is not supported on FreeBSD. The mapping will always move, as
           a new one is created and the data migrated. */
        void *dst = pipemem_alloc(new_sz);
        if (dst == NULL)
                return NULL;
        size_t cpy_sz = sz < new_sz ? sz : new_sz;
        memcpy(dst, ptr, cpy_sz);
        pipemem_free(ptr, sz);
#endif
        if (pipemem_map_populate(0))
                pipemem_populate(dst, new_sz, 0);
        return dst;
}

void
pipemem_free(void *ptr, size_t sz)
{
        munmap(ptr, sz);
}

int
pipeos_errno()
{
        return errno;
}

const char *
pipeos_strerror(int errnum)
{
        int e = errnum < 0 ? -errnum : errnum;
        return strerror(e);
}
