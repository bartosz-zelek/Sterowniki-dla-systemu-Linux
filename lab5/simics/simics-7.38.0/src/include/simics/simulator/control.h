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

#ifndef SIMICS_SIMULATOR_CONTROL_H
#define SIMICS_SIMULATOR_CONTROL_H

#include <simics/base/types.h>
#include <simics/base/time.h>
#include <simics/processor/time.h>
#include <simics/pywrap.h>

#include <simics/simulator/oec-control.h>

#if defined(__cplusplus)
extern "C" {
#endif

EXPORTED pc_step_t SIM_continue(int64 steps);

EXPORTED bool SIM_simics_is_running(void);

EXPORTED void SIM_break_simulation(const char *msg);

EXPORTED void SIM_break_message(const char *msg);

EXPORTED const char *VT_get_break_message(void);

EXPORTED void SIM_shutdown(void);

EXPORTED NORETURN void SIM_quit(int exit_code);

EXPORTED void VT_wait_for_simulator_init(void);

EXPORTED void SIM_break_cycle(conf_object_t *NOTNULL obj, int64 cycles);
EXPORTED void SIM_break_step(conf_object_t *NOTNULL obj, int64 steps);

EXPORTED void VT_user_interrupt(const char *msg, int break_script);

EXPORTED bool VT_inside_instruction(void);

EXPORTED void VT_thread_cell_association_begin(conf_object_t *NOTNULL obj);
EXPORTED void VT_thread_cell_association_end(conf_object_t *NOTNULL obj);

EXPORTED bool VT_in_main_branch(void);

#ifndef PYWRAP
EXPORTED int VT_process_work(int *done);
#endif

/* Internal */
SIM_INTERFACE(tcf_channel) {
        attr_value_t (*start_channel)(conf_object_t *NOTNULL obj,
                                      const char *NOTNULL url);
};

#define TCF_CHANNEL_INTERFACE "tcf_channel"

EXPORTED attr_value_t VT_get_stop_reasons(void);
typedef enum { Sim_Stop_Stopped, Sim_Stop_Aborted } stop_type_t;
EXPORTED stop_type_t VT_get_stop_type(void);

EXPORTED void VT_stop_message(conf_object_t *obj, const char *msg);
EXPORTED void VT_stop_error(conf_object_t *obj, const char *NOTNULL msg);

EXPORTED void VT_stop_user(const char *msg);
EXPORTED void VT_stop_finished(const char *msg);

EXPORTED void VT_abort_error(conf_object_t *obj, const char *NOTNULL msg);
EXPORTED void VT_abort_user(const char *msg);

typedef struct execute_environ execute_environ_t;
EXPORTED execute_environ_t *VT_create_execute_environ(void);
EXPORTED void VT_free_execute_environ(execute_environ_t *env);
EXPORTED void VT_set_execute_environ(conf_object_t *NOTNULL obj,
                                     execute_environ_t *env);

EXPORTED void VT_stop_execution(execute_environ_t *env);
EXPORTED void VT_activate_executor(execute_environ_t *NOTNULL ec,
                                   conf_object_t *obj);
EXPORTED bool VT_execute(execute_environ_t *NOTNULL ec);


#if defined(__cplusplus)
}
#endif

#endif
