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

#ifndef SIMPLE_DIRECTORY_H
#define SIMPLE_DIRECTORY_H

#include "simple-cache.h"

#if defined(__cplusplus)
extern "C" {
#endif
        
#define DIRECTORY_SIZE_LOG2 18
#define DIRECTORY_SIZE (1 << DIRECTORY_SIZE_LOG2)
#define DIRECTORY_SIZE_MASK (DIRECTORY_SIZE - 1)

typedef VECT(cache_line_t *) cache_line_vect_t;

typedef struct directory_entry {
        physical_address_t line_address; // address used in any cache
        cache_line_vect_t present_lines; // caches that possess this cache line
        struct directory_entry *next;    // next conflicting address in hashtab
} directory_entry_t;

typedef struct directory {
        conf_object_t obj;
        struct directory *next_directory;
        physical_address_t cache_size_log2;
        directory_entry_t *entries[DIRECTORY_SIZE];
} directory_t;

typedef struct {
        directory_entry_t *entry;
        directory_entry_t **prev;
} dir_lookup_info_t;

FORCE_INLINE directory_t *
obj_to_dir(conf_object_t *obj)
{
        return (directory_t *)obj;
}

void dir_insert(directory_t *d, physical_address_t pa, struct cache_line *cl);
void dir_remove(directory_t *d, physical_address_t pa, struct cache_line *cl);
dir_lookup_info_t dir_lookup(directory_t *d, physical_address_t pa);

void register_directory_class();

#if defined(__cplusplus)
}
#endif

#endif
