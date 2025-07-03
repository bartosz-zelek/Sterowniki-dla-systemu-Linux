/*
  sample-risc-memory.c - sample code for memory access and page buffers

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "sample-risc-memory.h"
#include <simics/processor-api.h>
#include <simics/util/vect.h>

#include <simics/model-iface/image.h>
#include <simics/model-iface/simulator-cache.h>
#include <simics/model-iface/step.h>

#include "sample-risc-local.h"

#ifndef SAMPLE_RISC_HEADER
#define SAMPLE_RISC_HEADER "sample-risc.h"
#endif
#include SAMPLE_RISC_HEADER

static attr_value_t
format_page_for_output(sample_page_t *p)
{
        ASSERT(p->mem_space);
        return SIM_make_attr_list(
                4,
                SIM_make_attr_object(p->mem_space),
                SIM_make_attr_uint64(p->paddr),
                SIM_make_attr_uint64(p->size),
                SIM_make_attr_uint64(p->access));
}

/* The actual page cache */
void
init_page_cache(sample_risc_t *sr)
{
        VINIT(sr->cached_pages);
}

void
clear_page_cache(sample_risc_t *sr)
{
        VFREE(sr->cached_pages);
}

sample_page_t *
search_page_cache(sample_risc_t *sr, conf_object_t *phys_mem_obj,
                  physical_address_t address)
{
        VFORI(sr->cached_pages, i) {
                sample_page_t *ret = &VGET(sr->cached_pages, i);
                if (ret->mem_space == phys_mem_obj) {
                        physical_address_t mask = ~(ret->size - 1);
                        if (ret->paddr == (address & mask))
                                return ret;
                }
        }
        return NULL;
}

sample_page_t *
add_to_page_cache(sample_risc_t *sr, sample_page_t p)
{
        ASSERT(p.mem_space);
        sample_page_t *entry = search_page_cache(sr, p.mem_space, p.paddr);
        if (entry) {
                *entry = p;
                return entry;
        } else {
                VADD(sr->cached_pages, p);
                return &VLAST(sr->cached_pages);
        }
}

static attr_value_t
get_page_cache(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        sample_risc_t *sr = conf_obj_to_sr(obj);
        attr_value_t ret = SIM_alloc_attr_list(VLEN(sr->cached_pages));
        VFORI(sr->cached_pages, p_idx) {
                sample_page_t *p = &VGET(sr->cached_pages, p_idx);
                SIM_attr_list_set_item(&ret, p_idx, format_page_for_output(p));
        }
        return ret;
}

void
check_virtual_breakpoints(sample_risc_t *sr, sample_risc_core_t *core,
                          access_t access, logical_address_t virt_start,
                          generic_address_t len, uint8 *data)
{
        logical_address_t virt_end = virt_start + len - 1;
        breakpoint_set_t bp_set =
                 core->context_bp_query_iface->get_breakpoints(
                         core->current_context, access, virt_start, virt_end);
        for (int i = 0; i < bp_set.num_breakpoints; i++) {
                core->context_bp_trig_iface->trigger_breakpoint(
                        core->current_context,
                        sr_core_to_conf_obj(core),
                        bp_set.breakpoints[i].handle,
                        virt_start,
                        len,
                        access,
                        data);
        }
        MM_FREE(bp_set.breakpoints);
}

static generic_transaction_t
create_generic_transaction(conf_object_t *initiator, mem_op_type_t type,
                           physical_address_t phys_address,
                           physical_address_t len, uint8 *data,
                           endianness_t endian, bool may_stall)
{
        generic_transaction_t ret;
        memset(&ret, 0, sizeof(ret));
        SIM_set_mem_op_virtual_address(&ret, 0);
        SIM_set_mem_op_physical_address(&ret, phys_address);
        ret.real_address = data;
        ret.size = len;
        SIM_set_mem_op_initiator(&ret,
                                 Sim_Initiator_CPU,
                                 initiator);
        ret.use_page_cache = 1;
        ret.may_stall = may_stall;
        SIM_set_mem_op_inquiry(&ret, 0);
        SIM_set_mem_op_type(&ret, type);

        if (endian == Sim_Endian_Host_From_BE)
                ret.inverse_endian = 1;

        return ret;
}

/* The program interface allows programs to read, write and fetch
   instructions from memory.  Check if the desired page is in the
   page cache.  If so, do the access from the page cache.  If
   not, call the memory interface function to get the desired
   bytes from the memory.  This is probably a memory mapped I/O
   access if it isn't cachable.
*/
bool
write_memory(sample_risc_t *sr, sample_risc_core_t *core,
             physical_address_t phys_address, physical_address_t len,
             uint8 *data, bool check_bp)
{
        conf_object_t *phys_mem_obj = core->phys_mem_obj;

        sample_page_t *page = get_page(sr, core, phys_mem_obj,
                                       phys_address, Sim_Access_Write);

        if (page) {
                uint8 *ma = page->data
                        + ((page->size - 1) & phys_address);
                memcpy(ma, data, len);
                return true;
        }

        // Check for pending asynchronous events in slow memory path
        VT_check_async_events();

        generic_transaction_t mem_op = create_generic_transaction(
                sr_core_to_conf_obj(core),
                Sim_Trans_Store,
                phys_address, len,
                data,
                Sim_Endian_Target,
                core->issue_stallable_memops);
        exception_type_t exc =
                core->phys_mem_space_iface->access(phys_mem_obj, &mem_op);

        if (exc != Sim_PE_No_Exception) {
                if (exc != Sim_PE_Stall_Cpu)
                        SIM_LOG_ERROR(
                                sr_to_conf_obj(sr), 0,
                                "write error to physical address 0x%llx",
                                phys_address);
                return false;
        }

        return true;
}

