/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SYSTEMC_INTERFACES_SYSTEMC_INTERFACES_H
#define SYSTEMC_INTERFACES_SYSTEMC_INTERFACES_H

#include <simics/device-api.h>
#include <simics/pywrap.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Deprecated interfaces, to be removed */
SIM_INTERFACE(sc_trace) {
    void (*set_trace)(conf_object_t *obj, bool enable);
};
#define SC_TRACE_INTERFACE "sc_trace"
        
//////////////////////////////////////////////////////////////////////
// Proxy class interfaces
//////////////////////////////////////////////////////////////////////
SIM_INTERFACE(sc_object) {
    attr_value_t (*sc_print)(conf_object_t *obj);
    attr_value_t (*sc_dump)(conf_object_t *obj);
    attr_value_t (*sc_kind)(conf_object_t *obj);
    const char *(*sc_name)(conf_object_t *obj);
};
#define SC_OBJECT_INTERFACE "sc_object"

SIM_INTERFACE(sc_module) {
    /* intentionally left empty for now, used to query type from python only */
    void (*_not_valid)(conf_object_t *obj);        
};
#define SC_MODULE_INTERFACE "sc_module"

SIM_INTERFACE(sc_process) {
    attr_value_t (*events)(conf_object_t *obj);
    const char *(*file)(conf_object_t *obj);
    int (*line)(conf_object_t *obj);
    int (*process_id)(conf_object_t *obj);
    char *(*dump_state)(conf_object_t *obj);
    bool (*initialize)(conf_object_t *obj);
    int (*state)(conf_object_t *obj);
    void (*run)(conf_object_t *obj);
};
#define SC_PROCESS_INTERFACE "sc_process"

SIM_INTERFACE(sc_initiator_gasket) {
    void (*set_dmi)(conf_object_t *obj, bool enable);
    bool (*is_dmi_enabled)(conf_object_t *obj);
    char *(*print_dmi_table)(conf_object_t *obj);
};
#define SC_INITIATOR_GASKET_INTERFACE "sc_initiator_gasket"

SIM_INTERFACE(sc_export) {
    attr_value_t (*export_to_proxy)(conf_object_t *obj);
    const char *(*if_typename)(conf_object_t *obj);
};
#define SC_EXPORT_INTERFACE "sc_export"

SIM_INTERFACE(sc_port) {
    attr_value_t (*port_to_proxies)(conf_object_t *obj);
    const char *(*if_typename)(conf_object_t *obj);
    int (*max_number_of_proxies)(conf_object_t *obj);
};
#define SC_PORT_INTERFACE "sc_port"

SIM_INTERFACE(sc_process_profiler) {
    uint64 (*min_time)(conf_object_t *obj);
    uint64 (*max_time)(conf_object_t *obj);
    uint64 (*total_time)(conf_object_t *obj);
    uint64 (*number_of_calls)(conf_object_t *obj);
};
#define SC_PROCESS_PROFILER_INTERFACE "sc_process_profiler"

SIM_INTERFACE(sc_process_profiler_control) {
    bool (*is_enabled)(conf_object_t *obj);
    void (*set_enabled)(conf_object_t *obj, bool enable);
    uint64 (*total_time)(conf_object_t *obj);
    uint64 (*total_number_of_calls)(conf_object_t *obj);
    void (*clear_data)(conf_object_t *obj);
};
#define SC_PROCESS_PROFILER_CONTROL_INTERFACE "sc_process_profiler_control"

SIM_INTERFACE(sc_memory_profiler) {
    attr_value_t (*allocations)(conf_object_t *obj);
};
#define SC_MEMORY_PROFILER_INTERFACE "sc_memory_profiler"

SIM_INTERFACE(sc_memory_profiler_control) {
    bool (*is_enabled)(conf_object_t *obj);
    void (*set_enabled)(conf_object_t *obj, bool enabled);
};
#define SC_MEMORY_PROFILER_CONTROL_INTERFACE "sc_memory_profiler_control"

