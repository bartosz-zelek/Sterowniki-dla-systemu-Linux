# © 2019 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


from cli import (
    arg,
    flag_t,
    str_t,
    new_command,
    new_info_command,
    new_status_command,
    add_tech_preview,
    tech_preview_exists,
    new_tech_preview_command,
    command_verbose_return
)
from simics import *
import sim_commands
from table import *
import importlib
from cli import CliError

def mask(w):
    return (1 << w) - 1

def bit(val,o):
    return (val >> o) & 1

def bits(val,w,o):
    return (val >> o) & mask(w)

def isamask(ch):
    return 1 << (ord(ch) - ord('A'))

def str_ext_state(state):
    return ('off', 'initial', 'clean', 'dirty')[state]


class status_str:
    def __init__(self):
        self.s = ''

    def txt(self):
        return self.s

    def add(self, txt):
        if len(self.s):
            self.s += ', '
        self.s += txt

    def flag(self, txt, value):
        if value:
            self.add(txt)

def str_mstatus(cpu):

    status = cpu.mstatus

    s = status_str()

    s.flag("MPV",  bit(status, 39))
    s.flag("GVA",  bit(status, 38))

    s.flag("TSR",  bit(status, 22))
    s.flag("TW",   bit(status, 21))
    s.flag("TVM",  bit(status, 20))
    s.flag("MXR",  bit(status, 19))
    s.flag("SUM",  bit(status, 18))
    s.flag("MPRV", bit(status, 17))

#    str += f"{comma(str)}XS: {str_ext_state(bits(status, 2, 15))}"
    if hasattr(cpu, "FLEN"):
        if cpu.FLEN:
            s.add(f'FS: {str_ext_state(bits(status, 2, 13))}')

    s.add(f'MPP: {str_cpu_mode(bits(status, 2, 11))}')
    s.add(f'SPP: {str_cpu_mode(bit(status, 8))}')

#    s.add(f'VS: {str_ext_state(bits(status, 2, 9))}')

    s.flag("MPIE", bit(status, 7))
    s.flag("SPIE", bit(status, 5))
    s.flag("MIE",  bit(status, 3))
    s.flag("SIE",  bit(status, 1))

    return s.txt()


def str_hstatus(cpu):
    status = cpu.hstatus

    s = status_str()

    s.flag("VTSR",  bit(status, 22))
    s.flag("VTW",   bit(status, 21))
    s.flag("VTVM",  bit(status, 20))

    vgein = bits(status, 6, 12)
    if vgein:
        s.add(f'VGEIN: {vgein}')

    s.flag("HU",   bit(status, 9))
    s.flag("SPVP", bit(status, 8))
    s.flag("SPV",  bit(status, 7))
    s.flag("GVA",  bit(status, 6))
#    s.flag("VSBE", bit(status, 5))

    return s.txt()


def str_vsstatus(cpu):
    status = cpu.vsstatus

    s = status_str()

    s.flag("MXR",  bit(status, 19))
    s.flag("SUM",  bit(status, 18))

#    s.add(f'XS: {str_ext_state(bits(status, 2, 15))}')
    if hasattr(cpu, 'FLEN'):
        if cpu.FLEN:
            s.add(f'FS: {str_ext_state(bits(status, 2, 13))}')

#    s.add(f'VS: {str_ext_state(bits(status, 2, 9))}')

    s.add(f'SPP: {str_cpu_mode(bit(status, 8))}')

#    s.flag("UBE", bit(status, 6))
    s.flag("SPIE", bit(status, 5))
    s.flag("SIE",  bit(status, 1))

    return s.txt()

def str_bitvector(bits):
    s = status_str()
    for i in range(64):
        if (1 << i) & bits:
            s.add(f'{i}')
    return s.txt()

def remove_regs(cpu, regs, rm_regs):
    for r in rm_regs:
        try:
            regs.remove(cpu.iface.int_register.get_number(r))
        except:
            pass
    return regs

abi_regs = ("zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
            "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7",
            "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11",
            "t3", "t4", "t5", "t6")

core_regs = ("x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7",
             "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15",
             "x16", "x17", "x18", "x19", "x20", "x21", "x22", "x23",
             "x24", "x25", "x26", "x27", "x28", "x29", "x30", "x31")

fp_abi_regs = ("ft0", "ft1", "ft2", "ft3", "ft4", "ft5", "ft6", "ft7",
               "fs0", "fs1", "fa0", "fa1", "fa2", "fa3", "fa4", "fa5",
               "fa6", "fa7", "fs2", "fs3", "fs4", "fs5", "fs6", "fs7",
               "fs8", "fs9", "fs10", "fs11", "ft8", "ft9", "ft10", "ft11")

