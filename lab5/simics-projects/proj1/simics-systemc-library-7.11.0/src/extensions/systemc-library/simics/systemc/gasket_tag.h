// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_GASKET_TAG_H
#define SIMICS_SYSTEMC_GASKET_TAG_H

#include <simics/base/attr-value.h>
#include <simics/cc-api.h>

#include <map>
#include <string>
#include <vector>

namespace simics {
namespace systemc {

/**
 * This interface provides the means to create gaskets encapsulated in
 * Simics objects. Two methods are provided. One for adding the gasket and
 * the second one to add tags to the newly added gasket.*/
class GasketTagInterface {
  public:
    virtual ~GasketTagInterface() {}
    /**
     * Add a new gasket to the simulation. Id has to be unique and is the same
     * as for setGasketTag.
     * Type is one of the following gaskets:
     * - composite-Pci
     * - simics2tlm-EthernetCommon
     * - simics2tlm-I2cSlaveV2
     * - simics2tlm-IoMemory
     * - simics2tlm-PciDevice
     * - simics2tlm-PciExpress
     * - simics2tlm-SerialDevice
     * - tlm2simics-EthernetCommon
     * - tlm2simics-I2cMasterV2
     * - tlm2simics-MemorySpace
     * - tlm2simics-PciBus
     * - tlm2simics-SerialDevice
     */
    virtual void addGasket(std::string id, std::string type) = 0;
    /**
     * Set tags for the gasket. Id has to be unique and is the same as for
     * addGasket.
     * For simics2tlm gaskets, name has to be "target" and value
     * is the name of the SystemC socket object. For instance:
     * - setGasketTag("id_1", "target", SIM_make_attr_string("socket_name"));
     * For tlm2simics gaskets, name has to be "initiator".
     * For the composite-Pci gasket, name has to be "device".
     */
    virtual void setGasketTag(std::string id, std::string name,
                              attr_value_t value) = 0;
};

/** \internal */
class GasketTag : public GasketTagInterface {
  public:
    explicit GasketTag(ConfObjectRef adapter);
    virtual ~GasketTag();
    virtual void addGasket(std::string id, std::string type);
    virtual void setGasketTag(std::string id, std::string name,
                              attr_value_t value);
    std::vector<ConfObjectRef> createGasketObjects();

  private:
    void addAttrList(std::string id, attr_value_t attr);
    void addSimulationAttr(const std::string &id);

    std::map<std::string, std::string> id_to_class_;
    std::map<std::string, attr_value_t> id_to_attr_;
    ConfObjectRef adapter_;
};

}  // namespace systemc
}  // namespace simics

#endif
