# © 2024 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import simics
from comp import (SerialDownConnector, SimpleConfigAttribute, ConfigAttribute,
                  StandardConnectorComponent)

KB = 0x400
MB = KB * KB
GB = MB * KB
TB = GB * KB

counter_map_length     = 0x20
counter_extras_length  = 0x30   # 6 x 8 byte registers

class internals_training_unit_comp(StandardConnectorComponent):
    """A RIBIT (RISC-V*-Based Internals Training) system single-core unit subsystem"""
    _class_desc = "a RIBIT (RISC-V*-Based Internals Training) unit"
    _do_not_init = object()
    
    ## "Copy" constants for mapping lengths from global scope into the class
    counter_map_length = counter_map_length
    counter_extras_length = counter_extras_length

    class cpu_list(StandardConnectorComponent.cpu_list):
        def getter(self):
            return [self._up.get_slot('hart')]

    class top_level(StandardConnectorComponent.top_level):
        def _initialize(self):
            self.val = True

    class basename(StandardConnectorComponent.basename):
        val = 'unit'

    class processor_id(SimpleConfigAttribute(0, 'i')):
        """Processor ID that each transaction from the core will carry."""

    class freq_mhz(SimpleConfigAttribute(100.0, 'f')):
        """Frequency in MHz of each core, default is 100 MHz"""

    class timebase_freq_mhz(SimpleConfigAttribute(1.0, 'f')):
        """Frequency in MHz of the timebase, i.e. 'mtime', default is 1MHz"""

    class memory_megs(ConfigAttribute):
        """Size in MB of RAM memory, default is 2048. Minimum is 256."""
        attrtype = "i"
        def _initialize(self):
            self.val = 2048
        def getter(self):
            return self.val
        def setter(self, val):
            if val < 240:
                return simics.Sim_Set_Illegal_Value
            self.val = val

    def setup(self):
        StandardConnectorComponent.setup(self)
        if not self.instantiated.val:
            self.add_objects()
        self.add_connectors()

    def add_objects(self):
        core_mem = self.add_pre_obj('core_mem', 'memory-space')

        clint = self.add_pre_obj('clint', 'riscv-clint',
                                 freq_mhz=self.timebase_freq_mhz.val)

        hart = self.add_pre_obj('hart', 'riscv-rv64',
                                freq_mhz=self.freq_mhz.val,
                                physical_memory=core_mem,
                                clint=clint,
                                mhartid=0)

        phys_mem = self.add_pre_obj('phys_mem', 'memory-space')
        txn_decorator = self.add_pre_obj('txn_decorator', 'i_processor_id_decorator',
                                    memory_space = phys_mem,
                                    processor_id = self.processor_id.val)

        core_mem.default_target = [txn_decorator, 0, 0, None]

        clint.hart = [hart]

        plic = self.add_pre_obj('plic', 'riscv-plic')
        plic.max_interrupt = 127
        plic.max_priority = 7
        plic.hart = [hart]

        boot_rom = self.add_pre_obj('boot_rom', 'rom', image = None, self_allocated_image_size = 0x1000000)
        low_ram  = self.add_pre_obj('low_ram', 'ram', image = None, self_allocated_image_size = 0x10000000-0x1000000)

        if self.memory_megs.val > 240:
            high_ram = self.add_pre_obj('high_ram', 'ram', image = None,
                                        self_allocated_image_size = self.memory_megs.val * MB - 0x10000000 - 0x1000000)

        uart = self.add_pre_obj('uart', 'NS16550')
        uart.irq_dev = plic.port.IRQ[1]
        uart.xmit_time = 0

        counter = self.add_pre_obj('counter', 'i_counter')
        mailbox   = self.add_pre_obj('mailbox', 'i_mailbox', phys_mem = phys_mem, irq = plic.port.IRQ[2])
        sync_e2l  = self.add_pre_obj('sync_e2l', 'i_synchronizer_e2l', level_out = plic.port.IRQ[3])

        rom_size = boot_rom.attr.self_allocated_image_size
        c_extra_map = 0x10001000 + self.counter_map_length
        phys_mem.map = [
            [0x00000000,  boot_rom,             0, 0,                               rom_size],
            [rom_size,    low_ram,              0, 0, low_ram.attr.self_allocated_image_size],
            [0x10000000,  uart.bank.regs,       0, 0,                                   0x11],
            [0x10001000,  counter.bank.regs,    0, 0,                self.counter_map_length],
            [c_extra_map, counter.bank.extras,  0, 0,             self.counter_extras_length],
            [0x10002000,  mailbox.bank.c_regs,  0, 0,                                   0x10],
            [0x10002010,  mailbox.far_end,      0, 0,                                   0x08],
            [0x10003000,  sync_e2l.bank.regs,   0, 0,                                   0x04],
            [0x10090000,  clint.bank.regs,      0, 0,                                 0xc000],
            [0x1c000000,  plic.bank.regs,       0, 0,                              0x4000000]
        ]
        
        if self.memory_megs.val > 240:
            phys_mem.map += [
                [0x80000000, high_ram, 0, 0, high_ram.attr.self_allocated_image_size]
            ]

    def add_connectors(self):
        self.add_connector('serial', SerialDownConnector('uart'))


