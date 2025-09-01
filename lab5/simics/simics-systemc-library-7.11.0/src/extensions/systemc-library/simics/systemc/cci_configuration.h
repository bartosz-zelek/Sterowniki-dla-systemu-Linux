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

#ifndef SIMICS_SYSTEMC_CCI_CONFIGURATION_H
#define SIMICS_SYSTEMC_CCI_CONFIGURATION_H

#include <simics/base/attr-value.h>
#include <simics/cc-api.h>

#if INTC_EXT && USE_SIMICS_CCI
#include <cci_configuration>
#include <cci_utils/broker.h>
#endif

#include <systemc>

#include <utility>  // pair
#include <map>
#include <string>
#include <vector>

namespace simics {
namespace systemc {

class CciConfiguration {
  public:
    CciConfiguration();
    void setPresetValues(
            const std::vector<std::pair<std::string, std::string>> *attr);
    void logUnconsumedPresetValues(ConfObjectRef obj);
    static void cleanCache();
#if INTC_EXT && USE_SIMICS_CCI
    const char* simicsType(cci::cci_param_handle parameter);
    std::string simicsName(cci::cci_param_handle parameter);
    std::vector <cci::cci_param_handle> getParameters(
            sc_core::sc_object *object);
    cci::cci_param_handle getParameter(const std::string &name);
    attr_value_t getAttribute(const cci::cci_param_handle &parameter);
    bool setAttribute(cci::cci_param_handle parameter,
                      const attr_value_t &value);

  private:
    cci::cci_originator simics_;
    cci::cci_broker_handle broker_;
    static std::map<const sc_core::sc_object *,
                    std::vector <cci::cci_param_handle> > cache_;
#endif
};

}  // namespace systemc
}  // namespace simics

#endif  // SIMICS_SYSTEMC_CCI_CONFIGURATION_H
