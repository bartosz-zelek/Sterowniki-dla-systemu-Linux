/*
  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_UTIL_HASHTAB_H
#define SIMICS_UTIL_HASHTAB_H

#include <simics/host-info.h>
#include <simics/base-types.h>

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined PYWRAP && !defined GULP_API_HELP

/* All the structs below are INTERNAL and should not be used directly. */

typedef union {
        uint64 i;
        const char *s;
} ht_key_t;

typedef struct ht_entry_common {
	const void *value;
	struct ht_entry_common *next;
        ht_key_t key;
	unsigned hash;                       /* raw hash value */
} ht_entry_common_t;

typedef struct {
        ht_entry_common_t e;
} ht_int_entry_t;

typedef struct {
        ht_entry_common_t e;
} ht_str_entry_t;

typedef struct {
        unsigned nbuckets;    /* size of buckets array; 0 or a power of 2 */
        unsigned bits;        /* nbuckets > 0 ? log2(nbuckets) : 0 */
	ht_entry_common_t **buckets;  /* array [nbuckets], or NULL */
	unsigned num_elements;     /* total number of elements */
} ht_table_common_t;

typedef struct {
        ht_table_common_t t;
        bool keys_owned;                 /* whether the table owns the keys */
} ht_str_table_t;

typedef struct {
        ht_table_common_t t;
} ht_int_table_t;

#define HT_NULL_COMMON {0, 0, NULL, 0}

/* Initialiser for an empty string-keyed table.
   If keys_owned is true, then the table will maintain the allocation of its
   keys. */
#define HT_STR_NULL(keys_owned) {HT_NULL_COMMON, keys_owned}

/* Initialiser for an empty integer-keyed table. */
#define HT_INT_NULL() {HT_NULL_COMMON}

/* Internal; don't use. */
FORCE_INLINE void
ht_init_table_common(ht_table_common_t *ht)
{
        ht->nbuckets = 0;
        ht->bits = 0;
        ht->buckets = NULL;
        ht->num_elements = 0;
}

/* Initialise an integer-keyed table, making it empty and ready for use.
   This will NOT free existing data. */
FORCE_INLINE void
ht_init_int_table(ht_int_table_t *ht) { ht_init_table_common(&ht->t); }

/* Initialise a string-keyed table, making it empty and ready for use.
   This will NOT free existing data.
   If keys_owned is true, then the table will maintain the allocation of its
   keys. */
FORCE_INLINE void
ht_init_str_table(ht_str_table_t *ht, bool keys_owned)
{ ht_init_table_common(&ht->t); ht->keys_owned = keys_owned; }

/* Internal; don't use. */
FORCE_INLINE void *
ht_value_common(ht_entry_common_t *e) { return (void *)e->value; }

/* Internal, don't use. */
FORCE_INLINE void
ht_set_value_common(ht_entry_common_t *e, const void *value)
{ e->value = value; }

/* Internal, don't use. */
FORCE_INLINE unsigned
ht_num_entries_common(const ht_table_common_t *ht) { return ht->num_elements; }

/* Number of entries in an integer-keyed table. */
FORCE_INLINE unsigned
ht_num_entries_int(const ht_int_table_t *ht)
{return ht_num_entries_common(&ht->t);}

/* Number of entries in a string-keyed table. */
FORCE_INLINE unsigned
ht_num_entries_str(const ht_str_table_t *ht)
{return ht_num_entries_common(&ht->t);}

typedef struct {
        ht_table_common_t *ht;
        unsigned index;               /* current bucket */
        ht_entry_common_t *e;         /* current entry */
        ht_entry_common_t *next;      /* next entry (current may be removed) */
} ht_iter_common_t;

typedef struct {
        ht_iter_common_t it;
} ht_int_iter_t;

typedef struct {
        ht_iter_common_t it;
} ht_str_iter_t;

/* The key of the current element of an iterator on an integer-keyed table. */
FORCE_INLINE uint64
ht_iter_int_key(ht_int_iter_t it) { return it.it.e->key.i; }

/* The key of the current element of an iterator on a string-keyed table.
   The returned string is still owned by the table. */
FORCE_INLINE const char *
ht_iter_str_key(ht_str_iter_t it) { return it.it.e->key.s; }

/* The value of the current element of an iterator on an integer-keyed table. */
FORCE_INLINE void *
ht_iter_int_value(ht_int_iter_t it) { return ht_value_common(it.it.e); }

/* The value of the current element of an iterator on a string-keyed table. */
FORCE_INLINE void *
ht_iter_str_value(ht_str_iter_t it) { return ht_value_common(it.it.e); }

/* Set the value of the current element of an iterator on an integer-keyed
   table. The old value is overwritten without being freed in any way. */
FORCE_INLINE void
ht_iter_int_set_value(ht_int_iter_t it, const void *value)
{ ht_set_value_common(it.it.e, value); }

/* Set the value of the current element of an iterator on a string-keyed
   table. The old value is overwritten without being freed in any way. */
FORCE_INLINE void
ht_iter_str_set_value(ht_str_iter_t it, const void *value)
{ ht_set_value_common(it.it.e, value); }