fp_regs = ("f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",
           "f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15",
           "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23",
           "f24", "f25", "f26", "f27", "f28", "f29", "f30", "f31")


def abi_reg(nr):
    return abi_regs[nr]

def fp_abi_reg(nr):
    return fp_abi_regs[nr]

def fmtval(val, is64):
    if is64:
        return "0x%016x" % val
    else:
        return "0x%08x" % val

def local_csrs(cpu, is64):
    all_csrs = cpu.iface.int_register.all_registers()

    # Remove all cpu_registers
    all_csrs = remove_regs(cpu, all_csrs, abi_regs)
    all_csrs = remove_regs(cpu, all_csrs, core_regs)
    all_csrs = remove_regs(cpu, all_csrs, fp_regs)
    all_csrs = remove_regs(cpu, all_csrs, ["pc"])

    # TBD: Divide in Machine, Supervisor and User csrs

    csrs = ["%-15s = %s" % (cpu.iface.int_register.get_name(csr),
                            fmtval(cpu.iface.int_register.read(csr), is64))
            for csr in all_csrs]

    return "\n" + "\n".join(csrs)

def local_status_regs(cpu, hasFp):
    status = "\nmstatus: 0x%x\n    %s\n" % (cpu.mstatus, str_mstatus(cpu))
#   TBD: Add fp status
#    if hasFp:
#        status += ""
    return status

def local_pregs(cpu, all):
    is64 = 1 if cpu.XLEN == 64 else 0
    hasExtF = bool(cpu.misa & isamask('F'))
    hasExtD = bool(cpu.misa & isamask('D'))

    regs = ["x%-2d %5s = %s" % (reg, abi_reg(reg), fmtval(cpu.gprs[reg],is64))
            for reg in range(32)]

    if hasExtF or hasExtD:
        for reg in range(32):
            regs[reg] += "     f%-2d %5s = %s" % (reg, fp_abi_reg(reg), fmtval(cpu.fprs[reg],hasExtD))

    cpu_regs = "\n".join(regs + ["\npc = %s\n" % (fmtval(cpu.pc,is64),)])

    status_regs = local_status_regs(cpu, is64)

    csr_regs = local_csrs(cpu, is64) if all else ""
    return cpu_regs + status_regs + csr_regs


def local_diff_regs(cpu):
    # TBD: Cache this list per cpu?

    non_diff_regs = (
        "pc","cycle","cycleh","instret","instreth","time","timeh",
        "hpmcounter3", "hpmcounter4", "hpmcounter5", "hpmcounter6",
        "hpmcounter7", "hpmcounter8", "hpmcounter9", "hpmcounter10",
        "hpmcounter11","hpmcounter12","hpmcounter13","hpmcounter14",
        "hpmcounter15","hpmcounter16","hpmcounter17","hpmcounter18",
        "hpmcounter19","hpmcounter20","hpmcounter21","hpmcounter22",
        "hpmcounter21","hpmcounter22","hpmcounter23","hpmcounter24",
        "hpmcounter25","hpmcounter26","hpmcounter27","hpmcounter28",
        "hpmcounter29","hpmcounter30","hpmcounter31",
        "hpmcounter3h", "hpmcounter4h", "hpmcounter5h", "hpmcounter6h",
        "hpmcounter7h", "hpmcounter8h", "hpmcounter9h", "hpmcounter10h",
        "hpmcounter11h","hpmcounter12h","hpmcounter13h","hpmcounter14h",
        "hpmcounter15h","hpmcounter16h","hpmcounter17h","hpmcounter18h",
        "hpmcounter19h","hpmcounter20h","hpmcounter21h","hpmcounter22h",
        "hpmcounter21h","hpmcounter22h","hpmcounter23h","hpmcounter24h",
        "hpmcounter25h","hpmcounter26h","hpmcounter27h","hpmcounter28h",
        "hpmcounter29h","hpmcounter30h","hpmcounter31h",

        "mcycle","mcycleh","minstret","minstreth",
        "mhpmcounter3", "mhpmcounter4", "mhpmcounter5", "mhpmcounter6",
        "mhpmcounter7", "mhpmcounter8", "mhpmcounter9", "mhpmcounter10",
        "mhpmcounter11","mhpmcounter12","mhpmcounter13","mhpmcounter14",
        "mhpmcounter15","mhpmcounter16","mhpmcounter17","mhpmcounter18",
        "mhpmcounter19","mhpmcounter20","mhpmcounter21","mhpmcounter22",
        "mhpmcounter21","mhpmcounter22","mhpmcounter23","mhpmcounter24",
        "mhpmcounter25","mhpmcounter26","mhpmcounter27","mhpmcounter28",
        "mhpmcounter29","mhpmcounter30","mhpmcounter31",
        "mhpmcounter3h", "mhpmcounter4h", "mhpmcounter5h", "mhpmcounter6h",
        "mhpmcounter7h", "mhpmcounter8h", "mhpmcounter9h", "mhpmcounter10h",
        "mhpmcounter11h","mhpmcounter12h","mhpmcounter13h","mhpmcounter14h",
        "mhpmcounter15h","mhpmcounter16h","mhpmcounter17h","mhpmcounter18h",
        "mhpmcounter19h","mhpmcounter20h","mhpmcounter21h","mhpmcounter22h",
        "mhpmcounter21h","mhpmcounter22h","mhpmcounter23h","mhpmcounter24h",
        "mhpmcounter25h","mhpmcounter26h","mhpmcounter27h","mhpmcounter28h",
        "mhpmcounter29h","mhpmcounter30h","mhpmcounter31h"
    )

    diff_regs = cpu.iface.int_register.all_registers()
    diff_regs = remove_regs(cpu, diff_regs, non_diff_regs)
    def f(x):
        return cpu.iface.int_register.get_name(x)
    return list(map(f, diff_regs))


