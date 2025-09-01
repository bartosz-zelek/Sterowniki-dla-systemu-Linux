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

#include <stdio.h>
#include <string.h>

static void
cpuid(unsigned op, unsigned *eax, unsigned *ebx, unsigned *ecx, unsigned *edx)
{
        asm ("cpuid"
             : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
             : "0" (op), "c" (0));
}

int
main(void)
{
        unsigned a = 0, b = 0, c = 0, d = 0;
        char vendor[13] = {0};
        cpuid(0, &a, &b, &c, &d);
        if (a < 1) {
                fprintf(stderr, "Host processor not VMP compatible.");
                return 1;
        }
        memcpy(vendor, &b, 4);
        memcpy(vendor + 4, &d, 4);
        memcpy(vendor + 8, &c, 4);
        fprintf(stderr, "Processor vendor id: %s\n", vendor);

        if (strcmp(vendor, "GenuineIntel")) {
                fprintf(stderr, "Host processor not VMP compatible.");
                return 1;
        }

        cpuid(1, &a, &b, &c, &d);
        if (c & (1 << 5)) {
                fprintf(stderr, "Host processor supports Intel(R) VT.\n");
        } else {
                fprintf(stderr, "Host processor does not support Intel(R) VT, check your BIOS settings and also make sure you are not running under a virtualizer that blocks access to Intel(R) VT, such as Hyper-V on Windows.\n");
                return 1;
        }

        cpuid(0x80000000ul, &a, &b, &c, &d);
        if (a < 0x80000001ul) {
                fprintf(stderr, "Host processor not VMP compatible.");
                return 1;
        }

        cpuid(0x80000001ul, &a, &b, &c, &d);
        if (d & (1 << 20)) {
                fprintf(stderr, "Host processor supports execute disable.\n");
        } else {
                fprintf(stderr, "Host processor does not support execute disable, check your BIOS settings.\n");
                return 1;
        }
        fprintf(stderr, "Hardware checks passed, your system will work with VMP.\n");
        return 0;
}
