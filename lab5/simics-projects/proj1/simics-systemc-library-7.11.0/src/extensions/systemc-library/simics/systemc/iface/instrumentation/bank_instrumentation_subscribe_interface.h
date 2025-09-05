// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2019 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_INSTRUMENTATION_BANK_INSTRUMENTATION_SUBSCRIBE_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_INSTRUMENTATION_BANK_INSTRUMENTATION_SUBSCRIBE_INTERFACE_H

#include <stdint.h>

#include <simics/model-iface/bank-instrumentation.h>

namespace simics {
namespace systemc {
namespace iface {
namespace instrumentation {

/** \internal */
class BankInstrumentationSubscribeInterface {
  public:
    virtual bank_callback_handle_t register_before_read(
            conf_object_t *connection,
            uint64_t offset,
            uint64_t size,
            before_read_callback_t before_read,
            lang_void *user_data) = 0;
    virtual bank_callback_handle_t register_after_read(
            conf_object_t *connection,
            uint64_t offset,
            uint64_t size,
            after_read_callback_t after_read,
            lang_void *user_data) = 0;
    virtual bank_callback_handle_t register_before_write(
            conf_object_t *connection,
            uint64_t offset,
            uint64_t size,
            before_write_callback_t before_write,
            lang_void *user_data) = 0;
    virtual bank_callback_handle_t register_after_write(
            conf_object_t *connection,
            uint64_t offset,
            uint64_t size,
            after_write_callback_t after_write,
            lang_void *user_data) = 0;

    virtual void remove_callback(bank_callback_handle_t callback) = 0;
    virtual void remove_connection_callbacks(
            conf_object_t *NOTNULL connection) = 0;
    virtual void enable_connection_callbacks(
            conf_object_t *NOTNULL connection) = 0;
    virtual void disable_connection_callbacks(
            conf_object_t *NOTNULL connection) = 0;

    virtual ~BankInstrumentationSubscribeInterface() {}
};

}  // namespace instrumentation
}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
