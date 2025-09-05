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
#include "lowlevel.h"
#include <net-utils.h>

int
pl_incref(packet_loop_info_t *pl)
{
        return ++pl->refcount;
}

int
pl_decref(packet_loop_info_t *pl)
{
        int i = --pl->refcount;
        if (i == 0) {
                QFREE(pl->packet_queue);
                pthread_mutex_destroy(&pl->main_mutex);
        }
        return i;
}

static void
generic_packet_loop(void *arg)
{
        packet_loop_info_t *pl = (packet_loop_info_t *)arg;
        captured_packet_t packet;

        char *eb_obj_id;
        domain_lock_t *lock;

        if (pl->exiting) {
                return;
        }

        // copy string since there is no object exist check, used in exception below
        eb_obj_id = MM_STRDUP(SIM_object_id(&pl->eb->obj));

        SIM_ACQUIRE_CELL(&pl->eb->obj, &lock);

        if (pl->exiting) {
                /* edge case due to thread mixing,
                *  if SIM_ACQUIRE_CELL has been called and then real-network
                *  is taken down and object is deleted, SIM_RELEASE_CELL 
                *  needs to be called but isn't in some cases
                */
                if (SIM_get_object(eb_obj_id) != NULL) {
                        SIM_RELEASE_CELL(&pl->eb->obj, &lock);
                }

                MM_FREE(eb_obj_id);
                return;
        }

        if (pl_decref(pl) == 0)
                goto out;

        dbuffer_t *frame = new_dbuffer();
        for (;;) {
                pthread_mutex_lock(&pl->main_mutex);
                if (QEMPTY(pl->packet_queue)) {
                        /* tell thread that it needs to wake us up when more
                           packets arrive */
                        pl->packet_handler_awake = 0;
                        pthread_mutex_unlock(&pl->main_mutex);
                        break;
                }
                packet = QREMOVE(pl->packet_queue);
                pthread_mutex_unlock(&pl->main_mutex);

                /* let dbuffer adopt the MM_MALLOC'ed data */
                dbuffer_append_external_data(frame, packet.data, packet.len,
                                             1);

                if (SIM_simics_is_running()) {
                        /* pass packet to incoming handler */
                        bridge_incoming_packet(pl->eb, frame);
                }

                dbuffer_clear(frame);

                if (pl->exiting) {
                        break;
                }
        }
        dbuffer_free(frame);
  out:
        SIM_RELEASE_CELL(&pl->eb->obj, &lock);

        MM_FREE(eb_obj_id);
}


void
generic_receive_async(packet_loop_info_t *pl, int len, const void *data)
{
        captured_packet_t packet;
        int awake;

        if (pl->exiting) {
                return;
        }

        packet.data = MM_MALLOC(len, uint8);
        memcpy(packet.data, data, len);
        packet.len = len;

        pthread_mutex_lock(&pl->main_mutex);

        /* enqueue packet to be handled in main thread */
        QADD(pl->packet_queue, packet);

        /* wake up main thread, if necessary */
        awake = pl->packet_handler_awake;
        pl->packet_handler_awake = 1;
        pthread_mutex_unlock(&pl->main_mutex);

        if (!awake) {
                /* cannot have locks held when calling this, since it might
                   execute right away */
                pl_incref(pl);
                SIM_run_in_thread(generic_packet_loop, pl);
        }
}


void
init_packet_loop(packet_loop_info_t *pl, eth_bridge_t *eb)
{
        pthread_mutex_init(&pl->main_mutex, NULL);
        QINIT(pl->packet_queue);

        pl->eb = eb;
        pl->refcount = 1;
}


void
fini_packet_loop(packet_loop_info_t *pl)
{
        pl->exiting = 1;

        while (pl_decref(pl) > 0);
}

void
register_real_network_interface(conf_class_t *cls)
{
        static const real_network_interface_t rni = { 0 };
        SIM_register_interface(cls, REAL_NETWORK_INTERFACE, &rni);
}


void
init_local()
{
        create_eth_bridge_class();
        register_delayed_packet_event();
}
