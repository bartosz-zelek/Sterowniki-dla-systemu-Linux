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

#ifndef SIMICS_SYSTEMC_AWARENESS_CCI_ATTRIBUTE_H
#define SIMICS_SYSTEMC_AWARENESS_CCI_ATTRIBUTE_H

#include <simics/base/attr-value.h>
#include <simics/base/conf-object.h>

#if INTC_EXT && USE_SIMICS_CCI
#include <cci_configuration>
#endif

#include <simics/systemc/awareness/attribute.h>

#include <string>

namespace simics {
namespace systemc {
namespace awareness {

class CciAttribute : public Attribute {
  public:
    explicit CciAttribute(int key);
    virtual ~CciAttribute();
    CciAttribute(const CciAttribute& copy);
    virtual Attribute *create();
    virtual attr_value_t get() const;
    virtual set_error_t set(attr_value_t *val);
    static int define(std::string parameter_name);

    CciAttribute& operator=(const CciAttribute&) = delete;

  private:
    std::string parameter_name_;
#if INTC_EXT && USE_SIMICS_CCI
    cci::cci_param_handle *parameter_;
#endif
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
