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

#ifndef SIMICS_SYSTEMC_AWARENESS_SC_PORT_CONNECTION_H
#define SIMICS_SYSTEMC_AWARENESS_SC_PORT_CONNECTION_H


#include <simics/systemc/iface/sc_port_interface.h>
#include <simics/systemc/iface/simulation_interface.h>

#include <systemc>

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace simics {
namespace systemc {
namespace awareness {

/** \internal
 * Class ScPortConnection implements iface::ScPortInterface.
 * Responsible to provide the connected export and port proxy objects for the
 * sc_port object provided in init.
 */
class ScPortConnection : public iface::ScPortInterface {
  public:
    typedef std::pair<sc_core::sc_interface *, std::string> keytype;
    typedef std::vector<sc_core::sc_port_base *> valuetype;

    ScPortConnection();
    void init(sc_core::sc_object *object,
              iface::SimulationInterface *simulation);
    virtual std::vector<conf_object_t *> port_to_proxies();
    virtual const char *if_typename();
    virtual int max_number_of_proxies();
    virtual ~ScPortConnection();

  protected:
    static std::map<keytype, valuetype> ports_;

  private:
    class Item {
      public:
        Item(sc_core::sc_interface *iface)  // NOLINT[runtime/explicit]
             : iface_(iface), port_(NULL) {}
        Item(sc_core::sc_port_base *port)  // NOLINT[runtime/explicit]
             : iface_(NULL), port_(port) {}

        sc_core::sc_interface* iface_;
        sc_core::sc_port_base* port_;
    };

    int countLevels(sc_core::sc_object *object);
    std::string extractStem(sc_core::sc_object *object);
    conf_object_t *ifaceToObj(sc_core::sc_interface *iface);
    bool compareStem(std::string stem, sc_core::sc_object *object);
    std::vector<Item> bind_info_;
    sc_core::sc_port_base *base_;
    std::string base_typename_;
    int max_number_of_proxies_;
    iface::SimulationInterface *simulation_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
