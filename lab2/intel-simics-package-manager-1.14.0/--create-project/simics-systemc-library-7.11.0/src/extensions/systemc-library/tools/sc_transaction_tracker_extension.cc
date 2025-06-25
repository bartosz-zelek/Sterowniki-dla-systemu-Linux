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

#include <simics/simulator-api.h>
#include <simics/systemc/tools/sc_transaction_tracker_extension.h>

#include <limits>

namespace simics {
namespace systemc {
namespace tools {

TransactionTrackerExtension::TransactionTrackerExtension()
    : socket_obj_(NULL) {}

tlm::tlm_extension_base *TransactionTrackerExtension::clone() const {
    return new TransactionTrackerExtension(*this);
}

void TransactionTrackerExtension::copy_from(
        tlm::tlm_extension_base const &extension) {
    *this = static_cast<const TransactionTrackerExtension &>(extension);
}

unsigned int TransactionTrackerExtension::get_id() {
    return TransactionTrackerExtension::ID;
}

void TransactionTrackerExtension::add_sighting(
        ConfObjectRef obj,
        tlm::tlm_generic_payload const *trans,
        sc_core::sc_object const *socket_sc_obj) {
    Sightings &sightings = tool_sightings_[SIM_object_name(obj.object())];
    PayloadAttributes attributes = {
        trans->get_address()
    };
    Sighting sighting = {
        socket_sc_obj->name(),
        socket_sc_obj,
        attributes
    };
    sightings.push_back(sighting);
}

void TransactionTrackerExtension::log_transaction_history() const {
    for (ToolSightings::const_iterator it_tool = tool_sightings_.begin();
         it_tool != tool_sightings_.end();
         ++it_tool) {
        conf_object_t *tool_obj = SIM_get_object(it_tool->first);
        // Use (void) to avoid coverity warning "unchecked_return_value"
        (void)SIM_clear_exception();
        if (!tool_obj)
            continue;
        for (Sightings::const_iterator it = it_tool->second.begin();
             it != it_tool->second.end();
             ++it) {
            if (it == it_tool->second.begin())
                SIM_LOG_INFO(4, tool_obj, 0, "transaction history:");
            SIM_LOG_INFO(4, tool_obj, 0, " %s: address 0x%llx",
                         it->socket_name, it->attributes.address);
        }
    }
}

void TransactionTrackerExtension::set_initiator_socket(
        sc_core::sc_object const *socket_obj) {
    socket_obj_ = socket_obj;
}

sc_core::sc_object const *TransactionTrackerExtension::get_initiator_socket()
        const {
    return socket_obj_;
}

void TransactionTrackerExtension::clear() {
    tool_sightings_.clear();
    socket_obj_ = NULL;
}

}  // namespace tools
}  // namespace systemc
}  // namespace simics
