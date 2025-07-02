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

#ifndef SIMICS_SYSTEMC_GASKET_INFO_H
#define SIMICS_SYSTEMC_GASKET_INFO_H

#include <simics/cc-api.h>

#include <simics/systemc/iface/sc_gasket_info_interface.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {

class GasketInfo : public iface::ScGasketInfoInterface {
  public:
    explicit GasketInfo(ConfObjectRef obj);
    const std::vector<std::vector<std::string> > *simics2tlm() override;
    const std::vector<std::vector<std::string> > *tlm2simics() override;
    const std::vector<std::vector<std::string> > *simics2systemc() override;
    const std::vector<std::vector<std::string> > *systemc2simics() override;
    virtual ~GasketInfo();

  private:
    ConfObjectRef obj_;
    std::vector<std::vector<std::string> > simics2tlm_;
    std::vector<std::vector<std::string> > tlm2simics_;
    std::vector<std::vector<std::string> > simics2systemc_;
    std::vector<std::vector<std::string> > systemc2simics_;
};

}  // namespace systemc
}  // namespace simics

#endif
