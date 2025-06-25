// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_DMI_DATA_TABLE_H
#define SIMICS_SYSTEMC_SIMICS2TLM_DMI_DATA_TABLE_H

#include <systemc>
#include <tlm>
#include <string>
#include <ostream>  // NOLINT(readability/streams)
#include <vector>

namespace simics {
namespace systemc {
namespace simics2tlm {

/** \internal */
class DmiDataTable {
  public:
    unsigned char *dmiPointer(sc_dt::uint64 start, sc_dt::uint64 range,
                              bool is_read);
    void add(const tlm::tlm_dmi &dmi_data);
    void remove(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
    std::string print() const;
  private:
    std::vector<tlm::tlm_dmi> table_;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
