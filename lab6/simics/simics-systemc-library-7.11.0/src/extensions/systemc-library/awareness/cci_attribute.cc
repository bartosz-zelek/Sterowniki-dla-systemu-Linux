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

#include <simics/systemc/awareness/cci_attribute.h>

#include <simics/base/attr-value.h>
#include <simics/base/conf-object.h>

#include <simics/systemc/awareness/attribute.h>
#include <simics/systemc/awareness/attributes.h>
#include <simics/systemc/cci_configuration.h>

#include <string>

namespace simics {
namespace systemc {
namespace awareness {

#if INTC_EXT && USE_SIMICS_CCI
CciAttribute::CciAttribute(int key) : Attribute(key), parameter_(NULL) {}
#else
CciAttribute::CciAttribute(int key) : Attribute(key) {}
#endif

CciAttribute::~CciAttribute() {
#if INTC_EXT && USE_SIMICS_CCI
    delete parameter_;
#endif
}

CciAttribute::CciAttribute(const CciAttribute& copy)
    : Attribute(copy.key())
    , parameter_name_(copy.parameter_name_) {
    CciConfiguration cfg;
    // Perform parameter lookup so that we get the parameter that belongs
    // to the current systemc context.
#if INTC_EXT && USE_SIMICS_CCI
    parameter_ = new cci::cci_param_handle(
            cfg.getParameter(parameter_name_));
#endif
}

Attribute *CciAttribute::create() {
    return new CciAttribute(*this);
}

attr_value_t CciAttribute::get() const {
#if INTC_EXT && USE_SIMICS_CCI
    if (!parameter_)
        return SIM_make_attr_invalid();

    CciConfiguration cfg;
    return cfg.getAttribute(*parameter_);
#else
    return SIM_make_attr_invalid();
#endif
}

set_error_t CciAttribute::set(attr_value_t *val) {
#if INTC_EXT && USE_SIMICS_CCI
    CciConfiguration cfg;
    if (!parameter_)
        return Sim_Set_Object_Not_Found;

    if (!parameter_->is_valid())
        return Sim_Set_Object_Not_Found;

    if (parameter_->is_locked())
        return Sim_Set_Not_Writable;

    if (parameter_->get_mutable_type() == cci::CCI_IMMUTABLE_PARAM)
        return Sim_Set_Not_Writable;

    if (!cfg.setAttribute(*parameter_, *val))
        return Sim_Set_Illegal_Value;

    return Sim_Set_Ok;
#else
    return Sim_Set_Object_Not_Found;
#endif
}

int CciAttribute::define(std::string parameter_name) {
    CciAttribute *a = Attributes::defineAttribute<CciAttribute>();
    CciConfiguration cfg;

    a->parameter_name_ = parameter_name;
#if INTC_EXT && USE_SIMICS_CCI
    a->parameter_ = new cci::cci_param_handle(cfg.getParameter(parameter_name));
#endif
    return a->key();
}

}  // namespace awareness
}  // namespace systemc
}  // namespace simics
