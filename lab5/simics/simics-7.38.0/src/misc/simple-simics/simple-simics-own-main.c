/*
  simple-simics-own-main.c - Minimal Simics binary, used to test embedding capability.

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
#include <setjmp.h>

#include <unistd.h>

/* pyconfig.h redefines the following symbols */
#undef _POSIX_C_SOURCE
#undef LONG_BIT
#include <Python.h>

#include <simics/simulator-api.h>

#ifdef _WIN32
#include <windows.h>
#include <libloaderapi.h>
typedef jmp_buf sigjmp_buf;
#define sigsetjmp(env, _n) setjmp(env)
#define PYTHON_VER "3.10"
#endif /* _WIN32 */

static void
print_attribute(attr_value_t val)
{
        if (SIM_attr_is_invalid(val)) {
                // be quiet

        } else if (SIM_attr_is_integer(val)) {
                printf("%lld", SIM_attr_integer(val));

        } else if (SIM_attr_is_string(val)) {
                printf("%s", SIM_attr_string(val));

        } else if (SIM_attr_is_object(val)) {
                printf("%s", SIM_object_name(SIM_attr_object(val)));

        } else if (SIM_attr_is_boolean(val)) {
                printf("%s", SIM_attr_boolean(val) ? "True" : "False");

        } else if (SIM_attr_is_floating(val)) {
                printf("%g", SIM_attr_floating(val));

        } else if (SIM_attr_is_nil(val)) {
                printf("NIL");

        } else if (SIM_attr_is_list(val)) {
                printf("[");
                int first = 1;
                for (unsigned i = 0; i < SIM_attr_list_size(val); i++) {
                        if (!first) printf(", ");
                        first = 0;
                        print_attribute(SIM_attr_list_item(val, i));
                }
                printf("]");

        } else if (SIM_attr_is_dict(val)) {
                printf("{");
                int first = 1;
                for (unsigned i = 0; i < SIM_attr_dict_size(val); i++) {
                        if (!first) printf(", ");
                        first = 0;
                        print_attribute(SIM_attr_dict_key(val, i));
                        printf(" : ");
                        print_attribute(SIM_attr_dict_value(val, i));
                }
                printf("}");

        } else if (SIM_attr_is_data(val)) {
                printf("<data attribute>");
        } else {
                printf("<unknown attribute>");
        }
}

static int
initialize_python(char *argv[])
{
        /* Pre-initialize Python to set allocators and UTF-8 mode */
        PyPreConfig preconfig;
        PyPreConfig_InitIsolatedConfig(&preconfig);
        preconfig.utf8_mode = 1;
        Py_PreInitialize(&preconfig);

        /* Set up Python config structure */
        PyStatus status;
        PyConfig config;
        PyConfig_InitIsolatedConfig(&config);
        /* Make sure PYTHONHOME and PYTHONPATH are respected */
        config.isolated = 0;
        config.use_environment = 1;
        status = PyConfig_SetBytesString(&config, &config.program_name,
                                         argv[0]);
        if (PyStatus_Exception(status))
                return -2;

        /* Initialize Python */
        Py_InitializeFromConfig(&config);

        /* Load Simics Python module */
        PyObject *so = PyImport_ImportModule("simics");
        if (!so)
                return -1;
        return 0;
}

int
main(int argc, char *argv[], char *envp[])
{
        int ret = initialize_python(argv);
        if (ret < 0) {
                PyErr_PrintEx(0);
                return ret;
        }

        /* Release Python GIL, allow other threads to access Python. */
        PyEval_SaveThread();

        char line[1024];

        printf("Write some commands:\n");

        while (fgets(line, 1024, stdin)) {
                sigjmp_buf here;

                /* restart here when longjumping out (fatal Simics error) */
                while (sigsetjmp(here, 1) != 0)
                        printf("Returning to main() after a longjump\n");

                /* Tell Simics where to jump to */
                SIM_set_frontend_context(&here);

                char *nl = strrchr(line, '\n');
                if (nl)
                        *nl = 0;

                attr_value_t ret = SIM_run_command(line);
                if (SIM_clear_exception()) {
                        printf("Got exception: %s\n", SIM_last_error());
                } else {
                        printf("Command return value: ");
                        print_attribute(ret);
                        SIM_attr_free(&ret);
                        printf("\n");
                }

                /* Handle background work */
                SIM_process_pending_work();
        }
        return 0;
}
