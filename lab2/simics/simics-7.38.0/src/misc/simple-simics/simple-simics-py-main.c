/*
  simple-simics-py-main.c - Minimal Simics binary, used to test embedding capability.

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
#include <string.h>

/* pyconfig.h redefines the following symbols */
#undef _POSIX_C_SOURCE
#undef LONG_BIT
#include <Python.h>

#include <simics/simulator-api.h>

#ifdef _WIN32
#include <windows.h>
#include <libloaderapi.h>
#define PYTHON_VER "3.10"
#endif /* _WIN32 */

/* Construct a Python command line that runs the Simics Python module */
static char **
construct_python_args(int *argc, const char *exe, const char *mod)
{
        *argc = 4;
        char **argv = malloc((size_t)*argc * sizeof(char *));
        argv[0] = strdup(exe);
        argv[1] = strdup("-X");
        argv[2] = strdup("utf8");
        argv[3] = strdup(mod);
        return argv;
}

int
main(int argc, char *argv[], char *envp[])
{
        int py_argc;
        char **py_argv = construct_python_args(&py_argc, argv[0], argv[1]);
        return Py_BytesMain(py_argc, py_argv);
}
