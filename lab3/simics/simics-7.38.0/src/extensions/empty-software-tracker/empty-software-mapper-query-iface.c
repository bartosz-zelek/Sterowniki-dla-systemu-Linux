/*
  © 2016 Intel Corporation
*/

#include "empty-software-mapper-query-iface.h"
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
