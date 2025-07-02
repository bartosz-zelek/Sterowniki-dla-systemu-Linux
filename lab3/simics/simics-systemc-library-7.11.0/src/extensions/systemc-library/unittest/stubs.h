// -*- mode: C++; c-file-style: "virtutech-c++" -*-

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

#ifndef SYSTEMC_LIBRARY_UNITTEST_STUBS_H
#define SYSTEMC_LIBRARY_UNITTEST_STUBS_H

#include <simics/base/event.h>
#include <simics/base/sim-exception.h>
#include <simics/base/time.h>

#include <string>
#include <exception>      // std::exception

class Stubs {
  public:
    Stubs();

    conf_object_t *last_log_obj_;
    int log_level_;
    int fatal_error_count_;
    sim_exception_t SIM_clear_exception_return_;
    int SIM_create_object_count_;
    int SIM_event_post_cycle_count_;
    cycles_t SIM_event_post_cycle_cycles_;
    int SIM_event_cancel_time_count_;
    int SIM_log_info_count_;
    int SIM_log_error_count_;
    int SIM_log_spec_violation_count_;
    int SIM_log_unimplemented_count_;
    int SIM_log_critical_count_;
    bool SIM_object_is_configured_;
    bool SIM_is_restoring_state_;
    int check_async_events_count_;
    int async_events_pending_count_;
    int SIM_break_simulation_count_;
    int VT_effective_log_level_;
    conf_object_t sim_obj_;
    conf_object_t *created_obj_;
    conf_class_t *SIM_create_object_cls_;
    attr_value_t SIM_create_object_attrs_;
    std::string SIM_get_class_name_;
    conf_object_t *SIM_object_descendant_;
    std::string SIM_object_descendant_relname_;
    conf_object_t *SIM_port_object_parent_;
    interface_t *SIM_c_get_port_interface_;
    std::string SIM_object_name_;

    std::string SIM_log_error_;
    std::string SIM_log_info_;
    static Stubs instance_;
};

struct conf_class {
};

extern "C" {
pc_step_t SIM_continue(int64 steps);
}

#endif  // SYSTEMC_LIBRARY_UNITTEST_STUBS_H
