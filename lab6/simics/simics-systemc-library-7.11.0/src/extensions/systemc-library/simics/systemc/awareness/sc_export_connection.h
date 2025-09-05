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

#ifndef SIMICS_SYSTEMC_AWARENESS_SC_EXPORT_CONNECTION_H
#define SIMICS_SYSTEMC_AWARENESS_SC_EXPORT_CONNECTION_H



#include <systemc>
#include <simics/systemc/iface/sc_export_interface.h>
#include <simics/systemc/iface/simulation_interface.h>

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace simics {
namespace systemc {
namespace awareness {

/** \internal
 * Class ScExportConnection implements iface::ScExportInterface.
 * Responsible to provide the connected export and port proxy objects for the
 * sc_export object provided in init.
 */
class ScExportConnection : public iface::ScExportInterface {
  public:
    typedef std::pair<sc_core::sc_interface *, std::string> keytype;
    typedef std::vector<sc_core::sc_export_base *> valuetype;

    ScExportConnection();
    void init(sc_core::sc_object *object,
              iface::SimulationInterface *simulation);
    virtual conf_object_t *export_to_proxy();
    virtual const char *if_typename();
    void insertKey(keytype key);
    static std::vector<sc_core::sc_export_base *> findExport(keytype key);
    static void addBinderExport(const keytype &key,
                                sc_core::sc_export_base *obj);
    static void removeBinderExport(keytype key, sc_core::sc_export_base *obj);
    virtual ~ScExportConnection();

  protected:
    static std::map<keytype, valuetype> exports_;

  private:
    int countLevels(sc_core::sc_object *object);
    bool isPartOfModule(sc_core::sc_object *object);
    std::set<keytype> keys_;
    sc_core::sc_export_base *base_;
    sc_core::sc_interface *iface_;
    int levels_;
    iface::SimulationInterface *simulation_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
