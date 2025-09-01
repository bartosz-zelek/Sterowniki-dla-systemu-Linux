// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2019 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_AWARENESS_SC_ATTRIBUTE_H
#define SIMICS_SYSTEMC_AWARENESS_SC_ATTRIBUTE_H

#include <simics/systemc/awareness/attribute.h>

#include <systemc>

#include <string>

namespace simics {
namespace systemc {
namespace awareness {

class ScAttributeInterface {
  public:
    virtual ~ScAttributeInterface() {}
    virtual void init(sc_core::sc_object *object,
                      sc_core::sc_attr_base *attr) = 0;
    virtual int key() const = 0;
    virtual sc_core::sc_attr_base *attr() = 0;
    virtual sc_core::sc_attr_base *attr() const = 0;
};

template<class A>
class ScAttribute : public Attribute , public ScAttributeInterface {
  public:
    explicit ScAttribute(int key) : Attribute(key), attr_(NULL) {}
    virtual void init(sc_core::sc_object *object, sc_core::sc_attr_base *attr) {
        object_name_ = object->name();
        attr_name_ = attr->name();
        attr_ = attr;
    }
    virtual int key() const {
        return Attribute::key();
    }
    virtual Attribute *create() {
        sc_core::sc_object *obj = sc_core::sc_find_object(object_name_.c_str());
        if (obj)
            attr_ = obj->get_attribute(attr_name_);
        else
            attr_ = NULL;

        return new ScAttribute<A>(*this);
    }
    virtual sc_core::sc_attr_base *attr() {
        return attr_;
    }
    virtual sc_core::sc_attr_base *attr() const {
        return attr_;
    }
    virtual set_error_t set(attr_value_t *val) {
        A access;
        if (access.set(attr(), val))
            return Sim_Set_Ok;

        return Sim_Set_Illegal_Type;
    }
    virtual attr_value_t get() const {
        A access;
        return access.get(attr());
    }

  private:
    std::string object_name_;
    std::string attr_name_;
    sc_core::sc_attr_base *attr_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
