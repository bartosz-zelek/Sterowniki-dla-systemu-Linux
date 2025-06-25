/*
  gc-common.h - common macro and type definitions for generic-cache (all
  versions)

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef GC_COMMON_H
#define GC_COMMON_H

#include <simics/arch/x86.h>
#include <simics/arch/ppc.h>
#include <simics/arch/mips.h>

#include "scalar-time.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* MESI states */
typedef enum cache_line_status {
        CL_Invalid   = 0,
        CL_Shared    = 1,
        CL_Exclusive = 2,
        CL_Modified  = 3
} cache_line_status_t;

/* 
   Cache transaction used by caches to propagate copy-back/fetches to the next
   level.
   - s is the generic transaction created by the cache. Note that this is a
     virtual transaction, it doesn't contain any data.
   - initial_trans is a pointer to the initial cpu transaction.
*/
typedef struct cache_memory_transaction {
        generic_transaction_t s;
        generic_transaction_t *initial_trans;
} cache_memory_transaction_t;

/* STC where the data in the cache line could be stored */
typedef enum stc_type {
        STC_DataSTC = 1, /* this line contains data */
        STC_InstSTC = 2  /* this line contains inst. fetch */
} stc_type_t;

/* Lists of profilers implemented in the cache */
enum {
        I_Data_Read_Miss,       /* data read miss indexed per instruction */
        V_Data_Read_Miss,       /* data read miss indexed per virt. address */
        P_Data_Read_Miss,       /* data read miss indexed per phys. address */
        
        I_Data_Write_Miss,      /* ... */
        V_Data_Write_Miss,
        P_Data_Write_Miss,

        V_Ins_Fetch_Miss,
        P_Ins_Fetch_Miss,

        GC_Max_Profilers,        /* total number of possible profilers */

        I_DRM = I_Data_Read_Miss, /* shorter names */
        V_DRM = V_Data_Read_Miss,
        P_DRM = P_Data_Read_Miss,

        I_DWM = I_Data_Write_Miss,
        V_DWM = V_Data_Write_Miss,
        P_DWM = P_Data_Write_Miss,

        V_IFM = V_Ins_Fetch_Miss,
        P_IFM = P_Ins_Fetch_Miss,
};

/* debug groups */
enum {
        GC_Log_Read_Hit   = 0x01, /* Read hit transactions */
        GC_Log_Read_Miss  = 0x02, /* Read miss transactions */
        GC_Log_Write_Hit  = 0x04, /* Write hit transactions */
        GC_Log_Write_Miss = 0x08, /* Write miss transactions */
        GC_Log_Cache      = 0x10, /* Other cache activities */
        GC_Log_Repl       = 0x20, /* Replacement policies */
        GC_Log_MESI       = 0x40, /* MESI protocol */
        GC_Log_Attr       = 0x80, /* Cache attributes */
        GC_Log_OT         = 0x100 /* Outstanding Trans. (ooo only) */
};

extern const char *const gc_log_groups[];

typedef struct generic_cache generic_cache_t;

/* MESI listener interface */
typedef struct mesi_listen_interface {
        cycles_t (*snoop_transaction)(conf_object_t *obj,
                                      generic_transaction_t *mem_op);
} mesi_listen_interface_t;
#define MESI_LISTEN_INTERFACE "mesi_listen_interface"


/* Replacement policy interface */
typedef struct repl_interface {
        /* called when allocating a new cache instance */
        void       *(*new_instance) (generic_cache_t *gc);
        /* called when the cache configuration is updated */
        void        (*update_config)(void *data, generic_cache_t *gc);
        /* return the name of the algorithm */
        const char *(*get_name)     ();

        /* update the algorithm state when a transaction if finished */
        void        (*update_repl)  (void *data, generic_cache_t *gc, 
                                     generic_transaction_t *gt, int line_num);
        /* return a line to use for a new transaction */
        int         (*get_line)     (void *data, generic_cache_t *gc, 
                                     generic_transaction_t *gt);

        /* optional functions */
        int64   (*get_timestamp)(void *data, generic_cache_t *gc, int line);
        void        (*set_timestamp)(void *data, generic_cache_t *gc, int line,
                                     int64 timestamp);
} repl_interface_t;


/*
  common variables and functions
*/
uint32 ilog2(uint32 new_value);

void add_repl_policy(const repl_interface_t *ri);
void gc_common_register(conf_class_t *gc_class);
void gc_init_cache(generic_cache_t *gc);
conf_object_t *gc_alloc_object(void *data);
void *gc_init_object(conf_object_t *obj, void *data);
void gc_finalize_instance(conf_object_t *cache);
void gc_set_config_line_number(generic_cache_t *gc, int line_number);
int gc_set_config_repl(generic_cache_t *gc, const char *repl);

const repl_interface_t *init_rand_repl(conf_class_t *gc_class);
const repl_interface_t *init_lru_repl(conf_class_t *gc_class);
const repl_interface_t *init_lru2_repl(conf_class_t *gc_class);
const repl_interface_t *init_cyclic_repl(conf_class_t *gc_class);

void sort_init_local();

int is_uncacheable(generic_cache_t *gc, generic_transaction_t *mem_op, 
                   map_list_t *map, conf_object_t *o);


/* 
   common macros
*/

