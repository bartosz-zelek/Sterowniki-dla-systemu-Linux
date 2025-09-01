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

#include "stubs.h"

#include <simics/base/types.h>
#include <simics/base/sim-exception.h>
#include <simics/base/time.h>
#include <simics/base/map-target.h>
#include <simics/base/log.h>

#include <simics/cc-api.h>

// For psaddr_t
#include <sys/procfs.h>
#include <dlfcn.h>

#include <exception>      // std::exception
#include <string>
#include <utility>
#include <vector>

std::vector<std::pair <void (*)(lang_void *), lang_void*> >
    thread_safe_callbacks_;

Stubs::Stubs() : last_log_obj_(NULL),
                 log_level_(4),
                 fatal_error_count_(0),
                 SIM_clear_exception_return_(SimExc_No_Exception),
                 SIM_create_object_count_(0),
                 SIM_event_post_cycle_count_(0),
                 SIM_event_post_cycle_cycles_(-1),
                 SIM_event_cancel_time_count_(0),
                 SIM_log_info_count_(0),
                 SIM_log_error_count_(0),
                 SIM_log_spec_violation_count_(0),
                 SIM_log_unimplemented_count_(0),
                 SIM_log_critical_count_(0),
                 SIM_object_is_configured_(true),
                 SIM_is_restoring_state_(false),
                 check_async_events_count_(0),
                 async_events_pending_count_(0),
                 SIM_break_simulation_count_(0),
                 created_obj_(NULL),
                 SIM_create_object_cls_(NULL),
                 SIM_object_descendant_(NULL),
                 SIM_port_object_parent_(NULL),
                 SIM_c_get_port_interface_(NULL) {
    instance_ = *this;
    thread_safe_callbacks_.clear();
}

void fatal_error(const char *fmt, ...) {
    ++Stubs::instance_.fatal_error_count_;
    throw std::exception();
}

sim_exception_t SIM_clear_exception(void) {
    sim_exception_t ret = Stubs::instance_.SIM_clear_exception_return_;
    Stubs::instance_.SIM_clear_exception_return_ = SimExc_No_Exception;
    return ret;
}

const char *SIM_last_error(void) {
    return "stub error";
}

extern "C" {
conf_object_t *SIM_create_object(conf_class_t *NOTNULL cls, const char *name,
                                 attr_value_t attrs) {
    ++Stubs::instance_.SIM_create_object_count_;
    Stubs::instance_.SIM_create_object_cls_ = cls;
    Stubs::instance_.SIM_create_object_attrs_ = attrs;
    return Stubs::instance_.created_obj_;
}

conf_class_t *SIM_get_class(const char *NOTNULL name) {
    Stubs::instance_.SIM_get_class_name_ = name;
    return 0;
}

void SIM_break_simulation(const char *msg) {
    ++Stubs::instance_.SIM_break_simulation_count_;
}

void SIM_notify(conf_object_t *NOTNULL obj, notifier_type_t type) {
}

bool VT_check_async_events(void) {
    ++Stubs::instance_.check_async_events_count_;
    return true;
}

bool VT_async_events_pending(void) {
    ++Stubs::instance_.async_events_pending_count_;
    return false;
}

typedef enum {
    PS_OK,
    PS_ERR
} ps_err_e;

ps_err_e ps_pglobal_lookup(void *user_context, const char *object_name,
                           const char *sym_name, psaddr_t *sym_addr) {
    *sym_addr = dlsym(RTLD_DEFAULT, sym_name);
    if (*sym_addr)
        return PS_OK;

    return PS_ERR;
}

ps_err_e ps_pdread(void *user_context, psaddr_t addr, void *buf, size_t size) {
    memcpy(buf, addr, size);
    return PS_OK;
}

void SIM_thread_safe_callback(void (*NOTNULL f)(lang_void *),
                              lang_void *data) {
    thread_safe_callbacks_.push_back(std::make_pair(f, data));
}

pc_step_t SIM_continue(int64 steps) {
    std::vector<std::pair <void (*)(lang_void *),
                lang_void*> >::iterator i;
    for (i = thread_safe_callbacks_.begin(); i != thread_safe_callbacks_.end();
         ++i) {
        i->first(i->second);
    }
    thread_safe_callbacks_.clear();
    return steps;
}

set_error_t SIM_set_attribute_default(conf_object_t *NOTNULL obj,
                                      const char *NOTNULL name,
                                      attr_value_t value) {
    return Sim_Set_Ok;
}

conf_object_t *SIM_object_descendant(conf_object_t *obj,
                                     const char *NOTNULL relname) {
    Stubs::instance_.SIM_object_descendant_relname_ = relname;
    return Stubs::instance_.SIM_object_descendant_;
}

conf_object_t *SIM_port_object_parent(conf_object_t *obj) {
    return Stubs::instance_.SIM_port_object_parent_;
}

attr_value_t SIM_get_attribute(conf_object_t *NOTNULL obj,
                               const char *NOTNULL name) {
    return SIM_make_attr_nil();
}

set_error_t SIM_set_attribute(conf_object_t *NOTNULL obj,
                              const char *NOTNULL name,
                              attr_value_t *NOTNULL value) {
    return Sim_Set_Ok;
}

void SIM_require_object(conf_object_t *NOTNULL obj) {
}

bool SIM_simics_is_running(void) {
    return true;
}
}

