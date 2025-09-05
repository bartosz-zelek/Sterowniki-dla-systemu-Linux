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

#ifndef SIMICS_SYSTEMC_SIMULATION_DELETER_H
#define SIMICS_SYSTEMC_SIMULATION_DELETER_H

#include <systemc>
#include <set>
#include <string>

namespace simics {
namespace systemc {

class SimulationDeleter {
  public:
    explicit SimulationDeleter(conf_object_t *adapter);
    ~SimulationDeleter();
    SimulationDeleter(const SimulationDeleter&) = delete;
    SimulationDeleter& operator=(const SimulationDeleter&) = delete;
    sc_core::sc_simcontext *context();

  private:
    static void atCreate(lang_void *simulation_deleter, conf_object_t *obj);
    static void atDelete(lang_void *simulation_deleter, conf_object_t *obj,
                         const char *objname);
    sc_core::sc_simcontext *context_;
    bool context_allocated_;
    std::set<std::string> object_names_;
    static int instance_counter_;
};

}  // namespace systemc
}  // namespace simics

#endif