/* increase a stat counter */
#define INC_STAT(gc, label) do {                                             \
        (gc)->stat.label++;                                                  \
        st_ssp_increment_counter(&(gc)->scalar_time_port[ST_ ## label], 1);  \
} while (0)

/* compute the virtual/physical address of a cache line */
#define GC_LINE_VADDR(gc, line) \
((gc->config.virtual_tag ? line->tag : line->otag) << gc->config.line_size_log2)
#define GC_LINE_PADDR(gc, line) \
((gc->config.virtual_tag ? line->otag : line->tag) << gc->config.line_size_log2)

/* compute the base address from the tag. */
#define GC_UNTAG(gc, tag) ((tag) << (gc)->config.line_size_log2)

/* how many sets in the cache. */
#define GC_NEXT_ASSOC(gc) ((gc)->config.next_assoc)

#define GC_GET_LOGICAL_ADDRESS(mem_op)                                       \
        (SIM_get_mem_op_ini_type(mem_op) == Sim_Initiator_CPU_X86            \
         ? ((x86_memory_transaction_t *)(mem_op))->linear_address            \
         : SIM_get_mem_op_virtual_address(mem_op))

/* the address used to compute the tag */
#define GC_TAG_ADDRESS(gc, mem_op)                                           \
        (((gc)->config.virtual_tag)                                          \
         ? GC_GET_LOGICAL_ADDRESS(mem_op)                                    \
         : SIM_get_mem_op_physical_address(mem_op))

/* opposite of GC_TAG_ADDRESS */
#define GC_OTAG_ADDRESS(gc, mem_op)                                          \
        (((gc)->config.virtual_tag)                                          \
         ? SIM_get_mem_op_physical_address(mem_op)                           \
         : GC_GET_LOGICAL_ADDRESS(mem_op))

/* the address used to compute the index */
#define GC_INDEX_ADDRESS(gc, mem_op)                                         \
        (((gc)->config.virtual_index)                                        \
         ? GC_GET_LOGICAL_ADDRESS(mem_op)                                    \
         : SIM_get_mem_op_physical_address(mem_op))

/* compute the tag/otag for a given memory transaction */
#define GC_TAG(gc, mem_op)                                                   \
        ((GC_TAG_ADDRESS(gc, mem_op)) >> (gc)->config.line_size_log2)

#define GC_OTAG(gc, mem_op)                                                  \
        ((GC_OTAG_ADDRESS(gc, mem_op)) >> (gc)->config.line_size_log2)

/* Compute a mask to hide the address bits that do not belong to the
   tag. */
#define GC_TAG_MASK(gc) ((gc)->config.tag_mask)

/* Computes the index for a given memory transaction */
#define GC_INDEX_MASK(gc) ((gc)->config.index_mask)
#define GC_INDEX(gc, mem_op)                                                 \
        (((GC_INDEX_ADDRESS(gc, mem_op)) >> (gc)->config.line_size_log2) &    \
          GC_INDEX_MASK(gc))

/* offset of a memory transaction in the line */
#define GC_LINE_OFFSET(gc, mem_op)                                           \
        ((GC_TAG_ADDRESS(gc, mem_op)) & ((gc)->config.line_size - 1))

/* address for a memory transaction */
#define GC_PA_ADDRESS(mem_op) (SIM_get_mem_op_physical_address(mem_op))
#define GC_VA_ADDRESS(mem_op) GC_GET_LOGICAL_ADDRESS(mem_op)

#define GC_CPU(mem_op)                                                       \
        (((SIM_get_mem_op_ini_type(mem_op) & 0xFF00) == Sim_Initiator_Cache) \
         ? (SIM_get_mem_op_initiator(                                        \
                    ((cache_memory_transaction_t *) mem_op)->initial_trans)) \
         : (SIM_get_mem_op_initiator(mem_op)))

/* generic set function for integer attribute */
#define GC_INT_ATTR_SET(field, attr)                                         \
static set_error_t                                                           \
set_##field##_##attr(void *dont_care, conf_object_t *obj,                    \
                     attr_value_t *val, attr_value_t *idx)                   \
{                                                                            \
        generic_cache_t *gc = (generic_cache_t *) obj;                       \
        gc->field.attr = SIM_attr_integer(*val);                             \
        return Sim_Set_Ok;                                                   \
}

/* set function for integer attribute that updates the
   precomputed values */
#define GC_INT_ATTR_UPDATE_SET(field, attr)                                  \
static set_error_t                                                           \
set_##field##_##attr(void *dont_care, conf_object_t *obj,                    \
                     attr_value_t *val, attr_value_t *idx)                   \
{                                                                            \
        generic_cache_t *gc = (generic_cache_t *) obj;                       \
        gc->field.attr = SIM_attr_integer(*val);                             \
        update_precomputed_values(gc);                                       \
        return Sim_Set_Ok;                                                   \
}

/* generic get function for integer attribute */
#define GC_INT_ATTR_GET(field, attr)                                         \
static attr_value_t                                                          \
get_##field##_##attr(void *dont_care, conf_object_t *obj,                    \
                     attr_value_t *idx)                                      \
{                                                                            \
        generic_cache_t *gc = (generic_cache_t *) obj;                       \
        return SIM_make_attr_uint64(gc->field.attr);                        \
}

#if defined(__cplusplus)
}
#endif

#endif  /* ! GC_COMMON_H */
