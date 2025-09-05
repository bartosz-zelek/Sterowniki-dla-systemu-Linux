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

#include "simple-cache.h"
#include "simple-directory.h"

#include <simics/simulator-api.h>
#include <simics/model-iface/cpu-instrumentation.h>
#include <simics/arch/x86.h>
#include <simics/base/conf-object.h>

#include <stdio.h>

#define INC_STAT(c, x)

/* log groups */
enum {
        GC_Log_Read_Hit   = 0x01, /* Read hit transactions */
        GC_Log_Read_Miss  = 0x02, /* Read miss transactions */
        GC_Log_Write_Hit  = 0x04, /* Write hit transactions */
        GC_Log_Write_Miss = 0x08, /* Write miss transactions */
        GC_Log_Cache      = 0x10, /* Other cache activities */
        GC_Log_Repl       = 0x20, /* Replacement policies */
        GC_Log_MESI       = 0x40, /* MESI protocol */
        GC_Log_Attr       = 0x80, /* Cache attributes */
};

const char *const cache_log_groups[] = {
        "Read Hit Path",
        "Read Miss Path",
        "Write Hit Path",
        "Write Miss Path",
        "Misc. Cache Activities",
        "Replacement Policies",
        "MESI Protocol",
        "Attributes",
        NULL
};

#define ACCESS_TYPE_STR(s)  #s ,

static const char *access_str[] =
{  FOR_ACCESS_TYPE(ACCESS_TYPE_STR) };

static const char *cache_line_state_str[] = {
        "invalid", "shared", "exclusive", "modified"
};

conf_class_t *simple_cache;

/*
   Invalidates a cache line, performing a write-back if the cache line
   is modified.
*/
static cycles_t
empty_line(cache_t *c, cache_line_t *line)
{
        int penalty = 0;

        /* write-back, a write through cache does not have Modified */
        if (line->state == CL_Modified) {
                c->stat.evicted_modified++;
                if (c->next.obj) {
                        SIM_LOG_INFO(3, &c->obj, GC_Log_Repl,
                                     "Evict modified line 0x%llx to next level",
                                     pa_from_line(line));

                        penalty += c->next.iface->write(
                                c->next.obj, &c->obj, Sim_Initiator_Cache,
                                pa_from_line(line), c->config.block_size,
                                Access_Type_Write_Back);
                } else {
                        SIM_LOG_INFO(3, &c->obj, GC_Log_Repl,
                                     "Evict modified line 0x%llx to memory",
                                     pa_from_line(line));
                }
                penalty += c->config.write_miss_penalty;
        }

        if (line->state != CL_Invalid) {
                if (line->state != CL_Modified) {
                        SIM_LOG_INFO(3, &c->obj, GC_Log_Repl,
                                     "Evict %s line (0x%llx)",
                                     cache_line_state_str[line->state],
                                     pa_from_line(line));
                }

                dir_remove(c->directory, pa_from_line(line), line);
                line->state = CL_Invalid;
                line->ip_prefetched = false;
                line->prefetched = false;
                c->stat.evicted_total++;
        }

        return penalty;
}

/* Look for a cache line matching the address pa. Returns the cache line if
   found. Also move (bubble) the found line to the top of the lookup order. */
static cache_line_t *
lookup_line(cache_t *c, physical_address_t pa)
{
        /* LRU replacement */

        uint64 tag = tag_from_pa(c, pa);
        uint64 idx = index_from_pa(c, pa);
        uint64 set = idx * c->config.ways;

        for (int64 i = 0; i < c->config.ways; i++) {
                uint8 *set_order = &c->lookup_order[set];
                cache_line_t *line = &c->lines[set] + set_order[i];
                if (line->state != CL_Invalid && line->tag == tag) {
                        /* bubble the lookup order to the MRU position */
                        for (int64 j = i; j >= 1; j--) {
                                uint8 tmp = set_order[j - 1];
                                set_order[j - 1] = set_order[j];
                                set_order[j] = tmp;
                        }
                        return line;
                }
        }
        return NULL;
}

/* On a miss, find the LRU line to use, evict it with empty_line above. Also,
   move the line in the lookup_order array to MRU position.
 */
static cache_line_t *
get_replacement_line(cache_t *c, physical_address_t pa)
{
        /* LRU replacement */
        uint64 idx = index_from_pa(c, pa);
        uint64 set = idx * c->config.ways;
        uint8 *set_order = &c->lookup_order[set];

        /* return the last line in the ordered set, LRU position */
        cache_line_t *line = &c->lines[set] + set_order[c->config.ways - 1];

        /* bubble the lookup order for the way to MRU position */
        for (int64 i = c->config.ways - 1; i >= 1; i--) {
                uint8 tmp = set_order[i - 1];
                set_order[i - 1] = set_order[i];
                set_order[i] = tmp;
        }
        return line;
}

/* Invalidate the cache for physical address pa. */
static cycles_t
invalidate(conf_object_t *obj, physical_address_t pa, cache_propagate_t prop)
{
        cache_t *c = obj_to_cache(obj);
        cycles_t penalty = 0;
        cache_line_t *line = lookup_line(c, pa);

        if (line)
                penalty += empty_line(c, line);

        if (prop == Cache_Propagate_Next && c->next.obj) {
                penalty += c->next.iface->invalidate(c->next.obj, pa, prop);
        } else if (prop == Cache_Propagate_Prev) {
                VFORI(c->prev, i) {
                        if (VGET(c->prev, i).obj)
                                penalty += VGET(c->prev, i).iface->invalidate(
                                        VGET(c->prev, i).obj, pa, prop);
                }
        }
        return penalty;
}

/* Invalidate all cache lines. */
static cycles_t
invalidate_all(conf_object_t *obj, cache_propagate_t prop)
{
        cache_t *c = obj_to_cache(obj);
        uint64 penalty = 0;

        for (int i = 0; i < c->config.line_number; i++)
                penalty += empty_line(c, &c->lines[i]);

        if (prop == Cache_Propagate_Next && c->next.obj) {
                penalty += c->next.iface->invalidate_all(c->next.obj, prop);
        } else if (prop == Cache_Propagate_Prev) {
                VFORI(c->prev, i) {
                        if (VGET(c->prev, i).obj)
                                penalty += VGET(
                                        c->prev, i).iface->invalidate_all(
                                VGET(c->prev, i).obj, prop);
                }
        }
        return penalty;
}

