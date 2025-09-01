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

class QspX86ModeSwitcher:
    def __init__(self, cpu, page_table_base):
        self.cpu = cpu
        self.page_table_base = page_table_base

    def __set_efer(self, v):
        self.cpu.iface.x86_reg_access.set_efer(v)

    def __create_seg_reg(self, sel, d, dpl, g, p, s, seg_type, base, limit,
                         valid, l):
        seg = simics.x86_seg_reg_t()
        seg.valid = valid
        seg.sel = sel
        seg.base = base
        seg.attr = ((seg_type << 0) | (s << 4) | (dpl << 5) | (p << 7) |
                    (l << 13) | (d << 14) | (g << 15))
        seg.limit = limit
        return seg

    def __set_seg(self, reg, val):
        self.cpu.iface.x86_reg_access.set_seg(reg, val)

    def __set_all_data_seg_regs(self, val):
        for seg in (simics.X86_Seg_DS, simics.X86_Seg_ES, simics.X86_Seg_SS,
                    simics.X86_Seg_FS, simics.X86_Seg_GS):
            self.__set_seg(seg, val)

    def __set_cr(self, reg, val):
        self.cpu.iface.x86_reg_access.set_cr(reg, val)

    def __enter_normal_activity_state(self):
        self.cpu.iface.x86_reg_access.set_activity_state(
            simics.X86_Activity_Normal)

    def __enable_dstc(self):
        # enable the D-STC
        if hasattr(self.cpu, "stc_segreg_enabled"):
            self.cpu.stc_segreg_enabled = 0xff

    def __write_mem(self, addr, val, size):
        simics.SIM_write_phys_memory(self.cpu, addr, val, size)


    def __set_common_seg_regs(self):
        data_seg = self.__create_seg_reg(0x10, 1, 0, 1, 1, 1, 3, 0, 0xffffffff,
                                         1, 0)
        self.__set_all_data_seg_regs(data_seg)

        tr = self.__create_seg_reg(0x30, 0, 0, 0, 1, 0, 11, 0, 0xffff, 1, 0)
        self.__set_seg(simics.X86_Seg_TR, tr)
        ldtr = self.__create_seg_reg(0x40, 0, 0, 0, 1, 0, 2, 0, 0xffff, 1, 0)
        self.__set_seg(simics.X86_Seg_LDTR, ldtr)

    def setup_unity_paging_64(self):
        def pb_offs(offs):
            return self.page_table_base + offs

        # set up the PML4
        self.__write_mem(pb_offs(0), pb_offs(0x1027), 8)
        self.__write_mem(pb_offs(0x0ff8), pb_offs(0x7027), 8)

        # set up the PDP
        self.__write_mem(pb_offs(0x1000), pb_offs(0x2027), 8)
        self.__write_mem(pb_offs(0x1008), pb_offs(0x3027), 8)
        self.__write_mem(pb_offs(0x1010), pb_offs(0x4027), 8)
        self.__write_mem(pb_offs(0x1018), pb_offs(0x5027), 8)
        self.__write_mem(pb_offs(0x1020), pb_offs(0x6027), 8)
        # high area
        self.__write_mem(pb_offs(0x7ff0), pb_offs(0x8027), 8)

        # set up the PDE (2MB pages in the range 0x00000000 - 0x40000000)
        for i in range(0, 512):
            self.__write_mem(pb_offs(0x2000) + i * 8,
                             0x00000000 + 0x000000e7 + i * 0x200000, 8)

        # set up the PDE (2MB pages in the range 0x40000000 - 0x80000000)
        for i in range(0, 512):
            self.__write_mem(pb_offs(0x3000) + i * 8,
                             0x40000000 + 0x000000e7 + i * 0x200000, 8)

        # set up the PDE (2MB pages in the range 0x80000000 - 0xc0000000)
        for i in range(0, 512):
            self.__write_mem(pb_offs(0x4000) + i * 8,
                             0x80000000 + 0x000000e7 + i * 0x200000, 8)

        # set up the PDE (2MB pages in the range 0xc0000000 - 0x100000000)
        for i in range(0, 512):
            self.__write_mem(pb_offs(0x5000) + i * 8,
                             0xc0000000 + 0x000000e7 + i * 0x200000, 8)

        # set up the PDE (2MB pages in the range 0x100000000 - 0x140000000)
        for i in range(0, 512):
            self.__write_mem(pb_offs(0x6000) + i * 8,
                             0x100000000 + 0x000000e7 + i * 0x200000, 8)

        # set up the PDE (high area 2MB pages in the range 0x140000000 -
        # 0x180000000)
        for i in range(0, 512):
            self.__write_mem(pb_offs(0x8000) + i * 8,
                             0x140000000 + 0x000000e7 + i * 0x200000, 8)

        self.cpu.cr3 = self.page_table_base

    def enter_protected_mode_64(self):
        self.__enter_normal_activity_state()

        self.__set_efer(0x101)
        self.__set_cr(simics.X86_Cr0, 0x80010031)
        self.__set_cr(simics.X86_Cr4, 0x220)  # Enable PAE and SIMD

        cs = self.__create_seg_reg(0x20, 0, 0, 1, 1, 1, 11, 0, 0xffffffff, 1, 1)
        self.__set_seg(simics.X86_Seg_CS, cs)
        self.__set_common_seg_regs()

        self.__enable_dstc()
        self.setup_unity_paging_64()


    def enter_protected_mode_32(self):
        self.__enter_normal_activity_state()

        # Enter protected mode, paging disabled
        self.__set_efer(0)
        self.__set_cr(simics.X86_Cr0, 0x60010031)
        self.__set_cr(simics.X86_Cr4, 0x200)  # Enable SIMD

        cs = self.__create_seg_reg(0x20, 1, 0, 1, 1, 1, 11, 0, 0xffffffff, 1, 0)
        self.__set_seg(simics.X86_Seg_CS, cs)
        self.__set_common_seg_regs()

        self.__enable_dstc()


