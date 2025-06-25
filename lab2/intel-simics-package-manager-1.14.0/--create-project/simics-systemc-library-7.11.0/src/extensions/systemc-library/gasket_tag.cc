// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/base/attr-value.h>
#include <simics/cc-api.h>

#include <simics/systemc/module_loaded.h>
#include <simics/systemc/gasket_tag.h>

#include <algorithm>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace simics {
namespace systemc {

GasketTag::GasketTag(ConfObjectRef adapter) {
    adapter_ = adapter;
}

GasketTag::~GasketTag() {
    std::map<std::string, attr_value_t>::iterator i;
    for (i = id_to_attr_.begin(); i != id_to_attr_.end(); ++i)
        SIM_attr_free(&i->second);
}

void GasketTag::addGasket(std::string id, std::string type) {
    std::string &conf_class = id_to_class_[id];
    conf_class = ModuleLoaded::module_name();
    conf_class += "_gasket_";
    conf_class += type;
    std::replace(conf_class.begin(), conf_class.end(), '-', '_');
}

void GasketTag::setGasketTag(std::string id, std::string name,
                             attr_value_t value) {
    addAttrList(std::move(id),
                SIM_make_attr_list(2, SIM_make_attr_string(name.c_str()),
                                   value));
}

std::vector<ConfObjectRef> GasketTag::createGasketObjects() {
    std::vector<ConfObjectRef> ret;
    std::map<std::string, std::string>::iterator i;
    for (i = id_to_class_.begin(); i != id_to_class_.end(); ++i) {
        conf_class_t *cls = SIM_get_class(i->second.c_str());
        if (SIM_clear_exception() != SimExc_No_Exception) {
            FATAL_ERROR("GasketTag::createGasketObjects: %s", SIM_last_error());
        }
        addSimulationAttr(i->first);
        auto it = id_to_attr_.find(i->first);
        assert(it != id_to_attr_.end());
        ret.push_back(SIM_create_object(cls, i->first.c_str(), it->second));
    }

    std::map<std::string, attr_value_t>::iterator j;
    for (j = id_to_attr_.begin(); j != id_to_attr_.end(); ++j) {
        if (id_to_class_.find(j->first) != id_to_class_.end())
            continue;

        std::string msg("Missing addGasket invocation for id: ");
        msg += j->first;
        SIM_LOG_ERROR(adapter_, 0, "%s", msg.c_str());
    }

    return ret;
}

void GasketTag::addAttrList(std::string id, attr_value_t attr) {
    std::map<std::string, attr_value_t>::iterator i;
    i = id_to_attr_.find(id);
    int insert = 0;
    if (i == id_to_attr_.end()) {
        i = id_to_attr_.emplace(id, SIM_alloc_attr_list(1)).first;
    } else {
        insert = SIM_attr_list_size(i->second);
        SIM_attr_list_resize(&i->second, insert + 1);
    }

    SIM_attr_list_set_item(&i->second, insert, attr);
}

void GasketTag::addSimulationAttr(const std::string &id) {
    addAttrList(id,
                SIM_make_attr_list(2,
                                   SIM_make_attr_string("simulation"),
                                   SIM_make_attr_object(adapter_.object())));
}

}  // namespace systemc
}  // namespace simics
