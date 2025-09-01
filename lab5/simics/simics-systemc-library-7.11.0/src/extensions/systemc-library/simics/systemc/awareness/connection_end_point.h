// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_AWARENESS_CONNECTION_END_POINT_H
#define SIMICS_SYSTEMC_AWARENESS_CONNECTION_END_POINT_H

#include <simics/cc-api.h>
#include <simics/systemc/iface/simulation_interface.h>

#include <string>
#include <map>

namespace simics {
namespace systemc {
namespace awareness {

/** \internal */
class ConnectionEndPoint : public ConfObject {
  public:
    explicit ConnectionEndPoint(ConfObjectRef o);
    static conf_class_t *initClass();
    static conf_object_t *getEndPoint(iface::SimulationInterface *simulation,
                                      sc_core::sc_interface *key);

  private:
    static conf_object_t *createObject(const std::string &name,
        const std::string &full_name, iface::SimulationInterface *simulation);

    static std::map<sc_core::sc_interface *, conf_object_t *> end_points_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
