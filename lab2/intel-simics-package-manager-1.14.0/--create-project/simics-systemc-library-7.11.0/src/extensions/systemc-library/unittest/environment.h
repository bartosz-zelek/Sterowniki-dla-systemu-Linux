// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SYSTEMC_LIBRARY_UNITTEST_ENVIRONMENT_H
#define SYSTEMC_LIBRARY_UNITTEST_ENVIRONMENT_H

#include <string>
#include <map>
#include "mock/mock_simulation.h"

class EnvironmentBase {
  public:
    EnvironmentBase();
    virtual ~EnvironmentBase();
    void register_interface(std::string name, const interface_t *iface);

  public:
    static std::map<std::string, const interface_t *> interfaces;
};

class Environment : public EnvironmentBase {
  public:
    Environment();

  public:
    unittest::MockSimulation simulation_;
};

#endif
