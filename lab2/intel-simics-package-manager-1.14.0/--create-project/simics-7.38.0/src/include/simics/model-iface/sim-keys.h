/*
  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_MODEL_IFACE_SIM_KEYS_H
#define SIMICS_MODEL_IFACE_SIM_KEYS_H

#if defined(__cplusplus)
extern "C" {
#endif

#define MOUSE_LEFT_DOWN   0x01
#define MOUSE_RIGHT_DOWN  0x02
#define MOUSE_MIDDLE_DOWN 0x04
#define MOUSE_4_DOWN      0x08
#define MOUSE_5_DOWN      0x10


/* keyboard_info codes */
#define KBD_CAPSLOCK_ON    0x00
#define KBD_CAPSLOCK_OFF   0x01
#define KBD_NUMLOCK_ON     0x02
#define KBD_NUMLOCK_OFF    0x03
#define KBD_SCROLLLOCK_ON  0x04
#define KBD_SCROLLLOCK_OFF 0x05

#define SIM_INTERNAL_KEYS(op)                                           \
        op(ILLEGAL, 0)                                                  \
                                                                        \
        /* MF II keyboard keys (= standard 101 keyboard) */             \
        op(ESC, 1)                                                      \
        op(F1, 2)                                                       \
        op(F2, 3)                                                       \
        op(F3, 4)                                                       \
        op(F4, 5)                                                       \
        op(F5, 6)                                                       \
        op(F6, 7)                                                       \
        op(F7, 8)                                                       \
        op(F8, 9)                                                       \
        op(F9, 10)                                                      \
        op(F10, 11)                                                     \
        op(F11, 12)                                                     \
        op(F12, 13)                                                     \
                                                                        \
        op(PRNT_SCRN, 14)                                               \
        op(SCROLL_LOCK, 15)                                             \
        op(NUM_LOCK, 16)                                                \
        op(CAPS_LOCK, 17)                                               \
                                                                        \
        op(0, 18)                                                       \
        op(1, 19)                                                       \
        op(2, 20)                                                       \
        op(3, 21)                                                       \
        op(4, 22)                                                       \
        op(5, 23)                                                       \
        op(6, 24)                                                       \
        op(7, 25)                                                       \
        op(8, 26)                                                       \
        op(9, 27)                                                       \
        op(A, 28)                                                       \
        op(B, 29)                                                       \
        op(C, 30)                                                       \
        op(D, 31)                                                       \
        op(E, 32)                                                       \
        op(F, 33)                                                       \
        op(G, 34)                                                       \
        op(H, 35)                                                       \
        op(I, 36)                                                       \
        op(J, 37)                                                       \
        op(K, 38)                                                       \
        op(L, 39)                                                       \
        op(M, 40)                                                       \
        op(N, 41)                                                       \
        op(O, 42)                                                       \
        op(P, 43)                                                       \
        op(Q, 44)                                                       \
        op(R, 45)                                                       \
        op(S, 46)                                                       \
        op(T, 47)                                                       \
        op(U, 48)                                                       \
        op(V, 49)                                                       \
        op(W, 50)                                                       \
        op(X, 51)                                                       \
        op(Y, 52)                                                       \
        op(Z, 53)                                                       \
                                                                        \
        op(APOSTROPHE, 54)                                              \
        op(COMMA, 55)                                                   \
        op(PERIOD, 56)                                                  \
        op(SEMICOLON, 57)                                               \
        op(EQUAL, 58)                                                   \
        op(SLASH, 59)                                                   \
        op(BACKSLASH, 60)                                               \
        op(SPACE, 61)                                                   \
        op(LEFT_BRACKET, 62)                                            \
        op(RIGHT_BRACKET, 63)                                           \
        op(MINUS, 64)                                                   \
        op(GRAVE, 65)                                                   \
                                                                        \
        op(TAB, 66)                                                     \
        op(ENTER, 67)                                                   \
        op(BACKSPACE, 68)                                               \
                                                                        \
        op(CTRL_L, 69)                                                  \
        op(CTRL_R, 70)  /* Not on Sun */                                \
        op(SHIFT_L, 71)                                                 \
        op(SHIFT_R, 72)                                                 \
        op(ALT_L, 73)                                                   \
        op(ALT_R, 74)                                                   \
                                                                        \
        op(GR_DIVIDE, 75)                                               \
        op(GR_MULTIPLY, 76)                                             \
        op(GR_MINUS, 77)                                                \
        op(GR_PLUS, 78)                                                 \
        op(GR_ENTER, 79)                                                \
        op(GR_INSERT, 80)                                               \
        op(GR_HOME, 81)                                                 \
        op(GR_PG_UP, 82)                                                \
        op(GR_DELETE, 83)                                               \
        op(GR_END, 84)                                                  \
        op(GR_PG_DOWN, 85)                                              \
        op(GR_UP, 86)                                                   \
        op(GR_DOWN, 87)                                                 \
        op(GR_LEFT, 88)                                                 \
        op(GR_RIGHT, 89)                                                \
                                                                        \
        op(KP_HOME, 90)                                                 \
        op(KP_UP, 91)                                                   \
        op(KP_PG_UP, 92)                                                \
        op(KP_LEFT, 93)                                                 \
        op(KP_CENTER, 94)                                               \
        op(KP_RIGHT, 95)                                                \
        op(KP_END, 96)                                                  \
        op(KP_DOWN, 97)                                                 \
        op(KP_PG_DOWN, 98)                                              \
        op(KP_INSERT, 99)                                               \
        op(KP_DELETE, 100)                                              \
                                                                        \
        op(PAUSE, 101)                                                  \
                                                                        \
        /* Windows 95 keys found on 104 and 105 key keyboards */        \
        op(LEFT_WIN, 102) /* Not on Sun */                              \
        op(RIGHT_WIN, 103) /* Not on Sun */                             \
        op(LIST_BIT, 104) /* Not on Sun */                              \
                                                                        \
        /* the extra key on 102 and 105 key keyboards,                  \
           compared to 101 104 */                                       \
        op(KEYB, 105) /* Not on Sun */                                  \
                                                                        \
        /* not separate keys on 101+ key keyboards, but handled as such */ \
        op(BREAK, 106) /* Not on Sun */                                 \
        op(SYSREQ, 107) /* Not on Sun */                                \
                                                                        \
        /* the extra keys on sun keyboards */                           \
        op(SUN_STOP, 108)                                               \
        op(SUN_AGAIN, 109)                                              \
        op(SUN_PROPS, 110)                                              \
        op(SUN_UNDO, 111)                                               \
        op(SUN_FRONT, 112)                                              \
        op(SUN_COPY, 113)                                               \
        op(SUN_OPEN, 114)                                               \
        op(SUN_PASTE, 115)                                              \
        op(SUN_FIND, 116)                                               \
        op(SUN_CUT, 117)                                                \
                                                                        \
        op(SUN_HELP, 118)                                               \
                                                                        \
        op(SUN_COMPOSE, 119)                                            \
        op(SUN_META_L, 120)                                             \
        op(SUN_META_R, 121)                                             \
                                                                        \
        op(SUN_POWER, 122)                                              \
        op(SUN_AUDIO_D, 123)                                            \
        op(SUN_AUDIO_U, 124)                                            \
        op(SUN_AUDIO_M, 125)                                            \
        op(SUN_EMPTY, 126)

/* The number of keys in SIM_INTERNAL_KEYS. */
#define SIM_KEY_TALLY(name, number) + 1
#define SK_MAX_KEY (SIM_INTERNAL_KEYS(SIM_KEY_TALLY))

#define SIM_KEY_ENUM(name, number) SK_ ## name = number,
typedef enum {
        SIM_INTERNAL_KEYS(SIM_KEY_ENUM)
} sim_key_t;

#if defined(__cplusplus)
}
#endif

#endif