bool
read_memory(sample_risc_t *sr, sample_risc_core_t *core,
            physical_address_t phys_address, physical_address_t len,
            uint8 *data, bool check_bp)
{
        conf_object_t *phys_mem_obj = core->phys_mem_obj;

        sample_page_t *page = get_page(sr, core, phys_mem_obj,
                                       phys_address, Sim_Access_Read);

        if (page) {
                uint8 *ma = page->data
                        + ((page->size - 1) & phys_address);
                memcpy(data, ma, len);
                return true;
        }

        // Check for pending asynchronous events in slow memory path
        VT_check_async_events();

        generic_transaction_t mem_op = create_generic_transaction(
                sr_core_to_conf_obj(core),
                Sim_Trans_Load,
                phys_address, len,
                data,
                Sim_Endian_Target,
                core->issue_stallable_memops);
        exception_type_t exc =
                core->phys_mem_space_iface->access(phys_mem_obj, &mem_op);

        if (exc != Sim_PE_No_Exception) {
                if (exc != Sim_PE_Stall_Cpu)
                        SIM_LOG_ERROR(
                                sr_to_conf_obj(sr), 0,
                                "read error from physical address 0x%llx",
                                phys_address);
                return false;
        }

        return true;
}

bool
fetch_instruction(sample_risc_t *sr, sample_risc_core_t *core,
                  physical_address_t phys_address, physical_address_t len,
                  uint8 *data, bool check_bp)
{
        conf_object_t *phys_mem_obj = core->phys_mem_obj;
        sample_page_t *page = get_page(sr, core, phys_mem_obj, phys_address,
                                       Sim_Access_Execute);

        if (page) {
                uint8 *ma = page->data
                        + (phys_address & (page->size - 1));
                memcpy(data, ma, len);
                return true;
        }

        // Check for pending asynchronous events in slow memory path
        VT_check_async_events();

        generic_transaction_t mem_op = create_generic_transaction(
                sr_core_to_conf_obj(core),
                Sim_Trans_Instr_Fetch,
                phys_address, len,
                data,
                Sim_Endian_Target,
                false);
        exception_type_t exc =
                core->phys_mem_space_iface->access(phys_mem_obj, &mem_op);

        if (exc != Sim_PE_No_Exception) {
                SIM_LOG_ERROR(sr_to_conf_obj(sr), 0,
                              "fetch error from physical address 0x%llx",
                              phys_address);
                return false;
        }

        return true;
}

/*
 * simulator cache interface functions
 */
static void
simulator_cache_flush(conf_object_t *obj)
{
        sample_risc_t *sr = conf_obj_to_sr(obj);
        SIM_LOG_INFO(2, sr_to_conf_obj(sr), 0, "simulator_cache_flush");
        clear_page_cache(sr);
}

static void
breakpoint_added(conf_object_t *obj,
                 conf_object_t *bp_obj,
                 breakpoint_handle_t handle)
{
        /* A breakpoint has been added. This may cause some breakpoint
           information in the page cache to become stale. We would request more
           information about the added breakpoint here and do something clever,
           but we instead simply clear the entire page cache. */
        sample_risc_t *sr = conf_obj_to_sr(obj);
        SIM_LOG_INFO(2, sr_to_conf_obj(sr), 0, "breakpoint_added");
        clear_page_cache(sr);
}

static void
breakpoint_removed(conf_object_t *obj,
                   conf_object_t *bp_obj,
                   breakpoint_handle_t handle)
{
        /* A breakpoint has been removed. This may cause some breakpoint
           information in the page cache to become stale. We would request more
           information about the removed breakpoint here and do something 
           clever, but we instead simply clear the entire page cache. */
        sample_risc_t *sr = conf_obj_to_sr(obj);
        SIM_LOG_INFO(2, sr_to_conf_obj(sr), 0, "breakpoint_removed");
        clear_page_cache(sr);
}

static cycles_t
get_stall_cycles(conf_object_t *obj)
{
        sample_risc_t *sr = conf_obj_to_sr(obj);
        return sr->stall_cycles;
}

static void
set_stall_cycles(conf_object_t *obj, cycles_t cycles)
{
        sample_risc_t *sr = conf_obj_to_sr(obj);
        sr->stall_cycles = cycles;
}

static cycles_t
get_total_stall_cycles(conf_object_t *obj)
{
        sample_risc_t *sr = conf_obj_to_sr(obj);
        return sr->total_stall_cycles;
}

/* export the functions to set up the memory interfaces and attributes */
void
register_memory_interfaces(conf_class_t *cls)
{
        static const simulator_cache_interface_t sci_iface = {
                .flush = simulator_cache_flush
        };
        SIM_register_interface(cls, SIMULATOR_CACHE_INTERFACE, &sci_iface);

        static const breakpoint_change_interface_t bp_change_iface = {
                .breakpoint_added = breakpoint_added,
                .breakpoint_removed = breakpoint_removed
        };
        SIM_register_interface(cls, BREAKPOINT_CHANGE_INTERFACE,
                               &bp_change_iface);

        register_local_memory_interfaces(cls);

        static const stall_interface_t stall_iface = {
                .get_stall_cycles = get_stall_cycles,
                .set_stall_cycles = set_stall_cycles,
                .get_total_stall_cycles = get_total_stall_cycles
        };
        SIM_REGISTER_INTERFACE(cls, stall, &stall_iface);
}

void
register_memory_attributes(conf_class_t *cls)
{
        SIM_register_typed_attribute(
                cls, "cached_pages",
                get_page_cache, NULL,
                0, NULL,
                Sim_Attr_Pseudo,
                "[[oiii]*]", NULL,
                "Cached pages; for debugging/testing purposes only");
}