static cycles_t
mesi_snoop_read(cache_t *c, cache_line_t *line)
{
        int penalty = 0;

        switch (line->state) {
        case CL_Shared:
                break;
        case CL_Exclusive:
                SIM_LOG_INFO(
                        3, &c->obj, GC_Log_MESI,
                        "mesi_receive: snoop read hit "
                        "on an exclusive line");
                line->state = CL_Shared;
                INC_STAT(c, mesi_exclusive_to_shared);
                break;
        case CL_Modified:
                SIM_LOG_INFO(
                        3, &c->obj, GC_Log_MESI,
                        "mesi_receive: snoop read hit "
                        "on a modified line");
                if (c->next.obj) {
                        penalty += c->next.iface->write(c->next.obj, &c->obj,
                                                        Sim_Initiator_Cache,
                                                        pa_from_line(line),
                                                        c->config.block_size,
                                                        Access_Type_Write_Back);
                }
                penalty += c->config.write_miss_penalty;
                line->state = CL_Shared;
                INC_STAT(gc, mesi_modified_to_shared);
                break;
        default:
                SIM_LOG_ERROR(
                        &c->obj, GC_Log_MESI,
                        "Cache line in unexpected in snoop read hit.");
        }

        return penalty;
}

/*  Receives transaction from coherent caches. */
static cycles_t
mesi_snoop_transaction(conf_object_t *obj, physical_address_t pa,
                       bus_op_t busop)
{
        cache_t *c = obj_to_cache(obj);
        int penalty = 0;

        /* first, propagate the snoop to higher level caches */
        if (c->next.obj)
                penalty += c->next.iface->mesi_snoop(c->next.obj, pa, busop);

        penalty += c->config.snoop_penalty;
        dir_lookup_info_t dli = dir_lookup(c->directory, pa);
        if (!dli.entry)
                return penalty;

        cache_line_vect_t invalidate = VNULL;
        VFORP(dli.entry->present_lines, cache_line_t, line) {
                ASSERT(line->state != CL_Invalid);
                if (&line->owner->obj == obj)
                        continue; // we do not want to snoop us
                if (busop == Bus_Op_Rd_X) {
                        SIM_LOG_INFO(3, &c->obj, GC_Log_MESI,
                                     "mesi_receive: snoop request"
                                     " for ownership");

                        INC_STAT(c, mesi_invalidate);
                        VADD(invalidate, line);
                } else {
                        penalty += mesi_snoop_read(line->owner, line);
                }
        }

        /* We collect the lines to invalidate above, we should not remove them
           from the vector while we are iterating over it. */
        while (VLEN(invalidate)) {
                cache_line_t *line = VPOP(invalidate);
                penalty += empty_line(line->owner, line);
        }
        return penalty;
}

/* Handle instruction pointer prefetch for load and stores. For each address
   keep 3 cache blocks to be able to compute the distance between each access.
   Mark current blocks with the ip_prefetch flag. This is used when looking up
   a cache block. A cache hit in a marked cache block invokes this function,
   otherwise it will not be invoked because of performance reasons. See
   handle_read_prefetch and handle_write_prefetch functions below. */
static cycles_t
handle_ip_prefetch(cache_t *c, conf_object_t *ini, ini_type_t ini_type,
                   cache_line_t *block, access_type_t type)
{
        if (c->cpu.obj != ini && ini_type == Sim_Initiator_CPU) {
                const int_register_interface_t *ir_iface = SIM_get_interface(
                        ini, "int_register");
                if (ir_iface) {
                        c->cpu.ir_iface = ir_iface;
                        c->cpu.obj = ini;
                        c->cpu.rip = c->cpu.ir_iface->get_number(
                                c->cpu.obj, "rip");
                } else {
                        (void)SIM_clear_exception();
                        return 0;
                }
        }

        logical_address_t rip = c->cpu.ir_iface->read(
                c->cpu.obj, c->cpu.rip);
        prefetch_entry_t *pe;
        if (type == Access_Type_Prefetch)
                pe = &c->prefetcher.read_entries[rip & c->prefetcher.mask];
        else
                pe = &c->prefetcher.write_entries[rip & c->prefetcher.mask];

        if (block == pe->blocks[2])
                return 0; // ignore access to same block

        /* keep short history of accessed blocks */

        pe->blocks[0]->ip_prefetched = false; // forget prefetch status

        pe->blocks[0] = pe->blocks[1];
        pe->blocks[1] = pe->blocks[2];
        pe->blocks[2] = block;

        /* Calculate address diffs. */
        physical_address_t d1 = pe->blocks[1]->tag - pe->blocks[0]->tag;
        physical_address_t d2 = pe->blocks[2]->tag - pe->blocks[1]->tag;

        cycles_t p = 0;
        cache_line_t *next_block;

        /* Check distance between accesses */
        if (d1 == d2) {
                // we have a stride
                if (type == Access_Type_Prefetch) {
                        SIM_LOG_INFO(3, &c->obj, GC_Log_Cache,
                                     "Prefetch with stride %lld (0x%llx)",
                                     d1, pa_from_line(block));
                        p = handle_read(
                                &c->obj, ini, ini_type,
                                (block->tag + d1) << c->config.block_size_log2,
                                c->config.block_size,
                                Access_Type_Prefetch,
                                &next_block);
                } else {
                        SIM_LOG_INFO(3, &c->obj, GC_Log_Cache,
                                     "Read for ownership prefetch with"
                                     " stride %lld (0x%llx)",
                                     d1, pa_from_line(block));
                        p = handle_write(
                                &c->obj, ini, ini_type,
                                (block->tag + d1) << c->config.block_size_log2,
                                c->config.block_size,
                                Access_Type_Prefetch_Read_For_Ownership,
                                &next_block);
                }
                if (next_block)
                        next_block->ip_prefetched = true;
        }
        return p;
}

/* Handle prefetch of adjacent line and additional lines if configured. */
static void
handle_simple_prefetch(cache_t *c, conf_object_t *ini, ini_type_t ini_type,
                       physical_address_t pa)
{
        cache_line_t *line; // not used

        if (c->config.prefetch_adjacent) {
                handle_read(
                        &c->obj, ini, ini_type,
                        pa ^ (1 << c->config.block_size_log2),
                        c->config.block_size,
                        Access_Type_Prefetch,
                        &line);
        }
        if (c->config.prefetch_additional) {
                for (int i = 0; i < c->config.prefetch_additional; i++) {
                        handle_read(
                                &c->obj, ini, ini_type,
                                pa + c->config.block_size * (i + 1),
                                c->config.block_size,
                                Access_Type_Prefetch,
                                &line);
                }
        }
}

/* Used to return a not used line in case of a write through cache. */
static cache_line_t unused_line = { 0 };


