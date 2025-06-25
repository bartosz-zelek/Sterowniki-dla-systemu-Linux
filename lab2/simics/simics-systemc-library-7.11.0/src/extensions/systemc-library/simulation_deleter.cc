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

#include <simics/conf-object.h>
#include <simics/simulator-api.h>
#include <simics/systemc/simulation_deleter.h>

#include <systemc-interfaces.h>

namespace simics {
namespace systemc {

int SimulationDeleter::instance_counter_ = 0;

SimulationDeleter::SimulationDeleter(conf_object_t *adapter)
    : context_(sc_core::sc_get_curr_simcontext()), context_allocated_(false) {
    SIM_hap_add_callback("Core_Conf_Object_Created",
        reinterpret_cast<obj_hap_func_t>(atCreate), this);
    SIM_hap_add_callback("Core_Conf_Object_Delete",
        reinterpret_cast<obj_hap_func_t>(atDelete), this);

    object_names_.insert(SIM_object_name(adapter));
#if INTC_EXT
    ++instance_counter_;
    if (instance_counter_ > 1 ||
        sc_core::sc_get_status() != sc_core::SC_ELABORATION) {
        context_ = new sc_core::sc_simcontext;
        context_allocated_ = true;
    }
#endif
}

SimulationDeleter::~SimulationDeleter() {
    SIM_hap_delete_callback("Core_Conf_Object_Created",
                            reinterpret_cast<obj_hap_func_t>(atCreate), this);
    SIM_hap_delete_callback("Core_Conf_Object_Delete",
                            reinterpret_cast<obj_hap_func_t>(atDelete), this);
    if (context_allocated_)
        delete context_;

    --instance_counter_;
}

sc_core::sc_simcontext *SimulationDeleter::context() {
    return context_;
}

void SimulationDeleter::atCreate(lang_void *simulation_deleter,
                                 conf_object_t *obj) {
    if (!obj)
        return;

    ConfObjectRef proxy(obj);
    const sc_object_interface_t *sc_obj =
        static_cast<const sc_object_interface_t *>(
            proxy.get_interface(SC_OBJECT_INTERFACE));

    if (sc_obj) {
        SimulationDeleter *deleter =
            static_cast<SimulationDeleter *>(simulation_deleter);

        deleter->object_names_.insert(SIM_object_name(obj));
    }
}

void SimulationDeleter::atDelete(lang_void *simulation_deleter,
                                 conf_object_t *obj,
                                 const char *objname) {
    SimulationDeleter *deleter =
        static_cast<SimulationDeleter *>(simulation_deleter);

    if (deleter->object_names_.find(objname) != deleter->object_names_.end()) {
        deleter->object_names_.erase(objname);
        if (deleter->object_names_.empty())
            delete deleter;
    }
}

}  // namespace systemc
}  // namespace simics
