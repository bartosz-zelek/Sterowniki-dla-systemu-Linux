/*
  sample-risc.h - sample code for a stand-alone RISC processor

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SAMPLE_RISC_H
#define SAMPLE_RISC_H

#include <simics/base/types.h>
#include <simics/base/log.h>
#include <simics/base/bigtime.h>
#include <simics/processor/types.h>
#include <simics/model-iface/execute.h>
#include <simics/devs/frequency.h>

#include "sample-risc-local.h"

typedef struct sample_risc_core sample_risc_core_t;

#include "event-queue-types.h"
#include "sample-risc-core.h"
#include "sample-risc-exec.h"

typedef struct {
        conf_object_t *object;
        const frequency_listener_interface_t *iface;
        char *port_name;
} frequency_target_t;

typedef VECT(frequency_target_t) frequency_target_list_t;

typedef struct {
        conf_object_t *mem_space;
        physical_address_t paddr;
        physical_address_t size;
        access_t access;
        uint8 *data;
} sample_page_t;

/*
 * cosimulator class
 */
typedef struct cosimulator {
        /* pointer to the corresponding Simics object */
        conf_object_t *obj;

        /* Pointer to cell */
        conf_object_t *cell;
        const cell_inspection_interface_t *cell_iface;

        /* currently selected core */
        sample_risc_core_t *current_core;
        conf_object_t *current_core_obj;

        /* page cache */
        VECT(sample_page_t) cached_pages;

        /* execute state */
        execute_state_t state;

        /* cycle queue */
        cycles_t current_cycle;
        event_queue_t cycle_queue;

        /* step queue */
        pc_step_t current_step;
        event_queue_t step_queue;

        cycles_t stall_cycles;
        cycles_t total_stall_cycles;

        /* the offset that has to be added to the frequency-rescaled
           cycle counter to produce the absolute time */
        bigtime_t time_offset;

        /* frequency related data */
        uint64 freq_hz;
        conf_object_t *frequency_dispatcher;
        char *frequency_dispatcher_port;
        const simple_dispatcher_interface_t *frequency_dispatcher_iface;
        frequency_target_list_t frequency_targets;
} sample_risc_t;

static inline conf_object_t *
sr_to_conf_obj(sample_risc_t *sr)
{
        return sr->obj;
}

static inline sample_risc_t *
conf_obj_to_sr(conf_object_t *obj)
{
        return (sample_risc_t *) SIM_object_data(obj);
}

void sample_risc_cycle_event_posted(sample_risc_t *sr);
void sample_risc_step_event_posted(sample_risc_t *sr);

#endif
