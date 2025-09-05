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

#ifndef SIMICS_SYSTEMC_IFACE_SC_PROCESS_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_SC_PROCESS_SIMICS_ADAPTER_H

#include <simics/systemc/iface/simics_adapter.h>

#include <systemc-interfaces.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** \internal */
template<typename TBase,
         typename TInterface = ScProcessInterface>
class ScProcessSimicsAdapter : public SimicsAdapter<sc_process_interface_t> {
  public:
    ScProcessSimicsAdapter()
        : SimicsAdapter<sc_process_interface_t>(
            SC_PROCESS_INTERFACE, init_iface()) {
    }

  protected:
    static attr_value_t events(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->events();
    }
    static const char *file(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->file();
    }
    static int line(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->line();
    }
    static int process_id(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->process_id();
    }
    static char *dump_state(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->dump_state();
    }
    static bool initialize(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->initialize();
    }
    static int state(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->state();
    }
    static void run(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->run();
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    sc_process_interface_t init_iface() {
        sc_process_interface_t iface = {};
        iface.events = events;
        iface.file = file;
        iface.line = line;
        iface.process_id = process_id;
        iface.dump_state = dump_state;
        iface.initialize = initialize;
        iface.state = state;
        iface.run = run;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
