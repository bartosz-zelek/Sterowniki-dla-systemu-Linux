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

#ifndef SIMICS_SYSTEMC_ARGUMENT_REGISTRY_H
#define SIMICS_SYSTEMC_ARGUMENT_REGISTRY_H

#include <systemc>

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace sc_core {

int sc_argc();
const char *const *sc_argv();
int sc_elab_and_sim(int argc, char *argv[]);

}

namespace simics {
namespace systemc {

/** \internal */
class ArgumentRegistry {
  public:
    void set_argv(const std::vector<std::string> &argv);
    const char *const *argv();
    int argc();
    static ArgumentRegistry *Instance();
  private:
    std::map<sc_core::sc_simcontext*,
             std::pair<std::vector<char *>,
                       std::vector<std::string> > > arguments_;
};

}  // namespace systemc
}  // namespace simics

#endif
