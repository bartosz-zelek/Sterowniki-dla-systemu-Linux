/*
  © 2020 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "real-network.h"

static conf_object_t *
alloc(conf_class_t *cls)
{
        eth_bridge_t *ebt = MM_ZALLOC(1, eth_bridge_t);
        return &ebt->obj;
}

static void *
init(conf_object_t *obj)
{
        eth_bridge_t *eb = from_obj(obj);

        init_eth_adapter(&eb->ea, obj);
        init_rn_tap(eb, &eb->rt);
        return &eb->obj;
}

static void
finalize(conf_object_t *obj)
{
        eth_bridge_t *eb = from_obj(obj);
        finish_eth_setup(&eb->ea);
        finish_rn_tap_setup(&eb->rt);
}

static void
deinit(conf_object_t *obj)
{
        eth_bridge_t *eb = from_obj(obj);

        fini_rn_tap(&eb->rt);
        fini_eth_adapter(&eb->ea);
}

static void
dealloc(conf_object_t *obj)
{
        eth_bridge_t *eb = from_obj(obj);
        MM_FREE(eb);
}

void
create_eth_bridge_class()
{
        const class_info_t funcs = {
                .alloc = alloc,
                .init = init,
                .finalize = finalize,
                .deinit = deinit,
                .dealloc = dealloc,
                .short_desc = "an Ethernet bridge using TAP interface",
                .description = "Ethernet bridge using a TAP interface."
        };
        conf_class_t *class = SIM_create_class("rn-eth-bridge-tap", &funcs);
        register_rn_tap_attributes(class);
        register_eth_adapter_attributes(class);
        register_real_network_interface(class);
}