/* Handle read misses in the cache */
static cycles_t
handle_read_miss(cache_t *c, conf_object_t *ini, ini_type_t ini_type,
                 physical_address_t pa, unsigned size, access_type_t access,
                 cache_line_t **used_line)
{
        cycles_t penalty = 0;
        cache_line_t *line = NULL;

        /* Update the statistics. */
        c->stat.access_miss[access]++;

        /* Choose a line to use, empty it if any current content and fetch its
           new content. */
        line = get_replacement_line(c, pa);
        penalty += empty_line(c, line);

        /* Check directory for peer lines matching pa */
        dir_lookup_info_t dli = dir_lookup(c->directory, pa);
        if (!dli.entry || VEMPTY(dli.entry->present_lines)) {
                /* No peer lines, so this is a bus transaction "BusRd !Shared",
                   no action required. */

                /* Fetch from next level. */
                if (c->next.obj)
                        penalty += c->next.iface->read(
                                c->next.obj, ini, ini_type, pa,
                                size, access);
                else
                        penalty += c->config.read_miss_penalty;

                line->state = CL_Exclusive;
                dir_insert(c->directory, pa, line);

                SIM_LOG_INFO(3, &c->obj, GC_Log_Read_Miss,
                             "New exclusive line (0x%llx)", pa);
        } else {
                /* Bus transaction is BusRd Shared, we need to broadcast read to
                   the other peer caches, "return" data from one of them */
                penalty += mesi_snoop_transaction(&c->obj, pa, Bus_Op_Rd);

                line->state = CL_Shared;
                dir_insert(c->directory, pa, line);

                SIM_LOG_INFO(3, &c->obj, GC_Log_Read_Miss,
                             "New shared line (0x%llx)", pa);
        }

        line->tag = tag_from_pa(c, pa);
        line->owner = c;
        line->ip_prefetched = false;
        line->prefetched = access == Access_Type_Prefetch;

        *used_line = line;

        if (access != Access_Type_Prefetch)
                handle_simple_prefetch(c, ini, ini_type, pa);

        return penalty;
}

/* Handles read transactions coming to the cache. */
cycles_t
handle_read(conf_object_t *obj, conf_object_t *ini, ini_type_t ini_type,
            physical_address_t pa, unsigned size, access_type_t access,
            cache_line_t **used_line)
{
        cache_t *c = obj_to_cache(obj);
        int penalty = 0;

        /* Count access count for this type */
        c->stat.access_total[access]++;

        /* Look for a matching line */
        cache_line_t *line = lookup_line(c, pa);
        penalty += c->config.read_penalty; // set a read penalty for everything

        if (line) {
                /* Cache hit, no bus transaction required */

                SIM_LOG_INFO(3, &c->obj, GC_Log_Read_Hit,
                             "%s transaction is a hit (0x%llx)",
                             access_str[access], pa);
                if (line->prefetched) {
                        if (access != Access_Type_Prefetch) {
                                line->prefetched = false;
                                c->stat.prefetches_used++;
                        }
                }
                *used_line = line;
        } else {
                /* No, we have a miss. */
                SIM_LOG_INFO(3, &c->obj, GC_Log_Read_Miss,
                             "%s transaction is a miss (0x%llx)",
                             access_str[access], pa);

                penalty += handle_read_miss(c, ini, ini_type, pa, size,
                                            access, used_line);
        }

        return penalty;
}

/* Handles read transactions coming to the cache and also read prefetching. */
static cycles_t
handle_read_prefetch(conf_object_t *obj, conf_object_t *ini, ini_type_t it,
                     physical_address_t pa, unsigned size, access_type_t access)
{
        cache_t *c = obj_to_cache(obj);
        cache_line_t *line;
        uint64 misses = c->stat.access_miss[access];
        cycles_t penalty = handle_read(obj, ini, it, pa, size, access, &line);

        /* Only runt the prefetcher if we hit a prefetched line currently in a
           stride, or if we missed */
        if ((line->ip_prefetched || misses < c->stat.access_miss[access])
            && c->config.ip_read_prefetcher
            && access != Access_Type_Prefetch)
                handle_ip_prefetch(c, ini, it, line, Access_Type_Prefetch);

        return penalty;
}

/* Handle write misses in the cache */
static cycles_t
handle_write_miss(cache_t *c, conf_object_t *ini, ini_type_t ini_type,
                  physical_address_t pa, unsigned size, access_type_t access,
                  cache_line_t **used_line)
{
        /* Update the statistics. */
        c->stat.access_miss[access]++;

        /* We need to snoop peer lines on misses */
        cycles_t penalty = mesi_snoop_transaction(&c->obj, pa, Bus_Op_Rd_X);

        if (c->config.write_allocate) {
                /* If we have a write-allocate cache we choose a line to use,
                   empty it if it has any current contents and fetch its new
                   contents. */
                cache_line_t *line = get_replacement_line(c, pa);
                penalty += empty_line(c, line);

                line->tag = tag_from_pa(c, pa);
                line->owner = c;
                line->ip_prefetched = false;
                line->prefetched =
                        access == Access_Type_Prefetch_Read_For_Ownership;

                /* new line, insert into directory */
                dir_insert(c->directory, pa, line);

                /* Assume a write back will cover the entire line so
                   we need not fetch any data from next level if write back.
                   Only pass normal writes and RFO (as RFO) and prefetches. */
                if (access != Access_Type_Write_Back) {
                        if (c->next.obj) {

                                SIM_LOG_INFO(3, &c->obj, GC_Log_Write_Miss,
                                             "Send read for ownership (0x%llx)",
                                             pa);
                                access_type_t prfo;
                                prfo = Access_Type_Prefetch_Read_For_Ownership;
                                penalty += c->next.iface->write(
                                        c->next.obj, ini, ini_type, pa,
                                        c->config.block_size,
                                        access == prfo ?
                                        prfo : Access_Type_Read_For_Ownership);
                        }
                        penalty += c->config.write_miss_penalty;
                }

                SIM_LOG_INFO(3, &c->obj, GC_Log_Write_Miss,
                             "Write allocate line 0x%llx", pa);

                *used_line = line;
        }
        return penalty;
}

