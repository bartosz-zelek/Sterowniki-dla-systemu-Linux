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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * A full definition of the magic instructions is available
 * in simics/magic-instruction.h, which can be found in the
 * [simics]/src/include/ tree. Use -I flag to gcc.
 */
#include <simics/magic-instruction.h>

/* Struct describing a person and his/her occupation */
struct person {
        char info[20];
        char *type;
};

/* Reads a line from a file, and fill in the struct */
int read_developer(struct person *p, FILE *f)
{
        char line[100];

        if (fgets(line, 100, f) == NULL)
                return 0;       /* end of file */

        /* Type is always developer */
        p->type = "developer";
        strcpy(p->info, line);
        return 1;
}

int main(int argc, char **argv)
{
        const char *filename = "/etc/passwd";
        FILE *f;
        struct person user;

        MAGIC_BREAKPOINT;

        /* Open file */
        printf("Reading %s...\n", filename);
        f = fopen(filename, "r");

        /* Read and print all users found in the file */
        while (read_developer(&user, f)) {
                printf("Type: %s\n", user.type);
                printf("Info: %s, ", user.info);
        }

        printf("Done.\n");
        return 0;
}
