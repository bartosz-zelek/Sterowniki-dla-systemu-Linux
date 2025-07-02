/*
  gc.c - g-cache class

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <simics/simulator-api.h>

#include "gc-common.h"
#include "gc.h"

/* include the non-module specific code */
#include "gc-specialize.c"

/*
  Information for class registering
*/
void
init_local()
{
        const class_data_t gc_data = {
                .alloc_object = gc_alloc_object,
                .init_object = gc_init_object,
                .finalize_instance = gc_finalize_instance,
                .class_desc = "in-order 'flat' cache model",
                .description =
                "<class>g-cache</class> is an in-order 'flat' cache model "
                "with the following features: "
                "configurable number of lines, line size, associativity; "
                "indexing/tagging on physical/virtual addresses; "
                "configurable policies for write-allocate and write-back; "
                "random, cyclic and lru replacement policies; "
                "several caches can be chained; "
                "a single cache can be connected to several processors; "
                "support for a simple MESI protocol between caches."
                "\n\n"
                "It is imperative to start Simics with the <arg>-stall</arg> "
                "flag to get correct cache statistics. It is possible to "
                "start Simics without it, but no transactions will then be "
                "stalled, and all transaction may not be visible to the cache."
                "\n\n"
                "Note that the sample MESI protocol was written to handle "
                "simple cases such as several L1 write-through caches with L2 "
                "caches synchronizing via MESI. To model more complex "
                "protocols, you will need to modify <class>g-cache</class>."
                "\n\n"
                "See <cite>Simics Analyzer User's Guide</cite> for more "
                "information."
        };

        conf_class_t *gc_class = SIM_register_class("g-cache", &gc_data);
        if (gc_class == NULL) {
                pr_err("Could not create g-cache class\n");
                return;
        }

        /* register the attributes */
        gc_common_register(gc_class);
        gc_register(gc_class);

        /* register the timing model interface */
        static const timing_model_interface_t gc_ifc = {
                .operate = gc_operate
        };
        SIM_register_interface(gc_class, "timing_model", &gc_ifc);

        /* Register the cache control interface. */
        static const cache_control_interface_t cc_ifc = {
                .cache_control = cache_control
        };
        SIM_register_interface(gc_class, CACHE_CONTROL_INTERFACE,
                               &cc_ifc);

        /* Add the replacement policies. */
        add_repl_policy(init_rand_repl(gc_class));
        add_repl_policy(init_lru_repl(gc_class));
        add_repl_policy(init_cyclic_repl(gc_class));

        /* register the log groups */
        SIM_log_register_groups(gc_class, gc_log_groups);

        /* Initialize the other accompanying classes. */
        ts_init_local();
        sort_init_local();
}
