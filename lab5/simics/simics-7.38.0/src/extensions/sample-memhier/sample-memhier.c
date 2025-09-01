/*
  sample-memhier.c - example code for a memory hierarchy

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

/*
   The sample-memhier module provides sample code demonstrating how to
   implement a customized memory-hierarchy. Memory hierarchies allows the user
   to catch memory accesses for tracing, cache models, profiling, etc.

   Use the module by adding an object sample_memhier0 to a configuration file:

[...]

OBJECT sample_memhier0 TYPE sample-memhier {
}

OBJECT phys_mem0 TYPE memory-space {
       timing_model: sample_memhier0
       [...]
}

[...]

   This will cause all memory accesses be printed on the terminal (this
   is more or less identical to what the trace module does). It should
   look something like this:

simics> run 10
  data: 0x00000000ffd84e90 0x000001fff8084e90 Write  8 958
  data: 0x00000000ffd84e98 0x000001fff8084e98 Read   8 959
  data: 0x00000000ffda94c9 0x000001fff80a94c9 Write  1 960
  data: 0x00000000ffd84e98 0x000001fff8084e98 Write  8 961
  data: 0x00000000ffd84e90 0x000001fff8084e90 Read   8 962
  data: 0x00000000ffd84e90 0x000001fff8084e90 Write  8 963
  data: 0x00000000ffd84e98 0x000001fff8084e98 Read   8 964
simics>
*/

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <simics/simulator-api.h>
#include <simics/util/os.h>

#include <simics/model-iface/timing-model.h>

/* struct for the sample-memhier-class */
typedef struct {
        conf_object_t *obj;
        char *trace_file_name;
        FILE *trace_file;
        uint64 count;
} sample_memhier_object_t;

static inline sample_memhier_object_t *
sample_memhier_from_conf_obj(conf_object_t *obj)
{
        return SIM_object_data(obj);
}

static set_error_t
set_trace_file(sample_memhier_object_t *memhier, const char *file)
{
        FILE *f = NULL;

        if (!file[0])
                file = NULL;
        else if (strcmp(file, "stdout") == 0)
                f = stdout;
        else if (strcmp(file, "stderr") == 0)
                f = stderr;
        else {
                f = os_fopen(file, "a+");
                if (f == NULL) {
                        const char *errstr = strerror(errno);
                        char *strbuf = malloc(60 + strlen(file)
                                              + strlen(errstr));
                        sprintf(strbuf, "Opening trace file %s: %s",
                                file, errstr);
                        SIM_attribute_error(strbuf);
                        free(strbuf);
                        return Sim_Set_Illegal_Value;
                }
        }

        /* close previous trace file, if any */
        if (memhier->trace_file
            && memhier->trace_file != stdout
            && memhier->trace_file != stderr) {
                fclose(memhier->trace_file);
        }

        memhier->trace_file = f;
        MM_FREE(memhier->trace_file_name);
        memhier->trace_file_name = file ? MM_STRDUP(file) : NULL;

        if (file)
                SIM_printf("Trace output now goes to %s.\n", file);
        else
                SIM_printf("Trace output disabled.\n");

        return Sim_Set_Ok;
}

static void *
sample_memhier_init(conf_object_t *obj, void *data)
{
        sample_memhier_object_t *memhier =
                MM_ZALLOC(1, sample_memhier_object_t);

        memhier->obj = obj;
        memhier->count = 0;
        memhier->trace_file = NULL;
        memhier->trace_file_name = NULL;

        /* default trace to standard output */
        set_trace_file(memhier, "stdout");
        return memhier;
}

/*
 * The core of the memory hierarchy. This function is called once for
 * every memory operation.
 */
