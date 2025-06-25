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

#ifndef SIMICS_SYSTEMC_IFACE_INSTRUMENTATION_BANK_INSTRUMENTATION_SUBSCRIBE_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_INSTRUMENTATION_BANK_INSTRUMENTATION_SUBSCRIBE_SIMICS_ADAPTER_H

#include <simics/model-iface/bank-instrumentation.h>
#include <simics/systemc/iface/simics_adapter.h>
#include <simics/systemc/iface/instrumentation/bank_instrumentation_subscribe_interface.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {
namespace instrumentation {

template<typename TBase,
         typename TInterface = BankInstrumentationSubscribeInterface>
class BankInstrumentationSubscribeSimicsAdapter
    : public SimicsAdapter<bank_instrumentation_subscribe_interface_t> {
  public:
    BankInstrumentationSubscribeSimicsAdapter()
        : SimicsAdapter<bank_instrumentation_subscribe_interface_t>(
            BANK_INSTRUMENTATION_SUBSCRIBE_INTERFACE, init_iface()) {
    }

    static bank_callback_handle_t register_before_read(
            conf_object_t *NOTNULL bank,
            conf_object_t *connection,
            ::uint64 offset,
            ::uint64 size,
            before_read_callback_t before_read,
            lang_void *user_data) {
        return adapter<TBase, TInterface>(bank)->register_before_read(
                connection, offset, size, before_read, user_data);
    }
    static bank_callback_handle_t register_after_read(
            conf_object_t *NOTNULL bank,
            conf_object_t *connection,
            ::uint64 offset,
            ::uint64 size,
            after_read_callback_t after_read,
            lang_void *user_data) {
        return adapter<TBase, TInterface>(bank)->register_after_read(
                connection, offset, size, after_read, user_data);
    }
    static bank_callback_handle_t register_before_write(
            conf_object_t *NOTNULL bank,
            conf_object_t *connection,
            ::uint64 offset,
            ::uint64 size,
            before_write_callback_t before_write,
            lang_void *user_data) {
        return adapter<TBase, TInterface>(bank)->register_before_write(
                connection, offset, size, before_write, user_data);
    }
    static bank_callback_handle_t register_after_write(
            conf_object_t *NOTNULL bank,
            conf_object_t *connection,
            ::uint64 offset,
            ::uint64 size,
            after_write_callback_t after_write,
            lang_void *user_data) {
        return adapter<TBase, TInterface>(bank)->register_after_write(
                connection, offset, size, after_write, user_data);
    }

    static void remove_callback(conf_object_t *NOTNULL bank,
                                bank_callback_handle_t callback) {
        adapter<TBase, TInterface>(bank)->remove_callback(callback);
    }

    static void remove_connection_callbacks(conf_object_t *NOTNULL bank,
                                            conf_object_t *NOTNULL connection) {
        adapter<TBase, TInterface>(bank)->remove_connection_callbacks(
                connection);
    }
    static void enable_connection_callbacks(conf_object_t *NOTNULL bank,
                                            conf_object_t *NOTNULL connection) {
        adapter<TBase, TInterface>(bank)->enable_connection_callbacks(
                connection);
    }
    static void disable_connection_callbacks(
            conf_object_t *NOTNULL bank, conf_object_t *NOTNULL connection) {
        adapter<TBase, TInterface>(bank)->disable_connection_callbacks(
                connection);
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    bank_instrumentation_subscribe_interface_t init_iface() {
        bank_instrumentation_subscribe_interface_t iface = {};
        iface.register_before_read = register_before_read;
        iface.register_after_read = register_after_read;
        iface.register_before_write = register_before_write;
        iface.register_after_write = register_after_write;
        iface.remove_callback = remove_callback;
        iface.remove_connection_callbacks = remove_connection_callbacks;
        iface.enable_connection_callbacks = enable_connection_callbacks;
        iface.disable_connection_callbacks = disable_connection_callbacks;
        return iface;
    }
};

}  // namespace instrumentation
}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