/* Internal; don't use. */
FORCE_INLINE ht_iter_common_t
ht_make_iter(ht_table_common_t *ht, unsigned index,
             ht_entry_common_t *e, ht_entry_common_t *next)
{
        ht_iter_common_t it = {ht, index, e, next};
        return it;
}

/* Internal; don't use. */
FORCE_INLINE ht_iter_common_t
ht_iter_common(ht_table_common_t *ht)
{
        unsigned n = ht->nbuckets;
        unsigned i;
        for (i = 0; i < n; i++) {
                ht_entry_common_t *e = ht->buckets[i];
                if (e)
                        return ht_make_iter(ht, i, e, e->next);
        }
        return ht_make_iter(ht, n, NULL, NULL);
}

/* Create a new iterator for an integer-keyed table, set at the beginning. */
FORCE_INLINE ht_int_iter_t
ht_int_iter(ht_int_table_t *ht)
{ ht_int_iter_t it; it.it = ht_iter_common(&ht->t); return it; }

/* Create a new iterator for a string-keyed table, set at the beginning. */
FORCE_INLINE ht_str_iter_t
ht_str_iter(ht_str_table_t *ht)
{ ht_str_iter_t it; it.it = ht_iter_common(&ht->t); return it; }

/* Internal; don't use. */
FORCE_INLINE bool
ht_at_end_common(ht_iter_common_t it) {return it.index >= it.ht->nbuckets;}

/* Tell whether an integer-keyed iterator has reached the end of the table. */
FORCE_INLINE bool
ht_int_at_end(ht_int_iter_t it) { return ht_at_end_common(it.it); }

/* Tell whether a string-keyed iterator has reached the end of the table. */
FORCE_INLINE bool
ht_str_at_end(ht_str_iter_t it) { return ht_at_end_common(it.it); }

/* Internal; don't use. */
FORCE_INLINE ht_iter_common_t
ht_next_common(ht_iter_common_t it)
{
        unsigned n, i;
        if (it.next)
                return ht_make_iter(it.ht, it.index, it.next, it.next->next);
        n = it.ht->nbuckets;
        for (i = it.index + 1; i < n; i++) {
                ht_entry_common_t *e = it.ht->buckets[i];
                if (e)
                        return ht_make_iter(it.ht, i, e, e->next);
        }
        return ht_make_iter(it.ht, n, NULL, NULL);
}

/* Return an iterator (on an integer-keyed table) to the next element after the
   argument. May only be used if ht_int_at_end(it) is false.
   The table must not have been modified since the argument was created. */
FORCE_INLINE ht_int_iter_t
ht_int_next(ht_int_iter_t it)
{ ht_int_iter_t ii; ii.it = ht_next_common(it.it); return ii; }

/* Return an iterator (on a string-keyed table) to the next element after the
   argument. May only be used if ht_int_at_end(it) is false.
   The table must not have been modified since the argument was created. */
FORCE_INLINE ht_str_iter_t
ht_str_next(ht_str_iter_t it)
{ ht_str_iter_t si; si.it = ht_next_common(it.it); return si; }

/* Convenience macro for iterating over the integer-keyed table 'ht', setting
   'it' to each element in turn. The table must not be modified during
   iteration. */
#define HT_FOREACH_INT(ht, it)                                          \
    for (ht_int_iter_t it = ht_int_iter(ht); !ht_int_at_end(it);        \
         it = ht_int_next(it))

/* Convenience macro for iterating over the string-keyed table 'ht', setting
   'it' to each element in turn. The table must not be modified during
   iteration. */
#define HT_FOREACH_STR(ht, it)                                          \
    for (ht_str_iter_t it = ht_str_iter(ht); !ht_str_at_end(it);        \
         it = ht_str_next(it))

void ht_clear_int_table(ht_int_table_t *ht, bool free_vals);
void ht_clear_str_table(ht_str_table_t *ht, bool free_vals);
void ht_delete_int_table(ht_int_table_t *ht, bool free_values);
void ht_delete_str_table(ht_str_table_t *ht, bool free_values);
void ht_insert_int(ht_int_table_t *ht, uint64 key, const void *value);
void ht_insert_str(ht_str_table_t *ht, const char *key, const void *value);
void *ht_update_int(ht_int_table_t *ht, uint64 key, const void *value);
void *ht_update_str(ht_str_table_t *ht, const char *key, const void *value);
int ht_remove_int(ht_int_table_t *ht, uint64 key);
int ht_remove_str(ht_str_table_t *ht, const char *key);
ht_int_entry_t *ht_entry_lookup_int(const ht_int_table_t *ht, uint64 key);
ht_str_entry_t *ht_entry_lookup_str(const ht_str_table_t *ht, const char *key);
void *ht_lookup_int(const ht_int_table_t *ht, uint64 key);
void *ht_lookup_str(const ht_str_table_t *ht, const char *key);

int ht_for_each_entry_int(ht_int_table_t *table,
                          int (*f)(ht_int_table_t *table,
                                   uint64 key, void *value, void *data),
                          void *data);
int ht_for_each_entry_str(ht_str_table_t *table,
                          int (*f)(ht_str_table_t *table,
                                   const char *key, void *value, void *data),
                          void *data);

#endif  /* not PYWRAP and not GULP_API_HELP */

#if defined(__cplusplus)
}
#endif

#endif
