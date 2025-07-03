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

#include <simics/base/types.h>

#if defined(__GNUC__) && __GNUC__ >= 4 && (__GNUC__ >= 5 || __GNUC_MINOR__ >= 2)
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif

// Stubs for logging
#define SYSTEMC_LIBRARY_SIMICS_PROXY_INTERFACE_H

#define BOOST_TEST_MODULE SystemC_Library_Unittests

#include <boost/test/included/unit_test.hpp>

#include <systemc>

#include "formatter.h"

class domain_lock_t;
class generic_transaction_t;

struct TestFixture {
    TestFixture() {
        boost::unit_test::results_reporter::set_format(new Formatter);

        // suppress the warnings that we know are safe during unittesting
        sc_core::sc_report_handler::set_actions(
            sc_core::SC_ID_NO_SC_START_ACTIVITY_, sc_core::SC_DO_NOTHING);
        sc_core::sc_report_handler::set_actions(
            sc_core::SC_ID_INSTANCE_EXISTS_, sc_core::SC_DO_NOTHING);
    }
};

BOOST_GLOBAL_FIXTURE(TestFixture);

#include "stubs.h"

extern "C" {
lang_void *SIM_object_data(conf_object_t *NOTNULL obj) {
    return obj ? obj->instance_data : NULL;
}

const char *SIM_object_name(const conf_object_t *obj) {
    return "object_name";
}

const char *SIM_object_id(const conf_object_t *obj) {
    return "object_id";
}

unsigned SIM_log_level(const conf_object_t *NOTNULL obj) {
    return 4;  // always highest log-level
}

void VT_set_delete_protection(conf_object_t *obj, bool on) {
    BOOST_CHECK(obj != NULL);
}

int SIM_write(const void *src, int length) {
    return length;
}

void VT_register_thread(void) {
}

conf_object_t *SIM_get_object(const char *name) {
    if (strcmp(name, "sim") == 0)
        return &Stubs::instance_.sim_obj_;

    return NULL;
}

typedef int hap_handle_t;
typedef void (*obj_hap_func_t)();

hap_handle_t SIM_hap_add_callback(const char *NOTNULL hap,
                                  NOTNULL obj_hap_func_t func,
                                  lang_void *data) {
    return 0;
}

hap_handle_t SIM_hap_add_callback_obj(const char *NOTNULL hap,
                                      conf_object_t *NOTNULL obj,
                                      int flags,
                                      NOTNULL obj_hap_func_t func,
                                      lang_void *data) {
    return 0;
}

void SIM_hap_delete_callback(const char *NOTNULL hap,
                             NOTNULL obj_hap_func_t func,
                             lang_void *data) {
}

void SIM_hap_delete_callback_id(const char *NOTNULL hap, hap_handle_t handle) {
}

domain_lock_t *SIM_acquire_object(conf_object_t *NOTNULL obj,
                                  const char *NOTNULL func,
                                  const char *NOTNULL location) {
    return NULL;
}

domain_lock_t *SIM_acquire_target(conf_object_t *NOTNULL obj,
                                  const char *NOTNULL func,
                                  const char *NOTNULL location) {
    return NULL;
}

void SIM_release_object(conf_object_t *NOTNULL obj, domain_lock_t *lock) {
}

void SIM_release_target(conf_object_t *NOTNULL obj, domain_lock_t *lock) {
}

conf_object_t *SIM_object_clock(const conf_object_t *NOTNULL obj) {
    // NOTE: It is assumed that within the unittest framework, we can pass the
    //       object that implements the cycle interface to the
    //       SimulationTime::set_cycle_interface() method. This method expects
    //       the adapter as input and calls SIM_object_clock() to get the
    //       clock. Hence simply returning the input will work.
    return const_cast<conf_object_t*>(obj);
}

void SIM_c_set_mem_op_value_buf(generic_transaction_t *NOTNULL mop,
                                const uint8 *NOTNULL src) {
}

void SIM_c_get_mem_op_value_buf(const generic_transaction_t *NOTNULL mop,
                                uint8 *NOTNULL dst) {
}

void VT_log_warning(conf_object_t *NOTNULL dev, uint64 grp,
                    const char *NOTNULL str, ...) {}

void SIM_register_class_attribute(conf_class_t *NOTNULL cls,
                                  const char *NOTNULL name,
                                  attr_value_t (*get_attr)(conf_class_t *),
                                  set_error_t (*set_attr)(conf_class_t *,
                                                          attr_value_t *),
                                  attr_attr_t attr, const char *type,
                                  const char *desc) {}

int SIM_register_interface(conf_class_t *cls, const char *name,
                           const interface_t *iface) {
    return 0;
}

void SIM_register_attribute(
        conf_class_t *NOTNULL cls, const char *NOTNULL name,
        attr_value_t (*get_attr)(conf_object_t *),
        set_error_t (*set_attr)(conf_object_t *, attr_value_t *),
        attr_attr_t attr, const char *type, const char *desc) {}

void *SIM_get_class_data(conf_class_t *cls) {
    return nullptr;
}

void SIM_set_class_data(conf_class_t *cls, void *data) {}

void SIM_log_register_groups(conf_class_t *cls,
                             const char *const *names) {}

conf_class_t *SIM_create_class(const char *name,
                               const class_info_t *class_info) {
    return nullptr;
}

void SIM_register_port(conf_class_t *cls, const char *name,
                       conf_class_t *port_cls, const char *desc) {}

void VT_set_constructor_data(conf_class_t *cls, void *data) {}

}  // extern "C"

void assert_error(int line, const char *file, const char *mod_date,
                  const char *message) {
    std::ostringstream s;
    s << "Line: " << line << " File: " << file << std::endl;
    BOOST_FAIL(s.str());
    while (true) {
    }
}

void mm_free(void *ptr) {
    free(ptr);
}

void *mm_malloc(size_t size, size_t typesize, const char *type,
        const char *file, int line) {
    return malloc(size);
}

void *mm_realloc(void *ptr, size_t size, size_t typesize, const char *type,
         const char *file, int line) {
    return realloc(ptr, size);
}

Stubs Stubs::instance_;

namespace simics {

std::vector<std::string> expand_names(const std::string &name,
                                      const char delimiter) {
    return {};
}

namespace internal {
void deprecated(int when, const char *what, const char *ref) {
    BOOST_FAIL("Call to deprecated function");
}
}  // namespace internal
}  // namespace simics
