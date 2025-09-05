/*
  keycodes.h

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef _KEYCODES_H
#define _KEYCODES_H

#include <simics/base-types.h>
#include <simics/model-iface/sim-keys.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define MAX_X11_KEYS            0x100

#define MAX_SCAN_CODE_LENGTH 8

#if !defined(PYWRAP)

int sim_key_to_scan(sim_key_t key, int key_up, char *buf,
                    int xt_style_conversion,
                    int shift_down, int ctrl_down,
                    int alt_down, int num_lock_on);
int sim_key_to_sun(sim_key_t key);
const char *sim_key_name(sim_key_t keycode);
sim_key_t x11_pc_key_to_sim(int keycode);
sim_key_t x11_pc_xi2_key_to_sim(int keycode);
sim_key_t x11_sun_key_to_sim(int keycode);
sim_key_t x11_mac_key_to_sim(int keycode);
sim_key_t x11_cooked_key_to_sim(int keycode);
int ascii_to_sim(int ascii);

#endif /* ! PYWRAP */

#if defined(__cplusplus)
}
#endif

#endif /* _KEYCODES_H */
