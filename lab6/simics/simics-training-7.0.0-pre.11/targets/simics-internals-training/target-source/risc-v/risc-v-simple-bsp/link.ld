/*
  © 2023 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/


OUTPUT_ARCH("riscv")

ENTRY(_very_early_init)

MEMORY
{
 CODE_ROM : ORIGIN = 0x00000000, LENGTH = 0x1000000
 DATA_RAM : ORIGIN = 0x01000000, LENGTH = 0xF000000
 STACK_HEAP_RAM : ORIGIN = 0x80000000, LENGTH = 0x10000000
}

SECTIONS
{
   .text : { *(.text) } > CODE_ROM
   .rodata : { . = ALIGN(0x1000); *(.rodata) } > CODE_ROM
   _sidata = LOADADDR(.data);

   . = ALIGN(4);
   .data :
   {
     __DATA_BEGIN__ = .;
      _data_start = .;
     *(.data)
     *(.data.*)
     *(.eh_frame)
     *(.eh_frame.*)
     *(.fini_array)
     *(.fini_array.*)
     *(.init_array)
     *(.init_array.*)
     . = ALIGN(4);
     _data_end = .;
   } > DATA_RAM AT>CODE_ROM

   .sdata          :
   {
     __SDATA_BEGIN__ = .;
     *(.srodata.cst16) *(.srodata.cst8) *(.srodata.cst4) *(.srodata.cst2) *(.srodata .srodata.*)
     *(.sdata .sdata.* .gnu.linkonce.s.*)
   } > DATA_RAM AT>CODE_ROM
   _data_size = . - _data_start;

   .bss : {
     . = ALIGN(0x1000);
     __bss_start = .;
     *(.bss)
     *(.bss.*)
     *(.sbss)
     *(.sbss.*)
     _end = .;
    } > DATA_RAM AT> DATA_RAM


    __global_pointer$ = MIN(__SDATA_BEGIN__ + 0x800,
              MAX(__DATA_BEGIN__ + 0x800, _end - 0x800));

   __stack_top = ORIGIN(STACK_HEAP_RAM) + LENGTH(STACK_HEAP_RAM) - 0x10;
   __uart0_base = 0x10000000;
   __local_counter_base = 0x10001000;
   __mailbox_base = 0x10002000;
   __local_sync_base = 0x10003000;
   __clint_base = 0x10090000;
   __plic_base = 0x1c000000;
   __shared_con_base = 0x30000000;
   __global_sync_base = 0x30000010;
   __global_counter_base = 0x30000020;
   __heap_base = ORIGIN(STACK_HEAP_RAM);
}