void SIM_event_cancel_time(conf_object_t *NOTNULL clock,
                           event_class_t *NOTNULL evclass,
                           conf_object_t *NOTNULL obj,
                           int (*pred)(lang_void *data, lang_void *match_data),
                           lang_void *match_data) {
    ++Stubs::instance_.SIM_event_cancel_time_count_;
}

void SIM_event_post_cycle(conf_object_t *NOTNULL clock,
                          event_class_t *NOTNULL evclass,
                          conf_object_t *NOTNULL obj,
                          cycles_t cycles,
                          lang_void *user_data) {
    ++Stubs::instance_.SIM_event_post_cycle_count_;
    Stubs::instance_.SIM_event_post_cycle_cycles_ = cycles;
}

event_class_t *
SIM_register_event(
               const char *NOTNULL name,
               conf_class_t *cl,
               event_class_flag_t flags,
               void (*NOTNULL callback)(conf_object_t *obj, lang_void *data),
               void (*destroy)(conf_object_t *obj, lang_void *data),
               attr_value_t (*get_value)(conf_object_t *obj, lang_void *data),
               lang_void *(*set_value)(conf_object_t *obj, attr_value_t value),
               char *(*describe)(conf_object_t *obj, lang_void *data)) {
    return 0;
}

double
SIM_event_find_next_time(conf_object_t *NOTNULL clock,
                         event_class_t *NOTNULL evclass,
                         conf_object_t *NOTNULL obj,
                         int (*pred)(lang_void *data, lang_void *match_data),
                         lang_void *match_data) {
    return 0;
}

#define LOG_TO_STRING()                   \
    char buf[1024];                       \
    va_list va;                           \
    va_start(va, str);                    \
    vsnprintf(buf, sizeof(buf), str, va); \
    va_end(va);                           \
    buf[1023] = 0;

void
VT_log_message(conf_object_t *obj, int level, int group_ids,
               log_type_t log_type, const char *message) {
    switch (log_type) {
        case Sim_Log_Info: {
            SIM_log_info(level, obj, group_ids, "%s", message);
        }
        break;
        case Sim_Log_Error: {
            SIM_log_error(obj, group_ids, "%s", message);
        }
        break;
        case Sim_Log_Spec_Violation: {
            SIM_log_spec_violation(level, obj, group_ids, "%s", message);
        }
        break;
        case Sim_Log_Unimplemented: {
            SIM_log_unimplemented(level, obj, group_ids, "%s", message);
        }
        break;
        default:
            break;
    }
}

void
SIM_log_info(int lvl, conf_object_t *dev, int grp, const char *str, ...) {
    if (lvl > Stubs::instance_.log_level_)
        return;

    Stubs::instance_.last_log_obj_ = dev;
    ++Stubs::instance_.SIM_log_info_count_;
    LOG_TO_STRING();
    Stubs::instance_.SIM_log_info_ = buf;
}

void
VT_log_info(int lvl, conf_object_t *dev, uint64 grp, const char *str, ...) {
    if (lvl > Stubs::instance_.log_level_)
        return;

    Stubs::instance_.last_log_obj_ = dev;
    ++Stubs::instance_.SIM_log_info_count_;
    LOG_TO_STRING();
    Stubs::instance_.SIM_log_info_ = buf;
}

void
SIM_log_error(conf_object_t *dev, int grp, const char *str, ...) {
    ++Stubs::instance_.SIM_log_error_count_;
    Stubs::instance_.last_log_obj_ = dev;
    LOG_TO_STRING();
    Stubs::instance_.SIM_log_error_ = buf;
}

void
VT_log_error(conf_object_t *dev, uint64 grp, const char *str, ...) {
    ++Stubs::instance_.SIM_log_error_count_;
    Stubs::instance_.last_log_obj_ = dev;
    LOG_TO_STRING();
    Stubs::instance_.SIM_log_error_ = buf;
}

static void
log_spec_violation(int lvl) {
    if (lvl <= Stubs::instance_.log_level_)
        ++Stubs::instance_.SIM_log_spec_violation_count_;
}

