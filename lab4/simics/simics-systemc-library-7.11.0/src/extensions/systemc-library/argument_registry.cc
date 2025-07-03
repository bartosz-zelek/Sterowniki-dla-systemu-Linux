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

#include <simics/systemc/argument_registry.h>

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#if !SC_ELAB_AND_SIM_DISABLE
namespace sc_core {

int sc_argc() {
    return simics::systemc::ArgumentRegistry::Instance()->argc();
}
const char* const* sc_argv() {
    return simics::systemc::ArgumentRegistry::Instance()->argv();
}
int sc_elab_and_sim(int argc, char *argv[]) {
    // NOTE: not applicable in Simics, but:
    // - we use this to test that accellera modules work as expected
    // - it could be helpful to users who already call sc_elab_and_sim() if
    //   this can be used to set the argument vector
    std::vector<std::string> v(argv, argv+argc);
    simics::systemc::ArgumentRegistry::Instance()->set_argv(v);
    return 0;
}

}  // namespace sc_core
#endif

namespace simics {
namespace systemc {

char *convert(std::string &s) {  // NOLINT(runtime/references)
    return &s[0];
}

void ArgumentRegistry::set_argv(const std::vector<std::string> &argv) {
    std::pair<std::vector<char *>, std::vector<std::string> > &p =
        arguments_[sc_core::sc_curr_simcontext];

    p.second = argv;
    p.first.clear();
    std::transform(p.second.begin(), p.second.end(),
                   std::back_inserter(p.first), convert);
}
const char *const *ArgumentRegistry::argv() {
    std::pair<std::vector<char *>, std::vector<std::string> > &p =
        arguments_[sc_core::sc_curr_simcontext];
    if (p.first.size() == 0)
        return NULL;
    return &p.first[0];
}
int ArgumentRegistry::argc() {
    std::pair<std::vector<char *>, std::vector<std::string> > &p =
        arguments_[sc_core::sc_curr_simcontext];
    return p.first.size();
}
ArgumentRegistry *ArgumentRegistry::Instance() {
    static ArgumentRegistry registry;
    return &registry;
}

}  // namespace systemc
}  // namespace simics
