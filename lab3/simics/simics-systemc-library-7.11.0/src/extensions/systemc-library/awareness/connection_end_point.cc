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

#include <simics/systemc/internals.h>
#include <simics/systemc/awareness/connection_end_point.h>

#include <sstream>
#include <string>
#include <map>

namespace simics {
namespace systemc {
namespace awareness {

std::map<sc_core::sc_interface *, conf_object_t *>
    ConnectionEndPoint::end_points_;

ConnectionEndPoint::ConnectionEndPoint(ConfObjectRef o)
    : simics::ConfObject(o) {
}

conf_class_t *ConnectionEndPoint::initClass() {
    conf_class_t *cls = SIM_get_class("ConnectionEndPoint");
    if (SIM_clear_exception() == SimExc_No_Exception) {
        return cls;
    }

    auto new_cls = make_class<ConnectionEndPoint>(
        "ConnectionEndPoint", "SystemC Connection End Point",
        "Class for representing non-SystemC objects in SystemC Library",
        Sim_Class_Kind_Pseudo);

    return *new_cls.get();
}

conf_object_t *ConnectionEndPoint::getEndPoint(
        iface::SimulationInterface *simulation, sc_core::sc_interface *key) {
    if (key == NULL)
        return NULL;

    std::map<sc_core::sc_interface *, conf_object_t *>::iterator i
        = end_points_.find(key);

    if (i != end_points_.end())
        return i->second;

    std::stringstream ss;
    ss << "sc_interface_at_" << key;
    std::string name = ss.str();
    std::string full_name = simulation->simics_object().name() + "." + name;

    conf_object_t *obj = createObject(name, full_name, simulation);
    end_points_.emplace(key, obj);
    return obj;
}

conf_object_t *ConnectionEndPoint::createObject(const std::string &name,
        const std::string &full_name, iface::SimulationInterface *simulation) {
    conf_class_t *cls = initClass();
    attr_value_t attrs = SIM_make_attr_list(0);
    conf_object_t *object = SIM_create_object(cls, full_name.c_str(), attrs);
    SIM_attr_free(&attrs);

    if (SIM_clear_exception() != SimExc_No_Exception) {
        return NULL;
    }

    return object;
}

}  // namespace awareness
}  // namespace systemc
}  // namespace simics
