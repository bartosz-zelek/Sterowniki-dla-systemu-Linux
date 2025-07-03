/*                                                             -*- C++ -*-

  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_MODULE_LOADED_H
#define SIMICS_SYSTEMC_MODULE_LOADED_H

#include <simics/simulator-api.h>

namespace simics {
namespace systemc {

class ModuleLoaded {
  public:
    ModuleLoaded();
    virtual ~ModuleLoaded();
    ModuleLoaded(const ModuleLoaded &other) = delete;
    ModuleLoaded& operator=(const ModuleLoaded &other) = delete;

    virtual void loaded(const char *module_name) = 0;
    static void set_module_name(const char *module_name);
    static const char *module_name();

  protected:
    void removeCallback();
    static void callback(lang_void *arg, conf_object_t * /* obj */,
                         const char *name);

  private:
    static const char *module_name_;
    hap_handle_t hap_;
};

}  // namespace systemc
}  // namespace simics

#endif  // SIMICS_SYSTEMC_MODULE_LOADED_H
