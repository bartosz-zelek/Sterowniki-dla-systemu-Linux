// -*- mode: C++; c-file-style: "virtutech-c++" -*-

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

#ifndef SIMICS_SYSTEMC_TOOLS_SC_TRANSACTION_TRACKER_EXTENSION_H
#define SIMICS_SYSTEMC_TOOLS_SC_TRANSACTION_TRACKER_EXTENSION_H


#include <simics/systemc/awareness/proxy.h>
#include <systemc>
#include <tlm>

#include <cstring>
#include <map>
#include <utility>
#include <vector>

namespace simics {
namespace systemc {
namespace tools {

/* Extension for tracking TLM2 transaction changes.
   If you change the class, update corresponding GDB Pretty-Printer. */
class TransactionTrackerExtension
    : public tlm::tlm_extension<TransactionTrackerExtension> {
  public:
    TransactionTrackerExtension();

    virtual tlm::tlm_extension_base *clone() const;
    virtual void copy_from(tlm::tlm_extension_base const &extension);

    static unsigned int get_id();

    void add_sighting(simics::ConfObjectRef obj,
                      tlm::tlm_generic_payload const *trans,
                      sc_core::sc_object const *socket_sc_obj);
    void log_transaction_history() const;

    void set_initiator_socket(sc_core::sc_object const *socket_obj);
    sc_core::sc_object const *get_initiator_socket() const;

    void clear();

  private:
    struct CompareStr {
        bool operator()(const char *str1, const char *str2) const {
            return std::strcmp(str1, str2) < 0;
        }
    };

    struct PayloadAttributes {
        sc_dt::uint64 address;
    };

    struct Sighting {
        const char *socket_name;
        const sc_core::sc_object *socket_obj;
        PayloadAttributes attributes;
    };

    typedef std::vector<Sighting> Sightings;
    typedef std::map<const char *, Sightings, CompareStr> ToolSightings;
    ToolSightings tool_sightings_;

    sc_core::sc_object const *socket_obj_;
};

}  // namespace tools
}  // namespace systemc
}  // namespace simics

#endif