cycles_t
handle_write(conf_object_t *obj, conf_object_t *ini, ini_type_t ini_type,
             physical_address_t pa, unsigned size, access_type_t access,
             cache_line_t **used_line)
{
        cache_t *c = obj_to_cache(obj);
        int penalty = 0;

        c->stat.access_total[access]++;

        /* Look for a matching line. */
        cache_line_t *line = lookup_line(c, pa);
        penalty += c->config.write_penalty;

        if (line) {
                /* Yes, we have a hit. */
                SIM_LOG_INFO(3, &c->obj, GC_Log_Write_Hit,
                             "%s transaction is a hit (0x%08llx)",
                             access_str[access], pa);

                /* Hit, we must broadcast hit depending on the line status */
                if (!CL_IS_PRIVATE(line->state)) // !Modified or !Exclusive
                        penalty += mesi_snoop_transaction(obj, pa, Bus_Op_Rd_X);
                if (line->prefetched) {
                        if (access != Access_Type_Prefetch_Read_For_Ownership) {
                                line->prefetched = false;
                                c->stat.prefetches_used++;
                        }
                }
        } else {
                /* A miss! Follow the following scheme:

                   WT  = Write Through
                   WA  = Write Allocate
                   NWA = Non Write Allocate
                   WB  = Write Back

                   WT, NWA -> just WT
                   WT,  WA -> WT + line allocated Exclusive
                   WB, NWA -> just WT
                   WB,  WA -> send Read For Ownership and allocate line Modified
                */

                SIM_LOG_INFO(3, &c->obj, GC_Log_Write_Miss,
                             "%s transaction is a miss (0x%08llx)",
                             access_str[access], pa);

                penalty += handle_write_miss(
                        c, ini, ini_type, pa, size, access, &line);
        }

        /* If we have a write-through cache or we have a miss in a
           non-write-allocate cache we need to send the transaction to
           the next level cache. */
        if (!c->config.write_back || line == NULL) {

                SIM_LOG_INFO(3, &c->obj,
                             ((!c->config.write_back)
                              ? GC_Log_Write_Miss | GC_Log_Write_Hit
                              : GC_Log_Write_Miss),
                             "Write through to %s (0x%llx)",
                             c->next.obj ? "next level" : "memory", pa);

                /* count a write-next penalty */
                if (c->next.obj)
                        penalty += c->next.iface->write(
                                c->next.obj, ini, ini_type, pa, size, access);
                penalty += c->config.write_miss_penalty;
        }

        /* Check if we used a cache line. */
        if (line) {
                /* The line is now in modified or exclusive state,
                   depending on if the cache is write-back. */
                if (c->config.write_back)
                        line->state = CL_Modified;
                else
                        line->state = CL_Exclusive;
        }
        *used_line = line ? line : &unused_line;

        return penalty;
}

/* Handles write transactions coming to the cache and also write prefetching. */
static cycles_t
handle_write_prefetch(conf_object_t *obj, conf_object_t *ini, ini_type_t it,
                      physical_address_t pa, unsigned size,
                      access_type_t access)
{
        cache_t *c = obj_to_cache(obj);
        cache_line_t *line;
        uint64 misses = c->stat.access_miss[access];
        cycles_t penalty = handle_write(obj, ini, it, pa, size, access, &line);

        /* Only runt the prefetcher if we hit a prefetched line currently in a
           stride, or if we missed */
        if (line
            && (line->ip_prefetched || misses < c->stat.access_miss[access])
            && c->config.ip_write_prefetcher
            && access != Access_Type_Prefetch_Read_For_Ownership)
                handle_ip_prefetch(c, ini, it, line,
                                   Access_Type_Prefetch_Read_For_Ownership);

        return penalty;
}

/* Computes the log2 of a 32 bit integer */
static int
log2_int32(uint32 value)
{
	 int i;
	 for (i = 0; i < 32; i++) {
		 value >>= 1;
	     if (value == 0)
	    	 break;
	 }
	 return i;
}

static void
reset_statistics(cache_t *c)
{
        for (int i = 0; i < Access_Type_Num; i++) {
                c->stat.access_total[i] = 0;
                c->stat.access_miss[i] = 0;
        }
        c->stat.evicted_modified = 0;
        c->stat.evicted_total = 0;
}

/* Allocate the cache */
static conf_object_t *
alloc_object(void *arg)
{
        cache_t *cache = MM_ZALLOC(1, cache_t);

        reset_statistics(cache);

        cache->config.write_back = true;
        cache->config.write_allocate = true;

        cache->prefetcher.size = 1024;
        cache->prefetcher.mask = cache->prefetcher.size - 1;
        cache->prefetcher.read_entries = MM_ZALLOC(
                cache->prefetcher.size, prefetch_entry_t);
        cache->prefetcher.write_entries = MM_ZALLOC(
                cache->prefetcher.size, prefetch_entry_t);

        return &cache->obj;
}

static void
finalize_instance(conf_object_t *obj)
{
        cache_t *c = obj_to_cache(obj);

        /* Finalize cache parameters */
        c->config.tag_shift = c->config.block_size_log2;
        c->config.line_number = c->config.number_of_sets
                        * c->config.ways;
        c->lines = MM_ZALLOC(c->config.line_number, cache_line_t);
        c->lookup_order = MM_ZALLOC(c->config.line_number, uint8);

        /* set the lookup order so way 0 is LRU and will thus be used first */
        for (int s = 0; s < c->config.number_of_sets; s++)
                for (int w = 0; w < c->config.ways; w++)
                        c->lookup_order[s * c->config.ways + w]
                                = c->config.ways - w - 1;

        /* let prefetcher blocks point to something in the beginning */
        for (int i = 0; i < c->prefetcher.size; i++) {
                for (int j = 0; j < 3; j++) {
                        c->prefetcher.read_entries[i].blocks[j] = &c->lines[0];
                        c->prefetcher.write_entries[i].blocks[j] = &c->lines[0];
                }
        }
}

bool
object_exists(char *name)
{
        if (!name)
                return false;
        SIM_get_object(name);
        if (SIM_clear_exception())
                return false;
        return true;
}

static void
pre_delete_instance(conf_object_t *obj)
{
        cache_t *c = obj_to_cache(obj);

        if (c->directory && object_exists(c->directory_name))
                SIM_delete_object(&c->directory->obj);

        if (c->next.obj && object_exists(c->next.name))
                SIM_delete_object(c->next.obj);

        VFREE(c->prev);
}

static int
delete_instance(conf_object_t *obj)
{
        cache_t *c = obj_to_cache(obj);
        MM_FREE(c->next.name);
        MM_FREE(c->directory_name);

        VFORI(c->prev, i)
                MM_FREE(VGET(c->prev, i).name);
        VFREE(c->prev);

        if (c->prefetcher.read_entries)
                MM_FREE(c->prefetcher.read_entries);
        if (c->prefetcher.write_entries)
                MM_FREE(c->prefetcher.write_entries);
        if (c->lines)
                MM_FREE(c->lines);
        return 0;
}

/* Attributes for statistics */
typedef enum {
        Stat_Data_Write,
        Stat_Data_Write_Miss,
        Stat_Data_Write_Back,
        Stat_Data_Write_Back_Miss,
        Stat_Data_Read,
        Stat_Data_Read_Miss,
        Stat_Data_Read_For_Ownership,
        Stat_Data_Read_For_Ownership_Miss,
        Stat_Data_Prefetch,
        Stat_Data_Prefetch_Miss,
        Stat_Data_Prefetch_Read_For_Ownership,
        Stat_Data_Prefetch_Read_For_Ownership_Miss,
        Stat_Data_Prefetches_Used,
        Stat_Data_Evicted_Modified,
        Stat_Data_Evicted_Total,
        Stat_Instr_Fetch,
        Stat_Instr_Fetch_Miss,
} stat_t;

