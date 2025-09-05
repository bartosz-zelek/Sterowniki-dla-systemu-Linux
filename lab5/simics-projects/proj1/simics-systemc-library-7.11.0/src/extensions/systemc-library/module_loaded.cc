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

#include <simics/systemc/iface/iface.h>
#include <simics/systemc/module_loaded.h>

#include <string>

namespace simics {
namespace systemc {

const char *ModuleLoaded::module_name_ = NULL;

ModuleLoaded::ModuleLoaded() {
    hap_ = SIM_hap_add_callback("Core_Module_Loaded",
                                reinterpret_cast<obj_hap_func_t>(callback),
                                this);
}

ModuleLoaded::~ModuleLoaded() {
    removeCallback();
}

void ModuleLoaded::set_module_name(const char *module_name) {
    module_name_ = module_name;
}

const char *ModuleLoaded::module_name() {
    return module_name_;
}

void ModuleLoaded::removeCallback() {
    if (hap_ != -1)
        SIM_hap_delete_callback_id("Core_Module_Loaded", hap_);

    hap_ = -1;
}

void ModuleLoaded::callback(lang_void *arg, conf_object_t *,
                            const char *name) {
    ModuleLoaded *that = reinterpret_cast<ModuleLoaded *>(arg);
    if (module_name_ && std::string(module_name_) == name) {
        that->removeCallback();
        that->loaded(module_name_);
    }
}

}  // namespace systemc
}  // namespace simics
