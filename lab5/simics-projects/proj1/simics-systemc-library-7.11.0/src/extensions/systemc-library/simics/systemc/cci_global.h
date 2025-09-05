// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2020 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_CCI_GLOBAL_H
#define SIMICS_SYSTEMC_CCI_GLOBAL_H

#include <simics/cc-api.h>
#include <simics/simulator/sim-get-class.h>

#include <simics/systemc/awareness/attributes.h>
#include <simics/systemc/awareness/cci_attribute.h>
#include <simics/systemc/awareness/proxy_attribute_name.h>
#include <simics/systemc/cci_global.h>

#include <algorithm>
#include <string>
#include <vector>

namespace simics {
namespace systemc {

/** \internal */
class CciGlobal : public ConfObject {
  public:
    explicit CciGlobal(ConfObjectRef o) : ConfObject(o), attributes_(NULL) {
    }
    void init(awareness::Attributes *attributes) {
        attributes_ = attributes;
    }

    static attr_value_t getAttribute(lang_void *ptr, conf_object_t *obj,
                                     attr_value_t *idx) {
        uintptr_t key = reinterpret_cast<intptr_t>(ptr);
        CciGlobal *o = static_cast<CciGlobal *>(SIM_object_data(obj));
        attr_value_t result = o->attributes_->attribute<
                awareness::CciAttribute>(key)->get();
        return result;
    }

    static set_error_t setAttribute(lang_void *ptr, conf_object_t *obj,
                                    attr_value_t *val, attr_value_t *idx) {
        uintptr_t key = reinterpret_cast<intptr_t>(ptr);
        CciGlobal *o = static_cast<CciGlobal *>(SIM_object_data(obj));
        set_error_t result = o->attributes_->attribute<
                awareness::CciAttribute>(key)->set(val);
        return result;
    }

    static conf_class_t *initClass(const char *module_name,
                                   ConfObjectRef obj) {
        std::string cls_name(module_name);
        cls_name += "_cci_global";
        std::replace(cls_name.begin(), cls_name.end(), '-', '_');
        conf_class_t *cls = SIM_get_class(cls_name.c_str());
        if (SIM_clear_exception() == SimExc_No_Exception) {
            return cls;
        }

        auto new_cls = make_class<CciGlobal>(
                cls_name.c_str(),
                "Global CCI parameters.",
                "All CCI parameters on a global level.",
                Sim_Class_Kind_Pseudo);
#if INTC_EXT && USE_SIMICS_CCI
        CciConfiguration cfg;
        std::vector <cci::cci_param_handle> parameters =
            cfg.getParameters(NULL);
        std::vector <cci::cci_param_handle>::iterator i;
        for (i = parameters.begin(); i != parameters.end(); ++i) {
            const char *type = cfg.simicsType(*i);
            if (!type)
                continue;

            int key = awareness::CciAttribute::define(i->name());
            awareness::ProxyAttributeName attribute_name(cfg.simicsName(*i));
            if (attribute_name.transformed()) {
                SIM_LOG_INFO(4, obj, Log_Awareness,
                             "CCI Attribute %s is now %s.", i->name(),
                             attribute_name.c_str());
            }

            // SIM_register_typed_attribute expects a parameter of type void*.
            // On windows, casting int to void* fails because int has a
            // a different size compared to void*. According to spec, any
            // pointer can be cast to void* and size of char is one.
            char *key_arg = NULL;
            key_arg += key;
            SIM_register_typed_attribute(
                *new_cls.get(), attribute_name.c_str(),
                CciGlobal::getAttribute, key_arg,
                CciGlobal::setAttribute, key_arg,
                Sim_Attr_Pseudo, cfg.simicsType(*i), 0,
                i->get_description().c_str());
            if (SIM_clear_exception() != SimExc_No_Exception) {
                SIM_LOG_ERROR(obj, Log_Awareness,
                              "Failed to register CCI attribute: '%s'",
                              attribute_name.c_str());
            }
        }
#endif

        return SIM_get_class(cls_name.c_str());
    }

  private:
    awareness::Attributes *attributes_;
};

}  // namespace systemc
}  // namespace simics

#endif
