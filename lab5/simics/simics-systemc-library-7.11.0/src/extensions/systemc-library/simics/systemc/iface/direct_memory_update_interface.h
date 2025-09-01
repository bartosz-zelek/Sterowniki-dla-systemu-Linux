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

#ifndef SIMICS_SYSTEMC_IFACE_DIRECT_MEMORY_UPDATE_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_DIRECT_MEMORY_UPDATE_INTERFACE_H

#include <simics/model-iface/direct-memory.h>

namespace simics {
namespace systemc {
namespace iface {

/** Simics direct_memory_update interface. */
class DirectMemoryUpdateInterface {
  public:
    virtual void release(conf_object_t *target,
                         direct_memory_handle_t handle,
                         direct_memory_ack_id_t id) = 0;
    virtual void update_permission(conf_object_t *target,
                                   direct_memory_handle_t handle,
                                   access_t lost_access,
                                   access_t lost_permission,
                                   access_t lost_inhibit,
                                   direct_memory_ack_id_t id) = 0;
    virtual void conflicting_access(conf_object_t *target,
                                    direct_memory_handle_t handle,
                                    access_t conflicting_permission,
                                    direct_memory_ack_id_t id) = 0;
    virtual ~DirectMemoryUpdateInterface() {}
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
