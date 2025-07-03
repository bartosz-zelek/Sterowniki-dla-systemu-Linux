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

#ifndef SIMICS_SYSTEMC_EVENT_H
#define SIMICS_SYSTEMC_EVENT_H


#include <systemc>

#include <string>
#include <vector>

namespace simics {
namespace systemc {

/** \internal */
class Event {
  public:
    enum Type {
        STATIC,
        EVENT,
        SIMICS_EVENT,
        OR_LIST,
        AND_LIST,
        TIMEOUT,
        EVENT_TIMEOUT,
        OR_LIST_TIMEOUT,
        AND_LIST_TIMEOUT
    };

    Event(const sc_core::sc_time &when, const std::string &obj_name,
          const Type &type, const std::string &kind = "");
    Event(const sc_core::sc_time &when, const std::string &obj_name,
          int type, const std::string &kind = "");

    sc_core::sc_time when() const;
    std::string what() const;
    std::string kind() const;
    std::string name() const;
    Type type() const;

  private:
    sc_core::sc_time when_;
    std::string obj_name_;
    std::string kind_;
    Type type_;
};

typedef std::vector<Event> EventVector;

}  // namespace systemc
}  // namespace simics

#endif