class QspX86BareMetalHelpers:
    def __init__(self, cpus, stack_ptr, page_table_base, binary):
        cpu_objs = []
        for c in cpus:
            if isinstance(c, str):
                try:
                    c = simics.SIM_get_object(c)
                except simics.SimExc_General as e:
                    self._warn(f"Could not get object {c}: {e}")
                    continue
            if not (hasattr(c, "iface")
                    and hasattr(c.iface, "processor_info")):
                self._warn(f"Not a cpu object: {c}")
                continue
            cpu_objs.append(c)

        self.cpus = cpu_objs
        self.binary = binary
        self.stack_ptr = stack_ptr
        self.page_table_base = page_table_base

    def _warn(self, msg):
        simics.pr_err(msg)

    def __is_x86s(self, cpu):
        query_iface = getattr(cpu.iface, "x86_cpuid_query")
        if not query_iface:
            return False
        # Check LEGACY_REDUCED_OS_ISA feature bit in CPUID 7.1.ECX[2]
        cpuid = query_iface.cpuid_query(7, 1)
        return (cpuid.c >> 2) & 1 == 1

    def contains_x86s_cpu(self):
        for cpu in self.cpus:
            if self.__is_x86s(cpu):
                return True
        return False

    def mode_from_binary(self):
        if not self.binary:
            self._warn("mode_from_binary with any binary")
            return None

        tcf = simics.SIM_get_debugger()
        (err, file_id) = tcf.iface.debug_symbol_file.open_symbol_file(
            self.binary, 0, False)
        if err != simics.Debugger_No_Error:
            self._warn(f"Failed opening self.binary {self.binary}: {file_id}")
            return None

        (err, data) = tcf.iface.debug_symbol_file.symbol_file_info(file_id)
        tcf.iface.debug_symbol_file.close_symbol_file(file_id)

        if err != simics.Debugger_No_Error:
            self._warn(f"Failed getting file info for {self.binary}: {data}")
            return None

        (_, bin_data) = data
        machine = bin_data.get("machine")
        if not machine:
            self._warn(f"Failed getting self.binary type for {self.binary}")
            return None

        if machine == "X86":
            return 32
        elif machine == "X86-64":
            return 64
        else:
            self._warn(f'Self.Binary type "{machine}" is not for x86')
            return None

    def switch_to_mode(self, mode_bits):
        if mode_bits not in (32, 64):
            self._warn("Only 32 and 64-bit modes can be selected")
            return None
        for cpu in self.cpus:
            mode_switcher = QspX86ModeSwitcher(cpu, self.page_table_base)
            if mode_bits == 32:
                mode_switcher.enter_protected_mode_32()
                if self.stack_ptr >= (1 << 32):
                    self._warn(f"Stack pointer 0x{self.stack_ptr:x} does not"
                               " fit 32-bit ESP")
                cpu.esp = self.stack_ptr & 0xffffffff
            else:
                if self.__is_x86s(cpu):
                    mode_switcher.setup_unity_paging_64()
                else:
                    mode_switcher.enter_protected_mode_64()
                cpu.rsp = self.stack_ptr

    def top_level_object(self):
        first_cpu = self.cpus[0]
        obj = first_cpu
        while simics.SIM_object_parent(obj):
            obj = simics.SIM_object_parent(obj)
        return obj

    def add_framebuffer(self, con_comp_name, address, width, height):
        first_cpu = self.cpus[0]
        mem_space = first_cpu.shared_physical_memory
        system_name = self.top_level_object().name
        con = simics.SIM_get_object(con_comp_name).con

        # Prevent accel-vga device from pointing at the same console as that
        # will attempt to update resolution as well.
        for vga in simics.SIM_object_iterator_for_class('accel_vga_v2'):
            vga.console = None

        depth = 32
        size_bytes = width * height * depth

        fb_ns = simics.SIM_create_object(
            "namespace", f"{system_name}.framebuffer").name
        fb_img = simics.SIM_create_object(
            "ram", f"{fb_ns}.buffer", image=None,
            self_allocated_image_size=size_bytes)
        fb_mem = simics.SIM_create_object("memory-space", f"{fb_ns}.mem_space",
                                          map=[[0, fb_img, 0, 0, size_bytes,
                                                None, 0, 0]])
        mem_space.cli_cmds.add_map(base=address,
                                   length=size_bytes,
                                   priority=-10,
                                   device=fb_mem)
        simics.SIM_create_object("framebuffer", f"{fb_ns}.obj",
                                 width=width, height=height,
                                 depth=depth,
                                 scan_bytes=width * depth // 8,
                                 console=con, memory_space=fb_mem,
                                 refresh_rate=60)