typedef enum {
        Cache_Block_Size,
        Cache_Set_Number,
        Cache_Ways,
        Read_Penalty,
        Write_Penalty,
        Read_Miss_Penalty,
        Write_Miss_Penalty,
        Snoop_Penalty
} param_t;

static attr_value_t
get_stat(void *data, conf_object_t *obj, attr_value_t *idx)
{
        cache_t *c = obj_to_cache(obj);
        stat_t kind = (stat_t)data;
        attr_value_t ret;

        switch (kind) {

        case Stat_Data_Write:
                ret = SIM_make_attr_int64(
                        c->stat.access_total[Access_Type_Write]);
                break;
        case Stat_Data_Write_Miss:
                ret = SIM_make_attr_int64(
                        c->stat.access_miss[Access_Type_Write]);
                break;
        case Stat_Data_Write_Back:
                ret = SIM_make_attr_int64(
                        c->stat.access_total[Access_Type_Write_Back]);
                break;
        case Stat_Data_Write_Back_Miss:
                ret = SIM_make_attr_int64(
                        c->stat.access_miss[Access_Type_Write_Back]);
                break;
        case Stat_Data_Read:
                ret = SIM_make_attr_int64(
                        c->stat.access_total[Access_Type_Read]);
                break;
        case Stat_Data_Read_Miss:
                ret = SIM_make_attr_int64(
                        c->stat.access_miss[Access_Type_Read]);
                break;
        case Stat_Data_Read_For_Ownership:
                ret = SIM_make_attr_int64(
                        c->stat.access_total[Access_Type_Read_For_Ownership]);
                break;
        case Stat_Data_Read_For_Ownership_Miss:
                ret = SIM_make_attr_int64(
                        c->stat.access_miss[Access_Type_Read_For_Ownership]);
                break;
        case Stat_Data_Prefetch:
                ret = SIM_make_attr_int64(
                        c->stat.access_total[Access_Type_Prefetch]);
                break;
        case Stat_Data_Prefetch_Miss:
                ret = SIM_make_attr_int64(
                        c->stat.access_miss[Access_Type_Prefetch]);
                break;
        case Stat_Data_Prefetch_Read_For_Ownership:
                ret = SIM_make_attr_int64(
                        c->stat.access_total[
                                Access_Type_Prefetch_Read_For_Ownership]);
                break;
        case Stat_Data_Prefetch_Read_For_Ownership_Miss:
                ret = SIM_make_attr_int64(
                        c->stat.access_miss[
                                Access_Type_Prefetch_Read_For_Ownership]);
                break;
        case Stat_Data_Prefetches_Used:
                ret = SIM_make_attr_int64(c->stat.prefetches_used);
                break;
        case Stat_Instr_Fetch:
                ret = SIM_make_attr_int64(
                        c->stat.access_total[Access_Type_Execute]);
                break;
        case Stat_Instr_Fetch_Miss:
                ret = SIM_make_attr_int64(
                        c->stat.access_miss[Access_Type_Execute]);
                break;
        case Stat_Data_Evicted_Modified:
                ret = SIM_make_attr_int64(c->stat.evicted_modified);
                break;
        case Stat_Data_Evicted_Total:
                ret = SIM_make_attr_int64(c->stat.evicted_total);
                break;
        }
        return ret;
}

static attr_value_t
get_cache_param(void *data, conf_object_t *obj, attr_value_t *idx)
{
        cache_t *cache = obj_to_cache(obj);
        param_t kind = (param_t)data;
        attr_value_t ret;

        switch (kind) {

        case Cache_Set_Number:
                ret = SIM_make_attr_int64(cache->config.number_of_sets);
                break;
        case Cache_Block_Size:
                ret = SIM_make_attr_int64(cache->config.block_size);
                break;
        case Cache_Ways:
                ret = SIM_make_attr_int64(cache->config.ways);
                break;
        case Read_Penalty:
                ret = SIM_make_attr_int64(cache->config.read_penalty);
                break;
        case Read_Miss_Penalty:
                ret = SIM_make_attr_int64(cache->config.read_miss_penalty);
                break;
        case Write_Penalty:
                ret = SIM_make_attr_int64(cache->config.write_penalty);
                break;
        case Write_Miss_Penalty:
                ret = SIM_make_attr_int64(cache->config.write_miss_penalty);
                break;
        case Snoop_Penalty:
                ret = SIM_make_attr_int64(cache->config.snoop_penalty);
                break;
        }
        return ret;
}

static set_error_t
set_cache_param(void *data, conf_object_t *obj,
                attr_value_t *val, attr_value_t *idx)
{
        cache_t *cache = obj_to_cache(obj);
        param_t kind = (param_t)data;

        switch (kind) {

        case Cache_Block_Size:
                cache->config.block_size = SIM_attr_integer(*val);
                cache->config.block_size_log2 =
                        log2_int32(cache->config.block_size);
                break;
        case Cache_Set_Number:
                cache->config.number_of_sets = SIM_attr_integer(*val);
                break;
        case Cache_Ways:
                cache->config.ways = SIM_attr_integer(*val);
                break;
        case Read_Penalty:
                cache->config.read_penalty = SIM_attr_integer(*val);
                break;
        case Read_Miss_Penalty:
                cache->config.read_miss_penalty = SIM_attr_integer(*val);
                break;
        case Write_Penalty:
                cache->config.write_penalty = SIM_attr_integer(*val);
                break;
        case Write_Miss_Penalty:
                cache->config.write_miss_penalty = SIM_attr_integer(*val);
                break;
        case Snoop_Penalty:
                cache->config.snoop_penalty = SIM_attr_integer(*val);
                break;
        }
        return Sim_Set_Ok;
}

/* Get next level in the cache hierarchy */
static attr_value_t
get_next(conf_object_t *obj)
{
        cache_t *cache = obj_to_cache(obj);
        if (object_exists(cache->next.name)
            && !SIM_marked_for_deletion(cache->next.obj))
                return SIM_make_attr_object(cache->next.obj);
        return SIM_make_attr_nil();
}

