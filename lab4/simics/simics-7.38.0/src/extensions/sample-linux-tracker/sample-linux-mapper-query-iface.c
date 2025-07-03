/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "sample-linux-mapper-query-iface.h"
#include <simics/simulator-iface/os-awareness-tracker-interfaces.h>

void
register_mapper_query_iface(conf_class_t *cls)
{
        static const osa_mapper_query_interface_t mapper_query_iface = {
                .get_process_list = NULL,  /* Optional */
                .get_mapper = NULL,  /* Optional */
        };
        SIM_register_interface(cls, OSA_MAPPER_QUERY_INTERFACE,
                               &mapper_query_iface);
}