static cycles_t
sample_memhier_operate(conf_object_t *obj, conf_object_t *space,
                       map_list_t *map, generic_transaction_t *mem_op)
{
        const char *op_type, *op_dir;

        sample_memhier_object_t *mem_hier = sample_memhier_from_conf_obj(obj);
        char source[64];
        source[0] = 0;

        /*
         * We want to see future references, so make sure the STC does
         * not hide them from us.
         */
        SIM_mem_op_ensure_future_visibility(mem_op);

        /*
         * If tracing is disabled, don't do anything except increase the
         * counter.
         */
        if (!mem_hier->trace_file) {
                mem_hier->count++;
                return 0;
        }

        /* memory operations are either data or instructions */
        op_type = SIM_mem_op_is_data(mem_op) ? "data" : "instr";
        op_dir = SIM_mem_op_is_read(mem_op) ? "read" : "write";

        /*
         * Print a line in the trace file. The line will look something like:
         *
         * instr: 0x000001fff8008314 0x000001ccf8008314 Ld    4 21
         *
         * Meaning that the 21:nd memory operation was a 4-byte load
         * of an instruction at logical address 0x000001fff8008314
         * which mapped to physical address 0x000001ccf8008314.
         */
        fprintf(mem_hier->trace_file,
                "%6s: 0x%llx 0x%llx %-5s %2d %lld %s\n",
                op_type,
                SIM_get_mem_op_virtual_address(mem_op),
                SIM_get_mem_op_physical_address(mem_op),
                op_dir,
                SIM_get_mem_op_size(mem_op),
                mem_hier->count,
                source);

        /*
         * The return value is the number of instructions (sort of
         * cycles) to stall the processor. Note that it is possible to
         * stall it a huge number of instructions and then "reset" the
         * stall time when you know the true delay, but we haven't
         * hacked the export for that yet.
         */
        mem_hier->count++;
        return 0;
}

static set_error_t
set_trace_file_attribute(void *dont_care, conf_object_t *obj,
                         attr_value_t *val, attr_value_t *idx)
{
        sample_memhier_object_t *memhier = sample_memhier_from_conf_obj(obj);
        return set_trace_file(memhier, SIM_attr_string(*val));
}

static attr_value_t
get_trace_file_attribute(void *dont_care, conf_object_t *obj,
                         attr_value_t *idx)
{
        sample_memhier_object_t *memhier = sample_memhier_from_conf_obj(obj);
        return SIM_make_attr_string(memhier->trace_file_name);
}

static set_error_t
set_count_attribute(void *dont_care, conf_object_t *obj, attr_value_t *val,
                    attr_value_t *idx)
{
        sample_memhier_object_t *memhier = sample_memhier_from_conf_obj(obj);
        memhier->count = SIM_attr_integer(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_count_attribute(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{
        sample_memhier_object_t *memhier = sample_memhier_from_conf_obj(obj);
        return SIM_make_attr_uint64(memhier->count);
}

void
init_local()
{
        /* initialize and register the class "sample-memhier-class" */
        const class_data_t class_data = {
                .init_object = sample_memhier_init,
                .class_desc = "sample memory hierarchy",
                .description =
                "The sample-memhier class is an example of how to write"
                " memory hierarchies for Simics. It will print memory usage"
                " traces to a log file when its trace-file attribute is"
                " assigned a file name."
        };

        conf_class_t *conf_class = SIM_register_class("sample-memhier",
                                                      &class_data);

        /* initialize and register the timing-model interface */
        static const timing_model_interface_t timing_iface = {
                .operate = sample_memhier_operate
        };
        SIM_register_interface(conf_class, "timing_model", &timing_iface);

        /* initialize attributes */
        SIM_register_typed_attribute(conf_class, "trace_file",
                                     get_trace_file_attribute, NULL,
                                     set_trace_file_attribute, NULL,
                                     Sim_Attr_Optional,
                                     "s", NULL,
                                     "Name of output file the trace is"
                                     " written to.");

        SIM_register_typed_attribute(conf_class, "count",
                                     get_count_attribute, NULL,
                                     set_count_attribute, NULL,
                                     Sim_Attr_Optional,
                                     "i", NULL,
                                     "Number of memory operations traced"
                                     " so far.");
}
