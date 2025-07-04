// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2025 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

// This file is generated by the script bin/gen-cc-interface

#ifndef SIMICS_CPP_SIMULATOR_IFACE_OSA_NODE_PATH_INTERFACE_H
#define SIMICS_CPP_SIMULATOR_IFACE_OSA_NODE_PATH_INTERFACE_H

#include "simics/simulator-iface/osa-node-path-interface.h"

#include <simics/detail/conf-object-util.h>  // get_interface
#include <simics/iface/interface-info.h>

namespace simics {
namespace iface {

class OsaNodePathInterface {
  public:
    using ctype = osa_node_path_interface_t;

    // Function override and implemented by user
    virtual attr_value_t matching_nodes(node_id_t root_id, attr_value_t node_path_pattern) = 0;
    virtual attr_value_t node_path(node_id_t node_id) = 0;

    // Function convert C interface call to C++ interface call
    class FromC {
      public:
        static attr_value_t matching_nodes(conf_object_t *obj, node_id_t root_id, attr_value_t node_path_pattern) {
            return detail::get_interface<OsaNodePathInterface>(obj)->matching_nodes(root_id, node_path_pattern);
        }
        static attr_value_t node_path(conf_object_t *obj, node_id_t node_id) {
            return detail::get_interface<OsaNodePathInterface>(obj)->node_path(node_id);
        }
    };

    // Function convert C++ interface call to C interface call
    class ToC {
      public:
        ToC() : obj_(nullptr), iface_(nullptr) {}
        ToC(conf_object_t *obj, const OsaNodePathInterface::ctype *iface)
            : obj_(obj), iface_(iface) {}

        attr_value_t matching_nodes(node_id_t root_id, attr_value_t node_path_pattern) const {
            return iface_->matching_nodes(obj_, root_id, node_path_pattern);
        }
        attr_value_t node_path(node_id_t node_id) const {
            return iface_->node_path(obj_, node_id);
        }

        const OsaNodePathInterface::ctype *get_iface() const {
            return iface_;
        }

      private:
        conf_object_t *obj_;
        const OsaNodePathInterface::ctype *iface_;
    };

    class Info : public InterfaceInfo {
      public:
        // InterfaceInfo
        std::string name() const override { return OSA_NODE_PATH_INTERFACE; }
        const interface_t *cstruct() const override {
            static constexpr OsaNodePathInterface::ctype funcs {
                FromC::matching_nodes,
                FromC::node_path,
            };
            return &funcs;
        }
    };
};

}  // namespace iface
}  // namespace simics

#endif