SIM_INTERFACE(sc_signal_read) {
    attr_value_t (*read)(conf_object_t *obj);
};
#define SC_SIGNAL_READ_INTERFACE "sc_signal_read"

SIM_INTERFACE(sc_signal_write) {
    void (*write)(conf_object_t *obj, attr_value_t *value);
};
#define SC_SIGNAL_WRITE_INTERFACE "sc_signal_write"

typedef enum {
    SC_TLM_ACCEPTED,
    SC_TLM_UPDATED,
    SC_TLM_COMPLETED,
    SC_TLM_ATTR_ERROR
} sc_tlm_sync_enum_t;

SIM_INTERFACE(sc_tlm_fw_transport) {
    sc_tlm_sync_enum_t (*nb_transport_fw)(conf_object_t *obj,
                                          attr_value_t trans,
                                          uint64 phase, uint64 t);
    void (*b_transport)(conf_object_t *obj, attr_value_t trans, uint64 offset);
    bool (*get_direct_mem_ptr)(conf_object_t *obj, attr_value_t trans,
                               attr_value_t dmi_data);
    uint64 (*transport_dbg)(conf_object_t *obj, attr_value_t trans);
};
#define SC_TLM_FW_TRANSPORT_INTERFACE "sc_tlm_fw_transport"

SIM_INTERFACE(sc_tlm_bw_transport) {
    void (*invalidate_direct_mem_ptr)(conf_object_t *obj, uint64 start_range,
                                      uint64 end_range);
    sc_tlm_sync_enum_t (*nb_transport_bw)(conf_object_t *obj,
                                          attr_value_t trans,
                                          uint64 phase, uint64 t);
};
#define SC_TLM_BW_TRANSPORT_INTERFACE "sc_tlm_bw_transport"

SIM_INTERFACE(sc_vector) {
    attr_value_t (*get_elements)(conf_object_t *obj);
};
#define SC_VECTOR_INTERFACE "sc_vector"

SIM_INTERFACE(sc_event) {
    void (*notify)(conf_object_t *obj);
};
#define SC_EVENT_INTERFACE "sc_event"

SIM_INTERFACE(sc_register_access) {
    exception_type_t (*read)(conf_object_t *obj, uint64 address,
                             buffer_t value);
    exception_type_t (*write)(conf_object_t *obj, uint64 address,
                              bytes_t value);
};

#define SC_REGISTER_ACCESS_INTERFACE "sc_register_access"

SIM_INTERFACE(sc_memory_access) {
    exception_type_t (*read)(conf_object_t *obj, uint64 address,
                             buffer_t value, bool debug);
    exception_type_t (*write)(conf_object_t *obj, uint64 address,
                              bytes_t value, bool debug);
};

#define SC_MEMORY_ACCESS_INTERFACE "sc_memory_access"

//////////////////////////////////////////////////////////////////////
// Adapter interfaces
//////////////////////////////////////////////////////////////////////
SIM_INTERFACE(sc_simcontext) {
    uint64 (*time_stamp)(conf_object_t *o);
    uint64 (*delta_count)(conf_object_t *o);
    uint64 (*time_to_pending_activity)(conf_object_t *o);
    int (*status)(conf_object_t *o);
    attr_value_t (*events)(conf_object_t *o);
};
#define SC_SIMCONTEXT_INTERFACE "sc_simcontext"

SIM_INTERFACE(sc_version) {
    const char *(*kernel_version)(conf_object_t *o);
    const char *(*library_version)(conf_object_t *o);
    const char *(*library_kernel_version)(conf_object_t *o);
    attr_value_t (*versions)(conf_object_t *obj);
};
#define SC_VERSION_INTERFACE "sc_version"

SIM_INTERFACE(sc_gasket_info) {
    attr_value_t (*simics2tlm)(conf_object_t *obj);
    attr_value_t (*tlm2simics)(conf_object_t *obj);
    attr_value_t (*simics2systemc)(conf_object_t *obj);
    attr_value_t (*systemc2simics)(conf_object_t *obj);
};
#define SC_GASKET_INFO_INTERFACE "sc_gasket_info"

#ifdef __cplusplus
}
#endif

#endif
