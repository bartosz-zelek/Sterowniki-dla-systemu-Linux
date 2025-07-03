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

#include <simics/systemc/simics2tlm/dmi_data_table.h>

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace simics2tlm {

unsigned char *DmiDataTable::dmiPointer(sc_dt::uint64 start,
                                        sc_dt::uint64 range,
                                        bool is_read) {
    for (std::vector<tlm::tlm_dmi>::iterator i = table_.begin();
         i != table_.end(); ++i) {
        if (start > i->get_end_address())
            continue;
        if (is_read && !i->is_read_allowed())
            continue;
        if (!is_read && !i->is_write_allowed())
            continue;
        if (start >= i->get_start_address()
            && start + range <= i->get_end_address() + 1) {
            return i->get_dmi_ptr() + start - i->get_start_address();
        } else {
            return NULL;
        }
    }
    return NULL;
}

bool comparator(const tlm::tlm_dmi &i, const tlm::tlm_dmi &j) {
    return i.get_start_address() < j.get_start_address();
}

void DmiDataTable::add(const tlm::tlm_dmi &dmi_data) {
    table_.push_back(dmi_data);
    std::sort(table_.begin(), table_.end(), comparator);
}

void DmiDataTable::remove(sc_dt::uint64 start_range, sc_dt::uint64 end_range) {
    for (std::vector<tlm::tlm_dmi>::iterator i = table_.begin();
         i != table_.end();) {
        // Three kinds of overlap for each entry:
        // No/Partial/fully overlap
        if ((start_range > i->get_end_address())
            || (end_range < i->get_start_address())) {
            ++i;
        } else if ((start_range <= i->get_start_address())
                   && (end_range >= i->get_end_address())) {
            i = table_.erase(i);
        } else {
            if (start_range <= i->get_start_address()) {
                unsigned char *ptr = i->get_dmi_ptr() - i->get_start_address();
                i->set_dmi_ptr(ptr + end_range + 1);
                i->set_start_address(end_range + 1);
            } else if (end_range >= i->get_end_address()) {
                i->set_end_address(start_range - 1);
            } else {
                tlm::tlm_dmi dmi = *i;
                dmi.set_end_address(start_range - 1);
                unsigned char *ptr = i->get_dmi_ptr() - i->get_start_address();
                i->set_dmi_ptr(ptr + end_range + 1);
                i->set_start_address(end_range + 1);

                i = table_.insert(i, dmi);
            }
            ++i;
        }
    }
}

std::string DmiDataTable::print() const {
    std::ostringstream os;
    for (std::vector<tlm::tlm_dmi>::const_iterator i = table_.begin();
         i != table_.end(); ++i) {
        os << "(" << (*i).get_start_address() << " "
           << (*i).get_end_address() << " " << (*i).get_granted_access()
           << ")";
    }
    return os.str();
}

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics
