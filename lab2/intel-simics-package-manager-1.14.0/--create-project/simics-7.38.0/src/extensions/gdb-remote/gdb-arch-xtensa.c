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

#include "gdb-remote.h"
#include <simics/model-iface/describe-registers.h>

/* We support the xt-gdb here, which is a hacked gdb which supports loading
   core-specific details via dlopen() containing the register setup and
   disassembly function needed for the specific core.

   For our Simics native Xtensa cores, the we extract the known registers
   from the src/gdb/xtensa-config.c and presents parts of this information
   in the xtensa_gdb_register attribute.

   xt-gdb uses hardware register numbers to address the registers,
   which means we must map these numbers to the register index,
   done by the .reg_mapper function. (Uses a hash-table to do so)

   Some Xtensa specific remote protocols, not currently supported:
   Qxtsis=<instruction cache line width> 
      Set processor instruction cache line width (in bytes) 

   Qxtsds=<writeback data cache line width> 
      Set processor data cache line width, only if writeback 

   Qxtgr=<device number>:<register name>:<value>  "OK", "E:<xx>" 
      Write generic JTAG register. 

   Qxtexe=<len>:<opcode> 
      Execute xtensa instructions. 

   qxtn   "XMON", "XMON2.0", "XOCD", "ISS3", "KGDB", "TRACE", "REDBOOT" 
      Query target type. 

   qxtgr<r-device number>,<register name> 
      Read generic JTAG register 

   qxtversion    (ISS)  <tool version>;<configuration>;<tdk> 
      Ask ISS for its version. 

   qxtocdversion  (XOCD)  "6000" 
      Ask XOCD for its version.
*/


/* Additional data we store from gdb->arch_data */
typedef struct {
        /* target reg num -> reg_desc_vect_t index */
        ht_int_table_t hw_to_idx;          
} xtensa_arch_data_t;


/* Fill in the register_description array with information of the registers
   that we fetch from the xtensa_gdb_registers attribute. Also create a
   hash-table so we can quickly find the register based on the hardware
   register number. */
static bool
xtensa_init(gdb_remote_t *gdb, conf_object_t *cpu)
{
        xtensa_arch_data_t *d = MM_ZALLOC(1, xtensa_arch_data_t);
        ht_init_int_table(&d->hw_to_idx);
        gdb->arch_data = d;
        
        attr_value_t attr;
        if (!read_opt_attr(to_obj(gdb), cpu, "xtensa_gdb_registers", &attr)) {
                return false;
        }

        if (!SIM_C_GET_INTERFACE(cpu, describe_registers)) {
                SIM_LOG_ERROR(to_obj(gdb), 0,
                              "cannot find %s interface in CPU %s",
                              DESCRIBE_REGISTERS_INTERFACE,
                              SIM_object_name(cpu));
                return false;
        }

        for (int i = 0; i < SIM_attr_list_size(attr); i++) {
                attr_value_t reg = SIM_attr_list_item(attr, i);
                const char *name = SIM_attr_string(SIM_attr_list_item(reg, 0));
                int size = SIM_attr_integer(SIM_attr_list_item(reg, 1));
                int target_reg_num = SIM_attr_integer(
                        SIM_attr_list_item(reg, 2));
                const description_t *desc = find_reg_desc_by_dwarf_id(
                        cpu, target_reg_num);
                register_description_t rd;
                if (desc) {
                        rd = desc_reg_to_register_description(desc, name,
                                                              size * 8);
                        free_reg_desc(cpu, desc);
                } else {
                        SIM_LOG_WARNING(
                                to_obj(gdb), 0,
                                "Register %s missing in %s interface of %s",
                                name, DESCRIBE_REGISTERS_INTERFACE,
                                SIM_object_name(cpu));
                        rd.regnum = 0;
                        rd.read = reg_read_zero;
                        rd.write = reg_write_ignore;
                }
                int *vect_idx = MM_MALLOC(1, int);
                *vect_idx = VLEN(gdb->register_descriptions);
                ht_insert_int(&d->hw_to_idx, target_reg_num, vect_idx);

                VADD(gdb->register_descriptions, rd);
        }
        SIM_attr_free(&attr);
        return true;
}

/* Called when reading/writing registers, xt-gdb uses a target-register
   number to address the certain register. */
static register_description_t *
xtensa_reg_mapper(gdb_remote_t *gdb, int hw_num)
{
        xtensa_arch_data_t *d = gdb->arch_data;
        int *vect_idx = ht_lookup_int(&d->hw_to_idx, hw_num);
        if (!vect_idx) {
                return NULL;
        }
        return &VGET(gdb->register_descriptions, *vect_idx);
}

const gdb_arch_t gdb_arch_xtensa = {
        .name = "xtensa",
        .arch_name = "xtensa",
        .help = {
                .target_flag = "xtensa",
                .prompt_cmd = "set architecture xtensa"
        },
        .is_be = false,
        .bit_extend = false,

        /* Functions */
        .init = xtensa_init,
        .reg_mapper = xtensa_reg_mapper,

        .regs = {NULL},             /* Each core has a unique register set */
        .nonsteppable_watchpoint = true,
};
