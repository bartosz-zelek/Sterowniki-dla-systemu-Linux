/*
  sample-risc-exec.c - sample code for executing instructions

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "sample-risc-exec.h"
#include "event-queue.h"
#ifndef SAMPLE_RISC_HEADER
#define SAMPLE_RISC_HEADER "sample-risc.h"
#endif
#include SAMPLE_RISC_HEADER
#include "sample-risc-queue.h"
#include <simics/processor-api.h>

static void
check_event_queues(sample_risc_t *sr)
{
        handle_events(sr, &sr->cycle_queue);
        if (sr->current_core->is_enabled) {
                handle_events(sr, &sr->step_queue);
        }
}

static void
exec_run(conf_object_t *obj)
{
        sample_risc_t *sr = conf_obj_to_sr(obj);

        sr->cell_iface->set_current_processor_obj(
                sr->cell, sr_core_to_conf_obj(sr->current_core));
        sr->cell_iface->set_current_step_obj(sr->cell, obj);

        SIM_LOG_INFO(2, sr_to_conf_obj(sr), 0, "running");
        sr->state = State_Running;

        /* handle all events on the current step and cycle */
        check_event_queues(sr);

        while (sr->state == State_Running) {
                if (sr->current_core->is_enabled
                    && sr->stall_cycles == 0) {
                        if (!sample_core_fetch_and_execute_instruction(
                                    sr->current_core))
                                return;
                } else {
                        simtime_t delta = get_delta(&sr->cycle_queue);

                        if (sr->stall_cycles != 0) {
                                /* Cap time advance at current stall time,
                                   since then we want to get back to issuing
                                   instructions. */
                                if (delta > sr->stall_cycles)
                                        delta = sr->stall_cycles;

                                /* Stall as much as we can up to delta. */
                                sr->total_stall_cycles += delta;
                                sr->stall_cycles -= delta;
                        }

                        if (delta > 0) {
                                sample_core_increment_cycles(sr->current_core,
                                                             delta);
                        }
                }
                if (sr->state == State_Stopped) // breakpoint triggered
                        break;

                /* handle all events on the current step and cycle */
                check_event_queues(sr);
        }
}

static void
exec_stop(conf_object_t *obj)
{
        sample_risc_t *sr = conf_obj_to_sr(obj);
        SIM_LOG_INFO(2, sr_to_conf_obj(sr), 0, "stop");
        sr->state = State_Stopped;
        VT_stop_event_processing(obj);
}

void
register_execute_interface(conf_class_t *cls)
{
        static const execute_interface_t execute_iface = {
                .run = exec_run,
                .stop = exec_stop
        };
        SIM_register_interface(cls, EXECUTE_INTERFACE, &execute_iface);
}
