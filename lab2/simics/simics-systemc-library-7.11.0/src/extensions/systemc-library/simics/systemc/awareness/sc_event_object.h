// -*- mode: C++; c-file-style: "virtutech-c++" -*-

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

#ifndef SIMICS_SYSTEMC_AWARENESS_SC_EVENT_OBJECT_H
#define SIMICS_SYSTEMC_AWARENESS_SC_EVENT_OBJECT_H

#include <systemc>
#include <string>

namespace simics {
namespace systemc {
namespace awareness {

/** \internal */
class ScEventObject : public sc_core::sc_object {
  public:
    explicit ScEventObject(sc_core::sc_event *event)
        : sc_object(sc_core::sc_gen_unique_name(event->basename())),
          event_name_(event->name()) {}
    virtual const char *kind() const {
        return "ScEventObject";
    }
    sc_core::sc_event *get_event() {
        return sc_core::sc_find_event(event_name_.c_str());
    }

  protected:
    std::string event_name_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