class internals_training_system_comp(StandardConnectorComponent):
    """RIBIT (RISC-V*-Based Internals Training) system, with multiple RIBIT-unit subsystems"""
    _class_desc = "a RIBIT (RISC-V*-Based Internals Training) system"
    _do_not_init = object()

    ## Global constants
    shared_out_map_length = 0x10
    sync_map_length       = 0x10
    counter_map_length    = counter_map_length
    counter_extras_length = counter_extras_length    
    total_shared_devices_map_length = shared_out_map_length + sync_map_length + counter_map_length + counter_extras_length
    global_ram_size = 0x5_0000_0000

    # Names used in more than one place = constant
    name_of_sync_irq_bus = "sync_irq_bus"
    name_of_shared_mem_map_with_id = "shared_mem_with_id"
    name_of_id_router = "id_router"

    # NOTE: The expectation is that subsystem[x] slots are added from the yml.include file

    class cpu_list(StandardConnectorComponent.cpu_list):
        def getter(self):
            rv = []
            for subsys in self._up.get_slot(self._up.unit_name.val):
                rv += subsys.cpu_list
            return rv

    class top_level(StandardConnectorComponent.top_level):
        def _initialize(self):
            self.val = True


    # Configuration attributes for colors with good defaults
    #   0, 226 = black on light yellow
    #   0, 219 = black on light violet
    #   0, 119 = black on light green 
    #   0, 246 = black on grey
    #   0,  81 = black on light blue
    #  15,   4 = white on blue
    #  15, 196 = white on mid-red 
    #  15,  76 = white on green
    class shared_console_fg_colors(SimpleConfigAttribute([0,0,0,0,0,15,15,15], '[iiiiiiii]')):
        """Foreground colors for the shared console (ANSI VT100 color codes)."""

    class shared_console_bg_colors(SimpleConfigAttribute([226,219,119,246,81,4,196,28], '[iiiiiiii]')):
        """Background colors for the shared console (ANSI VT100 color codes)."""

    # Note on this configuration parameter: 
    #   Since we decided not to use a connector between the system and subsystems, 
    #   the system-level component has to find the subsystem units by their name.
    #   That name is set by the setup scripts creating the system and unit
    #   components separately.  This configuration parameter lets the setup
    #   script provide the name to the system component. 
    class unit_name(SimpleConfigAttribute("unit", 's')):
        """The name assigned by the setup script to the units/subsystems."""

    # Local clock for the global system 
    class use_clock(SimpleConfigAttribute(True, 'b')):
        """If true, the shared devices will use a clock separate from the
           processor cores in the subsystems. Defaults to true."""
        
    class freq_mhz(SimpleConfigAttribute(100.0, 'f')):
        """Frequency in MHz for the clock in the shared system, default is 100 MHz"""

    # Setup code
    def setup(self):
        StandardConnectorComponent.setup(self)
        if not self.instantiated.val:
            self.add_objects()
        self.add_connectors()

    def add_objects(self):

        # Thanks to the magic of the component system, this clock will be used as
        # the queue for all the objects in the shared system 
        if self.use_clock.val==True:
            self.add_pre_obj('clock', 'clock', freq_mhz = self.freq_mhz.val)

        # Devices in the shared system
        shared_out = self.add_pre_obj('shared_out', 'i_multi_writer_output',
                                      fg_colors = self.shared_console_fg_colors.val,
                                      bg_colors = self.shared_console_bg_colors.val)
        non_id_sink = self.add_pre_obj('non_id_sink', 'set-memory', log_level=4)
        global_ram = self.add_pre_obj('global_ram', 'ram', image = None, self_allocated_image_size = self.global_ram_size)

        sync_irq_bus = self.add_pre_obj(self.name_of_sync_irq_bus, 'signal-bus')
        sync = self.add_pre_obj('sync', 'i_synchronizer', irq = sync_irq_bus)
        
        counter = self.add_pre_obj('counter', 'i_counter')

        # Shared memory 
        #   Mapping devices at 0x3000_0000 and on
        #   Packing the devices into memory 
        shared_mem_with_id = self.add_pre_obj(self.name_of_shared_mem_map_with_id, 'memory-space')
        # Adressess
        shared_out_addr =  0x3000_0000
        sync_addr = shared_out_addr + self.shared_out_map_length
        counter_addr = sync_addr + self.sync_map_length
        extras_addr = counter_addr + self.counter_map_length
        # Set up the memory map
        shared_mem_with_id.map = [
            [shared_out_addr, shared_out.bank.regs, 0, 0, self.shared_out_map_length],
            [sync_addr,             sync.bank.regs, 0, 0, self.sync_map_length],
            [counter_addr,       counter.bank.regs, 0, 0, self.counter_map_length],
            [extras_addr,      counter.bank.extras, 0, 0, self.counter_extras_length],
            [0x1_0000_0000, global_ram, 0, 0, self.global_ram_size],
        ]
        shared_mem_without_id = self.add_pre_obj('shared_mem_without_id', 'memory-space')
        shared_mem_without_id.default_target = [non_id_sink, 0, 0, None]
        
        self.add_pre_obj(self.name_of_id_router, 'i_processor_id_router',
                                    memory_space_with_id = shared_mem_with_id,
                                    memory_space_without_id = shared_mem_without_id)
        
    def add_connectors(self):
        self.add_connector('shared_console', SerialDownConnector('shared_out'))

    def post_instantiate(self):
        id_router = self.get_slot(self.name_of_id_router)
        sub_sys_name = self.unit_name.val
        num_sub_sys = len(self.get_slot(sub_sys_name))
        for i in range(num_sub_sys):
            mailbox_base = self.get_slot(f'{sub_sys_name}[{i}].mailbox').shared_mem_mailbox_base_attr  # same for all mailboxes
            self.get_slot(f'{sub_sys_name}[{i}].phys_mem').map += [
                [0x30000000, id_router, 0, 0x30000000, self.total_shared_devices_map_length],
                [mailbox_base, id_router, 0, mailbox_base, num_sub_sys * 8],
                [0x100000000, id_router, 0, 0x100000000, self.global_ram_size]
            ]
            self.get_slot(self.name_of_shared_mem_map_with_id).map += [[mailbox_base + i*0x8, self.get_slot(f'{sub_sys_name}[{i}].mailbox').bank.f_regs, 0, 0, 0x8]]
            self.get_slot(self.name_of_sync_irq_bus).targets += [self.get_slot(f'{sub_sys_name}[{i}].sync_e2l').port.edge_in]
        ## Tell the global sync device about the total number of subsystems
        ## and reset it so that the sync "decrementer" register starts in a good state
        sync=self.get_slot('sync')
        sync.attr.num_sub_systems = num_sub_sys
        sync.port.reset.iface.signal.signal_raise()
        sync.port.reset.iface.signal.signal_lower()        

    class component(StandardConnectorComponent.component):
        def post_instantiate(self):
            self._up.post_instantiate()