/* Set next level in the cache hierarchy */
static set_error_t
set_next(conf_object_t *obj, attr_value_t *val)
{
        cache_t *cache = obj_to_cache(obj);
        if (SIM_attr_is_nil(*val)) {
                cache->next.obj = NULL;
                return Sim_Set_Ok;
        }
        conf_object_t *next = SIM_attr_object(*val);
        cache->next.obj = next;
        if (cache->next.name)
                MM_FREE(cache->next.name);
        cache->next.name = MM_STRDUP(SIM_object_name(cache->next.obj));
        cache->next.iface = SIM_C_GET_INTERFACE(next, simple_cache);
        if (!cache->next.iface)
                return Sim_Set_Interface_Not_Found;
        return Sim_Set_Ok;
}

/* Get prev level in the cache hierarchy */
static attr_value_t
get_prev(conf_object_t *obj)
{
        cache_t *cache = obj_to_cache(obj);
        attr_value_t ret = SIM_alloc_attr_list(VLEN(cache->prev));
        VFORI(cache->prev, i) {
                cache_info_t ci = VGET(cache->prev, i);
                SIM_attr_list_set_item(
                        &ret, i,
                        object_exists(ci.name) ? SIM_make_attr_nil() :
                        SIM_make_attr_object(ci.obj));
        }
        return ret;
}

/* Set prev level in the cache hierarchy */
static set_error_t
set_prev(conf_object_t *obj, attr_value_t *val)
{
        cache_t *cache = obj_to_cache(obj);
        VFORI(cache->prev, i)
                MM_FREE(VGET(cache->prev, i).name);
        VCLEAR(cache->prev);
        for (int i = 0; i < SIM_attr_list_size(*val); i++) {
                cache_info_t ci;
                ci.obj = SIM_attr_object(SIM_attr_list_item(*val, i));
                ci.name = MM_STRDUP(SIM_object_name(ci.obj));
                ci.iface = SIM_C_GET_INTERFACE(ci.obj, simple_cache);
                VADD(cache->prev, ci);
        }
        return Sim_Set_Ok;
}

/* Set prev policy in the cache hierarchy */
static set_error_t
set_policy(conf_object_t *obj, attr_value_t *val)
{
        cache_t *cache = obj_to_cache(obj);
        const char *s = SIM_attr_string(*val);
        if (strcmp(s, "inclusive") == 0) {
                cache->config.policy = Policy_Inclusive;
        } else if (strcmp(s, "exclusive") == 0) {
                SIM_LOG_ERROR(obj, 0, "Cache policy exclusive not implemented");
                cache->config.policy = Policy_Exclusive;
        } else if (strcmp(s, "nine") == 0) {
                cache->config.policy = Policy_NINE;
        } else {
                return Sim_Set_Illegal_Value;
        }
        return Sim_Set_Ok;
}

static attr_value_t
get_policy(conf_object_t *obj)
{
        cache_t *cache = obj_to_cache(obj);
        switch (cache->config.policy) {
        case Policy_Inclusive:
                return SIM_make_attr_string("inclusive");
        case Policy_Exclusive:
                return SIM_make_attr_string("exclusive");
        case Policy_NINE:
                return SIM_make_attr_string("nine");
        }
        ASSERT(0);
        return SIM_make_attr_string("");
}

