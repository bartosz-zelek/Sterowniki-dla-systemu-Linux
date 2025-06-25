/*
  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/cc-api.h>
#include <simics/systemc/awareness/proxy.h>
#include <simics/systemc/tools/sc_transaction_tracker_tool.h>

#include <string>
#include <iterator>
#include <limits>

namespace simics {
namespace systemc {
namespace tools {

void ScTransactionTrackerTool::allocate_extensions() {
    static const unsigned extra_extensions_count = 64;

    extensions_t::size_type initial_extensions_count = extensions_.size();
    extensions_.resize(initial_extensions_count + extra_extensions_count);

    extensions_t::iterator it = extensions_.begin();
    std::advance(it, initial_extensions_count);
    for (; it != extensions_.end(); ++it)
        extensions_free_list_.push(&*it);
}

ScTransactionTrackerTool::ScTransactionTrackerTool(ConfObjectRef o)
    : ScTool(o) {
    allocate_extensions();
}

void ScTransactionTrackerTool::nb_transport_fw_pre(
        scla::ProxyInterface *proxy_iface,
        tlm::tlm_generic_payload *trans,
        tlm::tlm_phase * /*phase*/,
        sc_core::sc_time * /*time*/) {
    handle_extension_pre(proxy_iface, trans);
}

void ScTransactionTrackerTool::nb_transport_fw_post(
        scla::ProxyInterface *proxy_iface,
        tlm::tlm_generic_payload *trans,
        tlm::tlm_phase * /*phase*/,
        sc_core::sc_time * /*time*/,
        tlm::tlm_sync_enum * /*ret*/) {
    handle_extension_post(proxy_iface, trans);
}

void ScTransactionTrackerTool::b_transport_pre(
        scla::ProxyInterface *proxy_iface,
        tlm::tlm_generic_payload *trans,
        sc_core::sc_time * /*time*/) {
    handle_extension_pre(proxy_iface, trans);
}

void ScTransactionTrackerTool::b_transport_post(
        scla::ProxyInterface *proxy_iface,
        tlm::tlm_generic_payload *trans,
        sc_core::sc_time * /*time*/) {
    handle_extension_post(proxy_iface, trans);
}

void ScTransactionTrackerTool::transport_dbg_pre(
        scla::ProxyInterface *proxy_iface,
        tlm::tlm_generic_payload *trans) {
    handle_extension_pre(proxy_iface, trans);
}

void ScTransactionTrackerTool::transport_dbg_post(
        scla::ProxyInterface *proxy_iface,
        tlm::tlm_generic_payload *trans,
        unsigned int * /*ret*/) {
    handle_extension_post(proxy_iface, trans);
}

void ScTransactionTrackerTool::nb_transport_bw_pre(
        scla::ProxyInterface *proxy_iface,
        tlm::tlm_generic_payload *trans,
        tlm::tlm_phase * /*phase*/,
        sc_core::sc_time * /*time*/) {
    handle_extension_pre(proxy_iface, trans);
}

void ScTransactionTrackerTool::nb_transport_bw_post(
        scla::ProxyInterface *proxy_iface,
        tlm::tlm_generic_payload *trans,
        tlm::tlm_phase * /*phase*/,
        sc_core::sc_time * /*time*/,
        tlm::tlm_sync_enum * /*ret*/) {
    handle_extension_post(proxy_iface, trans);
}

TransactionTrackerExtension* ScTransactionTrackerTool::find_free_extension() {
    if (extensions_free_list_.empty())
        allocate_extensions();

    TransactionTrackerExtension *extension = extensions_free_list_.front();
    extensions_free_list_.pop();
    return extension;
}

void ScTransactionTrackerTool::handle_extension_pre(
        scla::ProxyInterface *proxy_iface,
        tlm::tlm_generic_payload *trans) {
    scla::Proxy *proxy = static_cast<scla::Proxy *>(proxy_iface);

    /* As the extension is registered at runtime, it is required to allocate
       space for storing the extension pointer in the payload, i.e. call
       resize_extensions() function. */
    proxy->resizeExtensions(trans);

    unsigned int id = TransactionTrackerExtension::get_id();
    if (id == (std::numeric_limits<unsigned int>::max)())
        return;

    if (!trans->get_extension(id)) {
        TransactionTrackerExtension *new_extension = find_free_extension();
        new_extension->set_initiator_socket(proxy_iface->systemc_obj());
        trans->set_extension(id, new_extension);
    }

    TransactionTrackerExtension *extension =
        static_cast<TransactionTrackerExtension *>(trans->get_extension(id));
    if (extension) {
        extension->add_sighting(obj(), trans,
                                proxy_iface->systemc_obj());
    } else {
        SIM_LOG_ERROR(obj(), 0, "Expected extension in the payload.");
    }
}

void ScTransactionTrackerTool::handle_extension_post(
        scla::ProxyInterface *proxy_iface,
        tlm::tlm_generic_payload *trans) {
    unsigned int id = TransactionTrackerExtension::get_id();
    TransactionTrackerExtension *extension =
        static_cast<TransactionTrackerExtension *>(trans->get_extension(id));
    if (extension &&
        extension->get_initiator_socket() == proxy_iface->systemc_obj()) {
        extension->log_transaction_history();
        extension->clear();
        extensions_free_list_.push(extension);
        trans->set_extension(id, NULL);  // same as clear_extension()
    }
}

void ScTransactionTrackerTool::initialize(const std::string &module_name) {
    ScTool::register_class<ScTransactionTrackerTool>(
        module_name + "_sc_transaction_tracker_tool",
        "tests tracking TLM transactions",
        "SystemC Tool for tracking TLM transactions");
}

}  // namespace tools
}  // namespace systemc
}  // namespace simics