void
SIM_log_spec_violation(int lvl, conf_object_t *dev, int grp,
                       const char *str, ...) {
    log_spec_violation(lvl);
}

void
VT_log_spec_violation(int lvl, conf_object_t *dev, uint64 grp,
                      const char *str, ...) {
    log_spec_violation(lvl);
}

static void
log_unimplemented(int lvl, conf_object_t *dev) {
    if (lvl > Stubs::instance_.log_level_)
        return;

    ++Stubs::instance_.SIM_log_unimplemented_count_;
    Stubs::instance_.last_log_obj_ = dev;
}

void
SIM_log_unimplemented(int lvl, conf_object_t *dev, int grp,
                      const char *str, ...) {
    log_unimplemented(lvl, dev);
}

void
VT_log_unimplemented(int lvl, conf_object_t *dev, uint64 grp,
                     const char *str, ...) {
    log_unimplemented(lvl, dev);
}

void
SIM_log_critical(conf_object_t *NOTNULL dev, int grp,
                 const char *NOTNULL str, ...) {
    ++Stubs::instance_.SIM_log_critical_count_;
    Stubs::instance_.last_log_obj_ = dev;
    LOG_TO_STRING();
    Stubs::instance_.SIM_log_error_ = buf;
}

unsigned VT_effective_log_level(const conf_object_t *NOTNULL obj) {
    ++Stubs::instance_.VT_effective_log_level_;
    return 4;
}

void
VT_log_critical(conf_object_t *NOTNULL dev, uint64 grp,
                const char *NOTNULL str, ...) {
    ++Stubs::instance_.SIM_log_critical_count_;
    Stubs::instance_.last_log_obj_ = dev;
    LOG_TO_STRING();
    Stubs::instance_.SIM_log_error_ = buf;
}

bool SIM_object_is_configured(const conf_object_t *NOTNULL obj) {
    return Stubs::instance_.SIM_object_is_configured_;
}

bool SIM_is_restoring_state(conf_object_t *obj) {
    return Stubs::instance_.SIM_is_restoring_state_;
}

void SIM_set_mem_op_value_le(generic_transaction_t *NOTNULL mop, uint64 value) {
}

const interface_t *SIM_c_get_port_interface(const conf_object_t *NOTNULL obj,
                                            const char *NOTNULL name,
                                            const char *portname) {
    if (!portname || strlen(portname) == 0)
        return SIM_c_get_interface(obj, name);
    return Stubs::instance_.SIM_c_get_port_interface_;
}

map_target_t *SIM_new_map_target(conf_object_t *NOTNULL obj, const char *port,
                                 const map_target_t *chained_target) {
    if (obj == nullptr) {
        throw std::invalid_argument("NULL obj");
    }
    return nullptr;
}

void SIM_free_map_target(map_target_t *mt) {}

exception_type_t SIM_issue_transaction(const map_target_t *NOTNULL mt,
                                       transaction_t *NOTNULL t, uint64 addr) {
    return Sim_PE_No_Exception;
}

void
SIM_log_message_vararg(conf_object_t *obj, int level, uint64 group_ids,
                       log_type_t log_type, const char *str, ...) {
    if (level > Stubs::instance_.log_level_)
        return;
    
    switch (log_type) {
        case Sim_Log_Info: {
            Stubs::instance_.last_log_obj_ = obj;
            ++Stubs::instance_.SIM_log_info_count_;
            LOG_TO_STRING();
            Stubs::instance_.SIM_log_info_ = buf;
        }
        break;
        case Sim_Log_Error: {
            ++Stubs::instance_.SIM_log_error_count_;
            Stubs::instance_.last_log_obj_ = obj;
            LOG_TO_STRING();
            Stubs::instance_.SIM_log_error_ = buf;
        }
        break;
        case Sim_Log_Spec_Violation: {
            log_spec_violation(level);
        }
        break;
        case Sim_Log_Unimplemented: {
            log_unimplemented(level, obj);
        }
        break;
        case Sim_Log_Critical: {
            ++Stubs::instance_.SIM_log_critical_count_;
            Stubs::instance_.last_log_obj_ = obj;
            LOG_TO_STRING();
            Stubs::instance_.SIM_log_error_ = buf;
        }
        break;
        default:
            break;
    }
}

exception_type_t SIM_monitor_transaction(transaction_t *NOTNULL t,
                                         exception_type_t ex) {
    return Sim_PE_No_Exception;
}

bool SIM_transaction_is_read(const transaction_t *NOTNULL t) {
    // TODO(xiuliang): make it configurable
    return true;
}

unsigned SIM_transaction_size(const transaction_t *NOTNULL t) {
    // TODO(xiuliang): make it configurable
    return 4;
}

void SIM_get_transaction_bytes(const transaction_t *NOTNULL t,
                               buffer_t bytes) {
}