common_disass = sim_commands.make_disassembly_fun()
def local_disass(cpu, prefix, address, *args):
    (length, ret) = common_disass(cpu, prefix, address, *args)

    if address == cpu.iface.processor_info.get_program_counter():
        if cpu.wait_for_interrupt:
            ret += ' # Waiting for interrupt'
    return (length, ret)

class mmu_translation:
    def __init__(self, cpu, entry_size, x4):
        self.cpu = cpu
        self.esize = entry_size
        self.tree = False
        self.x4 = x4

    def logtree(self, txt):
        if self.tree:
            print(txt)

    def extract_properties(self, entry):
        def b(val, bitnr, char):
            return char if (val >> bitnr) & 1 else " "
        res = ""
        res += b(entry, 7, "D")
        res += b(entry, 6, "A")
        res += b(entry, 5, "G")
        res += b(entry, 4, "U")
        res += b(entry, 3, "X")
        res += b(entry, 2, "W")
        res += b(entry, 1, "R")
        res += b(entry, 0, "V")
        return res

    def table_size(self, level):
        if self.x4 and level == self.root_level():
            return 0x4000
        else:
            return 0x1000

    def scan_table(self, table_base_address, vpn_part_so_far, level, indent=""):
        translation = []
        if (level < 0):
            self.logtree("%s   ! Error, level < 0" % (indent))
            return translation

        if (indent == ""):
            self.logtree(" * Scanning table page level %d @ 0x%x" % (level, table_base_address))

        range_end = (self.table_size(level) // self.esize)
        for pte_index in range(range_end):
            pte_address = table_base_address + pte_index * self.esize
            pte = SIM_read_phys_memory(self.cpu, pte_address, self.esize)

            # TPE Valid
            if pte & 0x1:
                vpn = vpn_part_so_far + self.vpn_part(level, pte_index)
                ppn = self.extract_ppn(pte)

                # Pointer to next level
                if (pte & 0xe) == 0:
                    self.logtree("%s   + [%3x] Pointer to next block, level %d @ 0x%x" %
                        (indent, pte_index, level-1, ppn))
                    translation += self.scan_table(ppn, vpn, level - 1, indent + "   |")

                #Leaf-node
                else:
                    vpn   = self.extend_vpn(vpn)
                    size  = self.translation_size(level)
                    props = self.extract_properties(pte)
                    if self.valid_end_ppn(level, ppn):
                        self.logtree("%s   + [%3x] Leaf %s PTE: VN: 0x%x => 0x%x [%s]" %
                                     (indent, pte_index, size, vpn, ppn, props))
                        translation.append([size, vpn, ppn, props])
                    else:
                        self.logtree("%s   ? [%3x] Misaligned superpage: VN[%s]: 0x%x => 0x%x [%s]" %
                                     (indent, pte_index, size, vpn, ppn, props))

        return translation


class Sv32_mmu_translation(mmu_translation):
    def __init__(self, cpu, satp, x4 = False):
        mmu_translation.__init__(self, cpu, 4, x4)
        self.satp = satp

    def root_level(self):
        return 1

    def extract_ppn(self, table_entry):
        return (table_entry & 0xfffffc00) << 2

    def vpn_part(self, level, table_offset):
        return table_offset << (10*level + 12)

    def translation_size(self, level):
        return ("4K","4M")[level]

    def valid_end_ppn(self,level, ppn):
        return level == 0 or (ppn & 0x3fffff) == 0

    def extend_vpn(self, vpn):
        return vpn

    def scan(self, tree):
        self.tree = tree
        table_base_address = (self.satp & 0x3fffff) << 12
        return self.scan_table(table_base_address, 0, self.root_level())

class Sv39_mmu_translation(mmu_translation):
    def __init__(self, cpu, satp, x4 = False):
        mmu_translation.__init__(self, cpu, 8, x4)
        self.satp = satp

    def root_level(self):
        return 2

    def extract_ppn(self, table_entry):
        return (table_entry & 0x003ffffffffffc00) << 2

    def vpn_part(self, level, table_offset):
        return table_offset << (9*level + 12)

    def translation_size(self, level):
        return ("4K","2M","1G")[level]

    def valid_end_ppn(self, level, ppn):
        return ppn & (0,0x1fffff,0x3fffffff)[level] == 0

    def extend_vpn(self, vpn):
        if (vpn >> 38) == 1:
            return vpn | 0xffffff8000000000
        return vpn

    def scan(self, tree):
        self.tree = tree
        table_base_address = (self.satp & 0xfffffffffff) << 12
        return self.scan_table(table_base_address, 0, self.root_level())

class Sv48_mmu_translation(mmu_translation):
    def __init__(self, cpu, satp, x4 = False):
        mmu_translation.__init__(self, cpu, 8, x4)
        self.satp = satp

    def root_level(self):
        return 3

    def extract_ppn(self, table_entry):
        return (table_entry & 0x003ffffffffffc00) << 2

    def vpn_part(self, level, table_offset):
        return table_offset << (9*level + 12)

    def translation_size(self, level):
        return ("4K","2M","1G","512G")[level]

    def valid_end_ppn(self, level, ppn):
        return ppn & (0,0x1fffff,0x3fffffff,0x7fffffffff)[level] == 0

    def extend_vpn(self, vpn):
        if (vpn >> 47) == 1:
            return vpn | 0xffff000000000000
        return vpn

    def scan(self, tree):
        self.tree = tree
        table_base_address = (self.satp & 0xfffffffffff) << 12
        return self.scan_table(table_base_address, 0, self.root_level())

class Sv57_mmu_translation(mmu_translation):
    def __init__(self, cpu, satp, x4 = False):
        mmu_translation.__init__(self, cpu, 8, x4)
        self.satp = satp

    def root_level(self):
        return 4

    def extract_ppn(self, table_entry):
        return (table_entry & 0x003ffffffffffc00) << 2

    def vpn_part(self, level, table_offset):
        return table_offset << (9*level + 12)

    def translation_size(self, level):
        return ("4K","2M","1G","512G","256T")[level]

    def valid_end_ppn(self, level, ppn):
        return ppn & (0,0x1fffff,0x3fffffff,0x7fffffffff,0xffffffffffff)[level] == 0

    def extend_vpn(self, vpn):
        if (vpn >> 56) == 1:
            return vpn | 0xfe00000000000000
        return vpn

    def scan(self, tree):
        self.tree = tree
        table_base_address = (self.satp & 0xfffffffffff) << 12
        return self.scan_table(table_base_address, 0, self.root_level())


def get_mmu(cpu, satp):
    if cpu.XLEN == 64:
        mode = (satp >> 60) & 0xf
        if mode == 8:
            return Sv39_mmu_translation(cpu, satp)
        if mode == 9:
            return Sv48_mmu_translation(cpu, satp)
        if mode == 10:
            return Sv57_mmu_translation(cpu, satp)
    else:
        if ((satp >> 31) & 0x1) == 1:
            return Sv32_mmu_translation(cpu, satp)
    return None


def mmu_tables_command(cpu, tree = False, table = False):
    def cpu_has_supervisor_mode(cpu):
        misa = cpu.misa
        return (misa & 0x40000) == 0x40000

    if not cpu_has_supervisor_mode(cpu):
        print("\nCPU does not have supervisor mode.\n")
        return

    translator = get_mmu(cpu, cpu.satp)

    if not translator:
        print("\nMMU disabled.\n")
        return

    translate(translator, tree, table)


def guest_mmu_tables_command(cpu, tree = False, table = False):
    def cpu_has_hypervisor_mode(cpu):
        misa = cpu.misa
        return (misa & 0x80) == 0x80

    if not cpu_has_hypervisor_mode(cpu):
        print("\nCPU does not have hypervisor mode.\n")
        return

    translator = get_mmu(cpu, cpu.hgatp)

    if not translator:
        print("\nGuest MMU disabled.\n")
        return

    translate(translator, tree, table)


def translate(translator, tree, table):
    mmu_tables = translator.scan(tree)

    if not tree and not table:
        table = True

    if table:
        properties = [(Table_Key_Columns, [
            [(Column_Key_Name, "Size")],
            [(Column_Key_Name, "Virtual"),(Column_Key_Int_Radix, 16)],
            [(Column_Key_Name, "Physical"),(Column_Key_Int_Radix, 16)],
            [(Column_Key_Name,"Attributes")]
        ])]

        print(Table(properties, mmu_tables).to_string(no_row_column=True))

def pmp_regions_command(cpu):
    properties = [(Table_Key_Columns, [
        [(Column_Key_Name, "From"), (Column_Key_Int_Radix, 16)],
        [(Column_Key_Name, "To"),(Column_Key_Int_Radix, 16)],
        [(Column_Key_Name, "Cfg")]
        ])]

    regions = []

    if hasattr(cpu, "dump_pmp_regions"):
        regions = [[f,t,c] for (f,t,c) in cpu.dump_pmp_regions]

    tbl = Table(properties, regions)
    msg = tbl.to_string(no_row_column=True)
    return command_verbose_return(message = msg, value = regions)

def pma_regions_command(cpu):

    def str_attributes(val):
        attrs = []
        if val & 2: attrs.append('memory')
        if val & 4: attrs.append('cachable')
        if val & 8: attrs.append('atomics')
        if val & 1: attrs.append('side effects')
        if not attrs:
            return "nothing"
        return ", ".join(attrs)

    properties = [(Table_Key_Columns, [
        [(Column_Key_Name, "From"), (Column_Key_Int_Radix, 16)],
        [(Column_Key_Name, "To"),(Column_Key_Int_Radix, 16)],
        [(Column_Key_Name, "Attributes")]
        ])]

    regions = []

    if hasattr(cpu, "pma_regions"):
        regions = [[f,t,str_attributes(c)] for (f,t,c) in cpu.pma_regions]
        regions[-1][0] = 'default'
        regions[-1][1] = ''

    tbl = Table(properties, regions)
    msg = tbl.to_string(no_row_column=True, rows_printed=0)
    return command_verbose_return(message = msg, value = regions)

def exception_name(cpu, exc):
    if exc > 23:
        return "Exception_%d" % exc
    return cpu.iface.exception.get_name(exc)

def interrupt_name(cpu, irq):
    irq0 = cpu.iface.exception.get_number('Reserved_Irq_0')

    irqi = irq + irq0
    if irqi not in cpu.iface.exception.all_exceptions():
        return "Irq_%d" % irq

    return cpu.iface.exception.get_name(irqi)

def str_trap_vector_address(vec):
    vecmode = ("Direct","Vectored", "Unknown mode", "Unknown mode")
    mode = vec & 0x3
    address = vec & ~0x3
    return "0x%x [%s]" % (address, vecmode[mode])

def str_cpu_mode(mode):
    strmode = { Riscv_Mode_User:              "User",
                Riscv_Mode_Supervisor:        "Supervisor",
                Riscv_Mode_Machine:           "Machine",
                Riscv_Mode_Guest_User:        "Virtual User",
                Riscv_Mode_Guest_Supervisor:  "Virtual Supervisor" }
    if mode in strmode:
        return strmode[mode]
    return "???"

def str_riscv_isa(misa):

    def isa_ext(misa, ch):
        return ch if isamask(ch) & misa else ""

    str = "RV"
    if (misa >> 62) == 2:
        str += "64"
    elif (misa >> 30) == 1:
        str += "32"
    g_mask = isamask('A') | isamask('M') | isamask('F') | isamask('D')
    if (misa & g_mask) == g_mask:
        str += "G"
    else:
        str += isa_ext(misa, 'I')
        str += isa_ext(misa, 'E')
        str += isa_ext(misa, 'M')
        str += isa_ext(misa, 'A')
        str += isa_ext(misa, 'F')
        str += isa_ext(misa, 'D')

    str += isa_ext(misa, 'Q')
    str += isa_ext(misa, 'C')
    str += isa_ext(misa, 'P')
    str += isa_ext(misa, 'V')
    str += isa_ext(misa, 'H')

    return str

def str_riscv_extensions(cpu):

    # Add Z, S and X extensions

    def sort_Z_extensions(zexts):
        allzext = []
        for e in 'imafdqlcbkjtpv':
            if e in zexts:
                for ext in sorted(zexts[e]):
                    allzext.append(ext)

        return allzext

    zexts = {}
    sexts = []
    xexts = []

    for attr in dir(cpu.extensions):
        if len(attr) < 2:
            continue
        if attr[0] in ('X', 'S', 'Z'):
            if not getattr(cpu.extensions, attr):
                continue
            if attr[0] == 'Z':
                zexts.setdefault(attr[1], []).append(attr)
            elif attr[0] == 'S':
                sexts.append(attr)
            elif attr[0] == 'X':
                xexts.append(attr)

    # Special treatment for mmu support

    if cpu.supervisor_mode_supported:
        sexts.append('Svbare')
        if cpu.mmu_mode_support != 0:
            if cpu.mmu_mode_support == 1:
                sexts.append('Sv32')
            elif cpu.mmu_mode_support == 8:
                sexts.append('Sv39')
            elif cpu.mmu_mode_support == 9:
                sexts.append('Sv48')
            elif cpu.mmu_mode_support == 10:
                sexts.append('Sv57')

            if not cpu.extensions.Svadu:
                sexts.append('Svade')

    aexts = sort_Z_extensions(zexts) + sorted(sexts) + sorted(xexts)
    return "_".join(aexts)

def str_supported_modes(cpu):
    str = "Machine"
    if cpu.hypervisor_mode_supported:
        str += ", Hypervisor"
    if cpu.supervisor_mode_supported:
        str += ", Supervisor"
    if cpu.user_mode_supported:
        str += ", User"
    return str

def str_pending_interrupt(cpu):

    pending = cpu.interrupt_pending
    if pending < 0:
        if pending == -2:
            return "RESET"
        return "----"
    return interrupt_name(cpu, pending)

def str_signals(cpu, signals):
    str = ""
    first = True
    for n in range(31, -1, -1):
        if signals & (1 << n):
            if not first:
                str += ", "
            else:
                first = False
            str += interrupt_name(cpu, n)
    return str

def str_interrupts(cpu, interrupts):
    return str_signals(cpu, interrupts)

def str_exceptions(cpu, exceptions):
    str = ""
    first = True
    for n in range(31, -1, -1):
        if exceptions & (1 << n):
            if not first:
                str += ", "
            else:
                first = False
            str += exception_name(cpu, n)
    return str

def str_cause(cpu, cause):
    if cause > 0x8000000:
        return "%s" % interrupt_name(cpu, cause & 0xfff)
    return "%s" % exception_name(cpu, cause)


def get_machine_mode_status(cpu):
    is64 = 1 if cpu.XLEN == 64 else 0
    status = [("Trap address (M)", str_trap_vector_address(cpu.mtvec)),
          ("Enabled interrupt (M)", str_interrupts(cpu, cpu.mie))]

    if hasattr(cpu,"medeleg"):
        status.append(("Delegated exceptions (M)", str_exceptions(cpu, cpu.medeleg)))
    if hasattr(cpu,"mideleg"):
        status.append(("Delegated interrupts (M)", str_interrupts(cpu, cpu.mideleg)))

    status.append(("Last mcause", str_cause(cpu, cpu.mcause)))
    status.append(("Last mepc",   "%s" % fmtval(cpu.mepc, is64)))
    status.append(("Last mtval",  "%s" % fmtval(cpu.mtval, is64)))
    if hasattr(cpu, "mtval2"):
        status.append(("Last mtval2",  "%s" % fmtval(cpu.mtval2, is64)))

    if hasattr(cpu, "mtinst"):
        status.append(("Last mtinst",  "%s" % fmtval(cpu.mtinst, is64)))

    return (None, status)

def get_supervisor_mode_status(cpu):
    is64 = 1 if cpu.XLEN == 64 else 0
    status = [("Trap address (S)", str_trap_vector_address(cpu.stvec)),
          ("Enabled interrupt (S)", str_interrupts(cpu, cpu.sie))]

    status.append(("Last scause", str_cause(cpu, cpu.scause)))
    status.append(("Last sepc", "%s" % fmtval(cpu.sepc, is64)))
    status.append(("Last stval", "%s" % fmtval(cpu.stval, is64)))

    return (None, status)

def get_hypervisor_mode_status(cpu):
    is64 = 1 if cpu.XLEN == 64 else 0
    status = [("hstatus", f'0x{cpu.hstatus:08x}'),
              ("", str_hstatus(cpu)),
              ("Enabled interrupt (H)", str_interrupts(cpu, cpu.hie)),
              ("Enabled Guest External Interrupts", str_bitvector(cpu.hgeie)),
              ("Pending Guest External Interrupts", str_bitvector(cpu.hgeip))]

    if hasattr(cpu,"hedeleg"):
        status.append(("Delegated exceptions (H)", str_exceptions(cpu, cpu.hedeleg)))
    if hasattr(cpu,"hideleg"):
        status.append(("Delegated interrupts (H)", str_interrupts(cpu, cpu.hideleg)))

    status.append(("Last htval", fmtval(cpu.htval, is64)))
    status.append(("Last htinst", fmtval(cpu.htval, is64)))

    return (None, status)

def get_guest_mode_status(cpu):
    is64 = 1 if cpu.XLEN == 64 else 0
    status = [("vsstatus", f'0x{cpu.vsstatus:08x}'),
              ("", str_vsstatus(cpu)),
              ("Trap address (VS)", str_trap_vector_address(cpu.vstvec)),
              ("Enabled interrupt (VS)", str_interrupts(cpu, cpu.vsie))]

    status.append(("Last vscause", fmtval(cpu.vscause, is64)))
    status.append(("Last vstval", fmtval(cpu.vstval, is64)))
    status.append(("Last htinst", fmtval(cpu.htval, is64)))

    return (None, status)


def get_riscv_status(obj):
    cpu = obj
    status = []
#    if hasattr(sim_commands, "common_processor_get_status"):
#        status = [ (None, sim_commands.common_processor_get_status(cpu)) ]

    status.append((None,
                   [("Current cpu mode",str_cpu_mode(cpu.cpu_mode)),
                    ("mstatus","0x%x" % cpu.mstatus),
                    ("",str_mstatus(cpu))]))
    status.append((None,
                   [("Pending interrupt", str_pending_interrupt(cpu)),
                    ("Raised signals", str_signals(cpu, cpu.mip | cpu.signal_status))]))

    status.append(get_machine_mode_status(cpu))
    if cpu.supervisor_mode_supported:
        status.append(get_supervisor_mode_status(cpu))
    if cpu.hypervisor_mode_supported:
        status.append(get_hypervisor_mode_status(cpu))
        status.append(get_guest_mode_status(cpu))

    # Add more status info here if needed

    return status

def get_riscv_info(obj):
    info = sim_commands.common_processor_get_info(obj)
    info.append(("Supported isa", str_riscv_isa(obj.misa)))
    info.append(("Extensions", str_riscv_extensions(obj)))
    info.append(("Supported modes", str_supported_modes(obj)))

    return [ (None, info) ]


def riscv_get_opcode_length_info(cpu):
    ret = opcode_length_info_t()
    ret.min_alignment = 2
    ret.max_length = 4
    ret.avg_length = 4

    return ret


def setup_processor_ui(rv_model):
    cl = SIM_get_class(rv_model)
    SIM_register_interface(
        cl, 'processor_cli',
        processor_cli_interface_t(
            get_disassembly = local_disass,
            get_pregs = local_pregs,
            get_diff_regs = local_diff_regs,
        ))

    SIM_register_interface(cl, 'processor_gui', processor_gui_interface_t())

    opc_iface = opcode_info_interface_t()
    opc_iface.get_opcode_length_info = riscv_get_opcode_length_info
    SIM_register_interface(cl, 'opcode_info', opc_iface)

    sim_commands.register_aprof_views(cl)
    new_info_command(rv_model, get_riscv_info)
    new_status_command(rv_model, get_riscv_status)

    all_attrs = [a[0] for a in cl.attributes]

    if "pmpaddr0" in all_attrs:
        new_command("print-pmp-regions", pmp_regions_command,
                    args = [],
                    cls = rv_model,
                    type = ["Processors"],
                    short = "print current PMP regions",
                    doc = """Prints the current PMP regions of the given RISC-V processor.""")

    if "pma_regions" in all_attrs:
        new_command("print-pma-regions", pma_regions_command,
                    args = [],
                    cls = rv_model,
                    type = ["Processors"],
                    short = "print current PMA regions",
                    doc = """Prints the current PMA regions of the given RISC-V processor.""")

    if "satp" in all_attrs:
        new_command("print-page-table", mmu_tables_command,
                    args = [arg(flag_t, "-tree"), arg(flag_t, "-table")],
                    cls = rv_model,
                    type = ["Processors"],
                    short = "print page table entries",
                    doc = """Prints the Page Table entries of the given RISC-V processor.
                    The <tt>-tree</tt> flags, prints the entire PTE tree
                    structure, while the <tt>-table</tt> (default) presents
                    the PTEs in a flat table, where only the leaf-nodes
                    translations are shown.""")

    if "vsatp" in all_attrs:
        new_command("print-guest-page-table", guest_mmu_tables_command,
                    args = [arg(flag_t, "-tree"), arg(flag_t, "-table")],
                    cls = rv_model,
                    type = ["Processors"],
                    short = "print page table entries",
                    doc = """Prints the Page Table entries of the given RISC-V processor.
                    The <tt>-tree</tt> flags, prints the entire PTE tree
                    structure, while the <tt>-table</tt> (default) presents
                    the PTEs in a flat table, where only the leaf-nodes
                    translations are shown.""")


def add_compat_csr_attributes(rv_model, compat_csrs):
    cl = SIM_get_class(rv_model)
    for csr in compat_csrs:
        SIM_register_attribute(cl, csr,
                               None,
                               lambda obj, value: Sim_Set_Ok,
                               Sim_Attr_Pseudo | Sim_Attr_Internal,
                               "i", "")

def cmd_list_riscv_custom_cpu_parameters(base_cls):
    err = None
    try:
        cls = SIM_get_class(base_cls)
    except SimExc_General as e:
        err = f'Cannot list custom cpu parameter: {str(e)}'

    if not err and not hasattr(cls, "customization_module"):
        err = f'Class {base_cls} cannot be used as RISC-V customization base-class'

    if err:
        raise CliError(err)

    data = []
    props = [
        (Table_Key_Columns, [
            [(Column_Key_Name, 'Name')],
            [(Column_Key_Name, 'Description')],
            [(Column_Key_Name, 'Type')],
            [(Column_Key_Name, 'Default'),
             (Column_Key_Alignment, "center")
             ],
            [(Column_Key_Name, 'Valid values')],
        ]),
        (Table_Key_Extra_Headers, [
            (Extra_Header_Key_Row, [ #  row 1
                [(Extra_Header_Key_Name,
                  f'Available parameters for customizing class: {base_cls}')]
            ])
        ])
    ]

    cml = importlib.import_module(cls.customization_module)

    for (n, d, t, dv, *cp) in cml.internal_get_parameter_limits(cls.name):
        # See riscv_custom_cpu_internal.h for description of
        if t in 'string':
            opt = 'Any string'
            tp = t
        elif t == 'bool':
            opt = "True or False"
            tp = t
            dv = 'True' if dv else 'False'
        elif t == 'set':
            okvals = cp[0]
            if len(okvals) == 1:
                opt = f'Must be {okvals[0]}'
            elif len(okvals) == 2:
                opt = f'{okvals[0]} or {okvals[1]}'
            else:
                opt = f'One of {",".join([str(i) for i in okvals])}'
                tp = 'int'
        elif t == 'range':
            (low, high) = cp
            opt = f'{low} - {high}'
            tp = 'int'
        elif t == 'misa':
            opt = f'Valid lower 26 bits of misa register, max is 0x{cp[1]:x}'
            tp = 'int'
            dv = f'0x{dv:06x}'

        data.append([n,d,tp,dv,opt])

    tbl = Table(props, data)
    msg = tbl.to_string(no_row_column=True) if data else ""

    return command_verbose_return(msg, data)

def setup_customize_processor_cmds():

    tpv = 'riscv-cpu-customization'
    if not tech_preview_exists(tpv):
        add_tech_preview(tpv)
        new_tech_preview_command('list-riscv-custom-cpu-parameters', tpv,
                                 cmd_list_riscv_custom_cpu_parameters,
                                 args = [arg(str_t, 'base_class')],
                                 type = ["Processors"],
                                 short = 'list available parameters to customize a RISC-V CPU class',
                                 doc = """
                                 List all available parameters when creating a customized version of
                                 the <arg>base_class</arg>.""")
