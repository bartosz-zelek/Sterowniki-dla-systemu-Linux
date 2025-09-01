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

#ifndef SIMICS_UTIL_VTGETOPT_H
#define SIMICS_UTIL_VTGETOPT_H

#if defined(__cplusplus)
extern "C" {
#endif

enum vtopt_arguments {
        vtopt_no_argument       = 0,
        vtopt_required_argument = 1,
        vtopt_optional_argument = 2
};

struct vtoption {
        const char *name;
        enum vtopt_arguments has_arg;
        int *flag;
        int val;
};

int *get_vtoptind(void);
int *get_vtopterr(void);
int *get_vtoptopt(void);
const char **get_vtoptarg(void);

#define vtoptarg (*get_vtoptarg())
#define vtoptind (*get_vtoptind())
#define vtopterr (*get_vtopterr())
#define vtoptopt (*get_vtoptopt())

int vtgetopt_long(int argc, const char *argv[],
                  const char *optstring,
                  const struct vtoption *longopts, int *longindex);

void vtgetopt_restart(void);

#if defined(__cplusplus)
}
#endif

#endif