/* Set prev policy in the cache hierarchy */
static set_error_t
set_level(conf_object_t *obj, attr_value_t *val)
{
        cache_t *cache = obj_to_cache(obj);
        cache->level = SIM_attr_integer(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_level(conf_object_t *obj)
{
        cache_t *cache = obj_to_cache(obj);
        return SIM_make_attr_uint64(cache->level);
}


/* Set prefetcher */
static set_error_t
set_ip_read_prefetcher(conf_object_t *obj, attr_value_t *val)
{
        cache_t *cache = obj_to_cache(obj);
        cache->config.ip_read_prefetcher = SIM_attr_boolean(*val);

        return Sim_Set_Ok;
}

static attr_value_t
get_ip_read_prefetcher(conf_object_t *obj)
{
        cache_t *cache = obj_to_cache(obj);
        return SIM_make_attr_boolean(cache->config.ip_read_prefetcher);
}

static set_error_t
set_ip_write_prefetcher(conf_object_t *obj, attr_value_t *val)
{
        cache_t *cache = obj_to_cache(obj);
        cache->config.ip_write_prefetcher = SIM_attr_boolean(*val);

        return Sim_Set_Ok;
}

static attr_value_t
get_ip_write_prefetcher(conf_object_t *obj)
{
        cache_t *cache = obj_to_cache(obj);
        return SIM_make_attr_boolean(cache->config.ip_write_prefetcher);
}

static set_error_t
set_prefetch_adjacent(conf_object_t *obj, attr_value_t *val)
{
        cache_t *cache = obj_to_cache(obj);
        cache->config.prefetch_adjacent = SIM_attr_boolean(*val);

        return Sim_Set_Ok;
}

static attr_value_t
get_prefetch_adjacent(conf_object_t *obj)
{
        cache_t *cache = obj_to_cache(obj);
        return SIM_make_attr_boolean(cache->config.prefetch_adjacent);
}

static set_error_t
set_prefetch_additional(conf_object_t *obj, attr_value_t *val)
{
        cache_t *cache = obj_to_cache(obj);
        cache->config.prefetch_additional = SIM_attr_integer(*val);

        return Sim_Set_Ok;
}

static attr_value_t
get_prefetch_additional(conf_object_t *obj)
{
        cache_t *cache = obj_to_cache(obj);
        return SIM_make_attr_uint64(cache->config.prefetch_additional);
}

static attr_value_t
get_conn(conf_object_t *obj)
{
        cache_t *cache = obj_to_cache(obj);
        if (SIM_marked_for_deletion(cache->conn))
                return SIM_make_attr_nil();
        return SIM_make_attr_object(cache->conn);
}

static set_error_t
set_conn(conf_object_t *obj, attr_value_t *val)
{
        cache_t *c = obj_to_cache(obj);
        conf_object_t *conn = SIM_attr_object(*val);
        c->conn = conn;
        return Sim_Set_Ok;
}

static attr_value_t
get_directory(conf_object_t *obj)
{
        cache_t *cache = obj_to_cache(obj);
        return SIM_make_attr_object(&cache->directory->obj);
}

static set_error_t
set_directory(conf_object_t *obj, attr_value_t *val)
{
        cache_t *c = obj_to_cache(obj);
        if (c->directory_name) {
                MM_FREE(c->directory_name);
                c->directory_name = NULL;
        }
        if (SIM_attr_is_invalid(*val))
                c->directory = NULL;
        else {
                c->directory = obj_to_dir(SIM_attr_object(*val));
                c->directory->cache_size_log2 = c->config.block_size_log2;
                c->directory_name =
                        MM_STRDUP(SIM_object_name(&c->directory->obj));
        }
        return Sim_Set_Ok;
}

static attr_value_t
get_cpu(conf_object_t *obj)
{
        cache_t *c = obj_to_cache(obj);
        return SIM_make_attr_object(c->cpu_thread);
}

static set_error_t
set_cpu(conf_object_t *obj, attr_value_t *val)
{
        cache_t *c = obj_to_cache(obj);
        c->cpu_thread = SIM_attr_object_or_nil(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_write_back(conf_object_t *obj)
{
        cache_t *cache = obj_to_cache(obj);
        return SIM_make_attr_boolean(cache->config.write_back);
}

static set_error_t
set_write_back(conf_object_t *obj, attr_value_t *val)
{
        cache_t *c = obj_to_cache(obj);
        c->config.write_back = SIM_attr_boolean(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_write_allocate(conf_object_t *obj)
{
        cache_t *cache = obj_to_cache(obj);
        return SIM_make_attr_boolean(cache->config.write_allocate);
}

static set_error_t
set_write_allocate(conf_object_t *obj, attr_value_t *val)
{
        cache_t *c = obj_to_cache(obj);
        c->config.write_allocate = SIM_attr_boolean(*val);
        return Sim_Set_Ok;
}

/* Get meta content of cache */
static attr_value_t
get_meta_content(void *_id, conf_object_t *obj, attr_value_t *idx)
{
        cache_t *cache = obj_to_cache(obj);
        int indices = cache->config.number_of_sets * cache->config.ways;
        attr_value_t ret = SIM_alloc_attr_list(indices);
        for (int i = 0; i < indices; i++) {
                uint64 tag = cache->lines[i].tag << cache->config.tag_shift;
                uint64 access = cache->lines[i].access;
                uint64 ip_prefetch = cache->lines[i].ip_prefetched;
                uint64 prefetch = cache->lines[i].prefetched;
                if (cache->lines[i].state == CL_Invalid) {
                        tag = 0;
                        access = 0;
                        prefetch = 0;
                }
                SIM_attr_list_set_item(
                        &ret, i, SIM_make_attr_list(
                                5,
                                SIM_make_attr_int64(cache->lines[i].state),
                                SIM_make_attr_int64(tag),
                                SIM_make_attr_int64(access),
                                SIM_make_attr_int64(ip_prefetch),
                                SIM_make_attr_int64(prefetch))
                        );
        }
        return ret;
}

void
init_cache()
{
        const class_data_t funcs = {
                .alloc_object = alloc_object,
                .finalize_instance = finalize_instance,
                .pre_delete_instance = pre_delete_instance,
                .delete_instance = delete_instance,
                .description = "Simple cache structure",
                .class_desc = "simple cache structure",
                .kind = Sim_Class_Kind_Pseudo
        };

        simple_cache = SIM_register_class("simple_cache", &funcs);

        static const simple_cache_interface_t sci = {
                .read = handle_read_prefetch,
                .write = handle_write_prefetch,
                .mesi_snoop = mesi_snoop_transaction,
                .invalidate = invalidate,
                .invalidate_all = invalidate_all
        };
        SIM_REGISTER_INTERFACE(simple_cache, simple_cache, &sci);

        /* register the log groups */
        SIM_log_register_groups(simple_cache, cache_log_groups);

        /* register cache attributes */
        SIM_register_typed_attribute(
                simple_cache, "cache_block_size",
                get_cache_param, (void *)Cache_Block_Size,
                set_cache_param, (void *)Cache_Block_Size,
                Sim_Attr_Required,
                "i", NULL,
                "cache_block_size");

        SIM_register_typed_attribute(
                simple_cache, "cache_set_number",
                get_cache_param, (void *)Cache_Set_Number,
                set_cache_param, (void *)Cache_Set_Number,
                Sim_Attr_Required,
                "i", NULL,
                "cache_set_number");

        SIM_register_typed_attribute(
                simple_cache, "cache_ways",
                get_cache_param, (void *)Cache_Ways,
                set_cache_param, (void *)Cache_Ways,
                Sim_Attr_Optional,
                "i", NULL,
                "cache_ways");

        SIM_register_typed_attribute(
                simple_cache, "read_penalty",
                get_cache_param, (void *)Read_Penalty,
                set_cache_param, (void *)Read_Penalty,
                Sim_Attr_Optional,
                "i", NULL,
                "Read penalty");

        SIM_register_typed_attribute(
                simple_cache, "write_penalty",
                get_cache_param, (void *)Write_Penalty,
                set_cache_param, (void *)Write_Penalty,
                Sim_Attr_Optional,
                "i", NULL,
                "Write penalty");

        SIM_register_typed_attribute(
                simple_cache, "read_miss_penalty",
                get_cache_param, (void *)Read_Miss_Penalty,
                set_cache_param, (void *)Read_Miss_Penalty,
                Sim_Attr_Optional,
                "i", NULL,
                "Read miss penalty");

        SIM_register_typed_attribute(
                simple_cache, "write_miss_penalty",
                get_cache_param, (void *)Write_Miss_Penalty,
                set_cache_param, (void *)Write_Miss_Penalty,
                Sim_Attr_Optional,
                "i", NULL,
                "Write miss penalty, also read for ownership penalty.");

        SIM_register_typed_attribute(
                simple_cache, "snoop_penalty",
                get_cache_param, (void *)Snoop_Penalty,
                set_cache_param, (void *)Snoop_Penalty,
                Sim_Attr_Optional,
                "i", NULL,
                "Extra penalty for issuing snooping transaction.");

        SIM_register_typed_attribute(
                simple_cache, "stat_data_write",
                get_stat, (void *)Stat_Data_Write,
                NULL, NULL,
                Sim_Attr_Pseudo,
                "i", NULL,
                "n. of writes");

        SIM_register_typed_attribute(
                simple_cache, "stat_data_write_miss",
                get_stat, (void *)Stat_Data_Write_Miss,
                NULL, NULL,
                Sim_Attr_Pseudo,
                "i", NULL,
                "n. of writes misses");

        SIM_register_typed_attribute(
                simple_cache, "stat_data_write_back",
                get_stat, (void *)Stat_Data_Write_Back,
                NULL, NULL,
                Sim_Attr_Pseudo,
                "i", NULL,
                "n. of writes");

        SIM_register_typed_attribute(
                simple_cache, "stat_data_write_back_miss",
                get_stat, (void *)Stat_Data_Write_Back_Miss,
                NULL, NULL,
                Sim_Attr_Pseudo,
                "i", NULL,
                "n. of writes misses");

        SIM_register_typed_attribute(
                simple_cache, "stat_data_read",
                get_stat, (void *)Stat_Data_Read,
                NULL, NULL,
                Sim_Attr_Pseudo,
                "i", NULL,
                "n. of reads");

        SIM_register_typed_attribute(
                simple_cache, "stat_data_read_miss",
                get_stat, (void *)Stat_Data_Read_Miss,
                NULL, NULL,
                Sim_Attr_Pseudo,
                "i", NULL,
                "n. of reads misses");

        SIM_register_typed_attribute(
                simple_cache, "stat_data_prefetch",
                get_stat, (void *)Stat_Data_Prefetch,
                NULL, NULL,
                Sim_Attr_Pseudo,
                "i", NULL,
                "n. of prefetches");

        SIM_register_typed_attribute(
                simple_cache, "stat_data_prefetch_miss",
                get_stat, (void *)Stat_Data_Prefetch_Miss,
                NULL, NULL,
                Sim_Attr_Pseudo,
                "i", NULL,
                "n. of prefetch misses");

        SIM_register_typed_attribute(
                simple_cache, "stat_data_prefetch_read_for_ownership",
                get_stat, (void *)Stat_Data_Prefetch_Read_For_Ownership,
                NULL, NULL,
                Sim_Attr_Pseudo,
                "i", NULL,
                "n. of read for ownership prefetches");

        SIM_register_typed_attribute(
                simple_cache, "stat_data_prefetch_read_for_ownership_miss",
                get_stat, (void *)Stat_Data_Prefetch_Read_For_Ownership_Miss,
                NULL, NULL,
                Sim_Attr_Pseudo,
                "i", NULL,
                "n. of read for ownership prefetch misses");

        SIM_register_typed_attribute(
                simple_cache, "stat_data_read_for_ownership",
                get_stat, (void *)Stat_Data_Read_For_Ownership,
                NULL, NULL,
                Sim_Attr_Pseudo,
                "i", NULL,
                "n. of read for ownership");

        SIM_register_typed_attribute(
                simple_cache, "stat_data_read_for_ownership_miss",
                get_stat, (void *)Stat_Data_Read_For_Ownership_Miss,
                NULL, NULL,
                Sim_Attr_Pseudo,
                "i", NULL,
                "n. of read for ownership misses");

        SIM_register_typed_attribute(
                simple_cache, "stat_instr_fetch_miss",
                get_stat, (void *)Stat_Instr_Fetch_Miss,
                NULL, NULL,
                Sim_Attr_Pseudo,
                "i", NULL,
                "n. of instr miss");

        SIM_register_typed_attribute(
                simple_cache, "stat_instr_fetch",
                get_stat, (void *)Stat_Instr_Fetch,
                NULL, NULL,
                Sim_Attr_Pseudo,
                "i", NULL,
                "n. of instr");

        SIM_register_typed_attribute(
                simple_cache, "evicted_modified",
                get_stat, (void *)Stat_Data_Evicted_Modified,
                NULL, NULL,
                Sim_Attr_Pseudo,
                "i", NULL,
                "n. of eviction of modified data");

        SIM_register_typed_attribute(
                simple_cache, "evicted_total",
                get_stat, (void *)Stat_Data_Evicted_Total,
                NULL, NULL,
                Sim_Attr_Pseudo,
                "i", NULL,
                "n. of total eviction.");

        SIM_register_typed_attribute(
                simple_cache, "stat_prefetches_used",
                get_stat, (void *)Stat_Data_Prefetches_Used,
                NULL, NULL,
                Sim_Attr_Pseudo,
                "i", NULL,
                "n. of prefetched lines used.");

        SIM_register_attribute(
                simple_cache, "next_level",
                get_next,
                set_next,
                Sim_Attr_Optional,
                "o|n",
                "next hierarchy object");

        SIM_register_attribute(
                simple_cache, "prev_level",
                get_prev,
                set_prev,
                Sim_Attr_Optional,
                "[o|n*]|n",
                "Previous caches in hierarchy");

        SIM_register_attribute(
                simple_cache, "level",
                get_level,
                set_level,
                Sim_Attr_Optional,
                "i",
                "Cache level 1 - N");

        SIM_register_attribute(
                simple_cache, "policy",
                get_policy,
                set_policy,
                Sim_Attr_Optional,
                "s",
                "Cache policy, either 'inclusive', 'exclusive',"
                " or 'nine' which is default");

        SIM_register_attribute(
                simple_cache, "ip_read_prefetcher",
                get_ip_read_prefetcher,
                set_ip_read_prefetcher,
                Sim_Attr_Optional,
                "b",
                "Use instruction pointer based read stride prefetcher.");

        SIM_register_attribute(
                simple_cache, "ip_write_prefetcher",
                get_ip_write_prefetcher,
                set_ip_write_prefetcher,
                Sim_Attr_Optional,
                "b",
                "Use instruction pointer based write stride prefetcher.");

        SIM_register_attribute(
                simple_cache, "prefetch_additional",
                get_prefetch_additional,
                set_prefetch_additional,
                Sim_Attr_Optional,
                "i",
                "Additional consecutive lines to prefetch");

        SIM_register_attribute(
                simple_cache, "prefetch_adjacent",
                get_prefetch_adjacent,
                set_prefetch_adjacent,
                Sim_Attr_Optional,
                "b",
                "Prefetch the adjacent cache line as well so the total fetch"
                " region is block size * 2, naturally aligned.");

        SIM_register_attribute(
                simple_cache, "cache_conn",
                get_conn,
                set_conn,
                Sim_Attr_Pseudo,
                "o",
                "The instrumentation connection");

        SIM_register_attribute(
                simple_cache, "write_allocate",
                get_write_allocate, set_write_allocate,
                Sim_Attr_Pseudo, "b",
                "If TRUE, write misses reads the cache line first,"
                " and then the line is modified, otherwise write through"
                " on write miss.");

        SIM_register_attribute(
                simple_cache, "write_back",
                get_write_back, set_write_back,
                Sim_Attr_Pseudo, "b",
                "If TRUE, modified cache line is written back to memory when"
                " line is flushed, otherwise write through.");

        SIM_register_typed_attribute(
                simple_cache, "meta_content",
                get_meta_content, NULL,
                NULL, NULL,
                Sim_Attr_Pseudo,
                "[[iiiii]*]", NULL,
                "Cache meta content for each line: "
                "[State, tag, accessed, in prefetch stride, prefetched line]."
                "State is encoded as: 0 in Invalid, 1 is Shared,"
                " 2 is Exclusive, and 3 is Modified.");

        SIM_register_attribute(
                simple_cache, "directory",
                get_directory,
                set_directory,
                Sim_Attr_Pseudo,
                "o|n",
                "Directory controlling this cache level");

        SIM_register_attribute(
                simple_cache, "cpu",
                get_cpu,
                set_cpu,
                Sim_Attr_Pseudo,
                "o|n|[o*]",
                "The cpus connected to the cache");

        register_directory_class();
}
