# INTEL CONFIDENTIAL

# © 2010 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

from dataclasses import dataclass
import functools
from cli import *
import sim_commands
import fp_to_string
import simics
from simics import *
import vmp_common
from functools import cmp_to_key
from . import generated_cpuid_print
import conf
import table


column_size = { }

def bit(n):
    if n < 0 or n > 63:
        raise Exception("Internal error: bit number is out of range")
    return 1 << n

def local_pending_exception(cpu):
    pend = []
    if (SIM_class_has_attribute(cpu.classname, "pending_exception") and
        cpu.pending_exception):
        exc_type = {
            0: "exception",
            1: "software interrupt",
            2: "interrupt",
            3: "software exception",
            4: "privileged software exception",
            5: "triple fault",
            6: "NMI",
            7: "exception event",
            } [cpu.pending_exception_type]
        pend.append("Pending %s, vector %d" % (exc_type,
                                               cpu.pending_exception_vector))

    if (SIM_class_has_attribute(cpu.classname, "pending_start_up") and
        cpu.pending_start_up):
        pend.append("Pending StartUp IPI")

    if SIM_class_has_attribute(cpu.classname, "vmx_pending_exit"):
        if cpu.vmx_pending_exit != None:
            pend.append("Pending VMExit %d" % cpu.vmx_pending_exit)

    if (SIM_class_has_attribute(cpu.classname, "latch_nmi") and
        cpu.latch_nmi and not cpu.block_nmi):
        pend.append("Pending NMI")

    if getattr(cpu, "pending_reset", False):
        pend.append("Pending RESET")

    if (SIM_class_has_attribute(cpu.classname, "pending_init") and
        cpu.pending_init):
        pend.append("Pending INIT")

    if not (cpu.iface.x86_reg_access.get_interruptibility_state() & X86_Intstate_Blocking_INT_Mov_Ss):
        if (SIM_class_has_attribute(cpu.classname, "pending_debug_exception") and cpu.pending_debug_exception):
            pend.append("Pending debug exception")

    if (cpu.iface.x86_reg_access.get_rflags() & 0x200) and cpu.iface.x86_reg_access.get_interruptibility_state() == X86_Intstate_Not_Blocking:
        if (SIM_class_has_attribute(cpu.classname, "waiting_interrupt") and cpu.waiting_interrupt):
            pend.append("Pending interrupt")

    if (SIM_class_has_attribute(cpu.classname, "pending_user_interrupt")
        and cpu.pending_user_interrupt):
        pend.append("Pending user interrupt")

    try:
        (len, str) = SIM_disassemble_address(cpu,
                          cpu.iface.processor_info.get_program_counter(), 1, 0)
        if str == "cpuid" and cpu.eax == 0x4711:
            pend.append("magic instruction (cpuid w/ eax==0x4711)")
    except:
        pass
    act_state = cpu.iface.x86_reg_access.get_activity_state()
    if act_state != X86_Activity_Normal:
        act_str = {X86_Activity_Normal: None,
                   X86_Activity_Hlt: "HLT state",
                   X86_Activity_Shutdown: "Shutdown state",
                   X86_Activity_Wait_For_SIPI: "Wait for SIPI state",
                   X86_Activity_Cx_State: "Cx state",
                   X86_Activity_MWait: "MWait state",
                   X86_Activity_Senter_Sleep_State: "SENTER sleep state"}[act_state]
        if act_str:
            pend.append(act_str)

    if not cpu.iface.processor_info.get_enabled():
        pend.append("cpu is user disabled")

    ret_str = ", ".join(pend)
    if ret_str != "":
        return ret_str
    else:
        return None

def logical_to_physical(cpu, vaddr, access):
    return cpu.iface.processor_info.logical_to_physical(vaddr, access)

def x86_get_opcode(cpu, paddr, length):
    b = []
    try:
        for i in range(length):
            b.append("%02x" % SIM_read_phys_memory(cpu, paddr + i, 1))
    except:
        pass
    return "%-17s" % " ".join(b)

seg_to_index = {"es": X86_Seg_ES,
                "ss": X86_Seg_SS,
                "fs": X86_Seg_FS,
                "gs": X86_Seg_GS,
                "cs": X86_Seg_CS,
                "ds": X86_Seg_DS,
                "tr": X86_Seg_TR,
                "ldtr": X86_Seg_LDTR}

def local_translate_to_physical(cpu, p, a):
    if p == "":
        r = local_translate_to_physical(cpu, "ds", a)
        if not r.valid:
            r = local_translate_to_physical(cpu, "cs", a)
        return r

    def segbase(cpu, seg):
        seg_reg = cpu.iface.x86_reg_access.get_seg(seg_to_index[seg])
        return seg_reg.base

    if p == "l" or p == "ld":
        pa = cpu.iface.x86.linear_to_physical(1, a)
    elif p == "li":
        pa = cpu.iface.x86.linear_to_physical(0, a)
    elif p == "es":
        pa = cpu.iface.x86.linear_to_physical(1, segbase(cpu, "es") + a)
    elif p == "ds":
        return cpu.iface.processor_info.logical_to_physical(a, Sim_Access_Read)
    elif p == "cs":
        return cpu.iface.processor_info.logical_to_physical(a, Sim_Access_Execute)
    elif p == "ss":
        pa = cpu.iface.x86.linear_to_physical(1, segbase(cpu, "ss") + a)
    elif p == "fs":
        pa = cpu.iface.x86.linear_to_physical(1, segbase(cpu, "fs") + a)
    elif p == "gs":
        pa = cpu.iface.x86.linear_to_physical(1, segbase(cpu, "gs") + a)
    else:
        raise Exception("Illegal address prefix " + p)

    ret = physical_block_t()
    ret.valid = 1
    ret.address = pa
    ret.block_start = pa
    ret.block_end = pa
    return ret

def local_get_address_prefix(cpu):
    return "ds"

def hex_str(val, size): return "%0*x" % (size, val)

def hex_str_or_none(val):
    if val is None: return "None"
    return "%#x" % val

#
# -------------------- local-pregs --------------------
#

def get_eflags(cpu, reg_name="eflags"):
    ef = cpu.iface.x86_reg_access.get_rflags()
    if (ef & 0xf000) == 0xf000:
        has_pm = False
    else:
        has_pm = True
    ret = "%s = " % reg_name
    if has_pm:
        ret += "%d %d %d %d %d %d " % (
        (ef >> 21) & 1,
        (ef >> 20) & 1,
        (ef >> 19) & 1,
        (ef >> 18) & 1,
        (ef >> 17) & 1,
        (ef >> 16) & 1)

    ret += "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d = 0x%s\n" % (
        (ef >> 15) & 1,
        (ef >> 14) & 1,
        (ef >> 13) & 1,
        (ef >> 12) & 1,
        (ef >> 11) & 1,
        (ef >> 10) & 1,
        (ef >> 9) & 1,
        (ef >> 8) & 1,
        (ef >> 7) & 1,
        (ef >> 6) & 1,
        (ef >> 5) & 1,
        (ef >> 4) & 1,
        (ef >> 3) & 1,
        (ef >> 2) & 1,
        (ef >> 1) & 1,
        (ef >> 0) & 1,
        hex_str(ef,8))
    ret += " "*(len(reg_name) + 3)
    if has_pm:
        ret += "I V V A V R - N I I O D I T S Z - A - P - C\n"
    else:
        ret += "- - - - O D I T S Z - A - P - C\n"
    ret += ""
    ret += " "*(len(reg_name) + 3)
    if has_pm:
        ret += "D I I C M F   T O O F F F F F F   F   F   F\n"
    else:
        ret += "        F F F F F F   F   F   F\n"
    ret += ""
    ret += " "*(len(reg_name) + 3)
    if has_pm:
        ret += "  P F           P P\n"
        ret += " "*(len(reg_name) + 3)
        ret += "                L L\n"
    return ret


def get_uif(obj):
    if SIM_class_has_attribute(obj.classname, "uif"):
        return "\nuif = %d\n" % (obj.uif)
    return ''


def local_string(val, aa64):
    if aa64:
        return "0x%016x" % val
    else:
        return "0x" + hex_str(val, 8)

def get_seg_attribute(obj, segregname):
    return obj.iface.int_register.read(
        obj.iface.int_register.get_number("%s_attr" % segregname))

def get_seg_valid(obj, segregname):
    if SIM_get_attribute(obj, segregname)[9]:
        return ""
    else:
        return ", invalid"

def debug_break_desc(num, dr7):
    enabled = dr7 & (3 << (num * 2))
    rw = (dr7 >> ((num * 4) + 16)) & 3
    len = (dr7 >> ((num * 4) + 18)) & 3
    rw_desc = ("execution", "write", "I/O", "read/write")
    len_desc = ("1 byte", "2 bytes", "8 bytes", "4 bytes")
    if not enabled:
        return "disabled"
    else:
        return "%s, %s" % (len_desc[len], rw_desc[rw])

def cpu_has_efer(cpu):
    try:
        return cpu.iface.x86_msr.get_name(0xC0000080) != None
    except:
        return False

def cpu_get_efer(cpu):
    return cpu.iface.x86_reg_access.get_efer()

def cpu_has_longmode(obj):
    return obj.iface.x86_cpuid_query.cpuid_query(0x80000001, 0).d & bit(29)

def cpu_has_apx(obj):
    return hasattr(obj, "r16")

def cpu_has_pm(obj):
    return hasattr(obj, "idtr_base")

def cpu_has_cr4(obj):
    return True

def cpu_has_xmm(obj):
    return hasattr(obj, "xmm")

def cpu_has_ymm(obj):
    return hasattr(obj, "ymmu")

def cpu_has_mxcsr(obj):
    return hasattr(obj, "mxcsr")

def get_num_gprs(obj):
    return 32 if cpu_has_apx(obj) else 16

def get_remaining_regs(obj):
    ret = "\n"
    aa64 = cpu_has_longmode(obj)
    has_pm = cpu_has_pm(obj)

    def format_segment(obj, n, has_limit_and_attr):
        seg_reg = obj.iface.x86_reg_access.get_seg(seg_to_index[n])
        ret = f"{n:<4} = 0x{seg_reg.sel:04x}, base = 0x{hex_str(seg_reg.base,8)}"
        if has_limit_and_attr:
            ret += ", limit = 0x%x, attr = 0x%x" % (seg_reg.limit, seg_reg.attr)
        ret += "\n"
        return ret
    ret += format_segment(obj, "es", has_pm)
    ret += format_segment(obj, "cs", has_pm)
    ret += format_segment(obj, "ss", has_pm)
    ret += format_segment(obj, "ds", has_pm)
    ret += format_segment(obj, "fs", True)
    ret += format_segment(obj, "gs", True)
    ret += format_segment(obj, "tr", True)
    ret += format_segment(obj, "ldtr", True)

    if has_pm:
        idtr = obj.iface.x86_reg_access.get_system_seg(X86_Idtr)
        ret += "idtr: base = %s, limit = %05x\n" % (local_string(idtr.base,aa64), idtr.limit)
        gdtr = obj.iface.x86_reg_access.get_system_seg(X86_Gdtr)
        ret += "gdtr: base = %s, limit = %05x\n" % (local_string(gdtr.base,aa64), gdtr.limit)

    if cpu_has_efer(obj):
        efer = cpu_get_efer(obj)
        ret += "\nefer = %d %d - %d -- %d = 0x%s\n" % (
            (efer  >> 11) & 1,
            (efer  >> 10) & 1,
            (efer >> 8) & 1,
            (efer) & 1,
            hex_str(efer,8))
        ret += "       N L   L    S\n"
        ret += "       X M   M    C\n"
        ret += "       E A   E    E\n"

    cr0 = obj.iface.x86_reg_access.get_cr(X86_Cr0)

    ret += "\ncr0 = %d %d %d -- %d - %d -- %d %d %d %d %d %d = 0x%s\n" % (
        (cr0 >> 31) & 1,
        (cr0 >> 30) & 1,
        (cr0 >> 29) & 1,
        (cr0 >> 18) & 1,
        (cr0 >> 16) & 1,
        (cr0 >> 5) & 1,
        (cr0 >> 4) & 1,
        (cr0 >> 3) & 1,
        (cr0 >> 2) & 1,
        (cr0 >> 1) & 1,
        (cr0 >> 0) & 1,
        hex_str(cr0,8))
    ret += "      P C N    A   W    N E T E M P\n"
    ret += "      G D W    M   P    E T S M P E\n\n"

    ret += "cr2 = %s\n" % local_string(obj.iface.x86_reg_access.get_cr(X86_Cr2),aa64)
    ret += "cr3 = %s\n\n" % local_string(obj.iface.x86_reg_access.get_cr(X86_Cr3),aa64)

    cr4 = obj.iface.x86_reg_access.get_cr(X86_Cr4)
    cr4_width = 33
    cr4_bits = ' '.join(str((cr4 >> n) & 1)
                        for n in reversed(range(cr4_width)))
    ret += (
        f"cr4 = {cr4_bits} = 0x{hex_str(cr4, 8)}\n"
            # Note that non-public fields are not named below
            "      F       L     U P C P S S   O P F S S V L U O O P P M P P D T P V\n"
            "      R       A     I K E K M M   S C S E M M A M S S C G C A S E S V M\n"
            "      E       M     N S T E A E   X I G E X X 5 I X F E E E E E   D I E\n"
            "      D       S     T       P P   S D B   E E 7 P M X\n"
            "              U     R             A E A           M S\n"
            "              P                   V   S           E R\n"
            "                                  E   E           X\n"
            "                                                  C\n"
            "                                                  P\n"
            "                                                  T\n\n"
    )
    xcr0 = obj.iface.x86_reg_access.get_xcr(X86_Xcr0)
    ret += f"xcr0 = {xcr0:032b} = {xcr0:#08x}\n\n"

    dr7 = obj.iface.x86_reg_access.get_dr(X86_Dr7)
    dr0 = obj.iface.x86_reg_access.get_dr(X86_Dr0)
    dr1 = obj.iface.x86_reg_access.get_dr(X86_Dr1)
    dr2 = obj.iface.x86_reg_access.get_dr(X86_Dr2)
    dr3 = obj.iface.x86_reg_access.get_dr(X86_Dr3)
    ret += "dr0 = %s %s\n" % (local_string(dr0,aa64), debug_break_desc(0, dr7))
    ret += "dr1 = %s %s\n" % (local_string(dr1,aa64), debug_break_desc(1, dr7))
    ret += "dr2 = %s %s\n" % (local_string(dr2,aa64), debug_break_desc(2, dr7))
    ret += "dr3 = %s %s\n" % (local_string(dr3,aa64), debug_break_desc(3, dr7))

    dr6 = obj.iface.x86_reg_access.get_dr(X86_Dr6)
    ret += "\ndr6 = %d %d %d -- %d %d %d %d = 0x%s\n" % (
        (dr6 >> 15) & 1,
        (dr6 >> 14) & 1,
        (dr6 >> 13) & 1,
        (dr6 >> 3) & 1,
        (dr6 >> 2) & 1,
        (dr6 >> 1) & 1,
        (dr6 >> 0) & 1,
        hex_str(dr6, 4))
    ret += "      B B B    B B B B\n"
    ret += "      T S D    3 2 1 0\n"
    ret += "\ndr7 = %s\n\n" % hex_str(dr7, 8)

    leaf_1 = obj.iface.x86_cpuid_query.cpuid_query(1, 0)
    leaf_d = obj.iface.x86_cpuid_query.cpuid_query(0xd, 0)
    has_fpu = leaf_1.d & 1
    has_sse = leaf_1.d & (1 << 25)
    has_xsave = leaf_1.c & (1 << 26)
    has_ymm = leaf_d.a & (1 << 2)
    has_opmask = leaf_d.a & (1 << 5)
    has_zmm_hi = leaf_d.a & (1 << 6)
    has_zmm16_31 = leaf_d.a & (1 << 7)

    # FPU present
    if (has_fpu):
        ret += get_fpu_cmd(obj, 0, 0, 0, 0)
        ret += "\n"

    # SSE present
    if (has_sse):
        ret += get_xmm_cmd(obj, 0, 0, 0, 0, 0, 0)
        ret += "\n"

    # XSAVE and YMM present
    if (has_xsave and has_ymm):
        ret += get_ymm_hi_cmd(obj, 0, 0, 0, 0, 0, 0)
        ret += "\n"

    # XSAVE and ZMM present
    if (has_xsave and has_zmm_hi):
        ret += get_zmm_hi_cmd(obj, 0, 0, 0, 0, 0, 0)
        ret += "\n"

    # XSAVE and ZMM16..31 present
    if (has_xsave and has_zmm16_31):
        ret += get_zmm_cmd(obj, 16, 0, 0, 0, 0, 0, 0)
        ret += "\n"

    # XSAVE and OpMask present
    if (has_xsave and has_opmask):
        ret += get_opmask_cmd(obj)
        ret += "\n"

    # MXCSR present
    if (has_sse or (has_xsave and leaf_d.a > 1)):
        ret += get_mxcsr_cmd(obj)
        ret += "\n"

    return ret

def pregs_fpu_cmd(obj, f, x, i, b):
    print(get_fpu_cmd(obj, f, x, i, b))

def get_fpu_cmd(obj, f, x, i, b):
    fpu_env = obj.iface.x86_reg_access.get_fpu_env()
    fpu_status = fpu_env.sw
    fpu_control = fpu_env.cw
    fpu_tag = fpu_env.tag
    if f + x + i + b == 0:
        x = 1
    if f + x + i + b > 1:
        return "Only one flag of -f, -x, -i, -b can be given to the command"
    ret = ""
    for fnum in range(8):
        ret += "f%d (ST%d) = " % (fnum, (fnum - ((fpu_status >> 11) & 7)) & 7)
        freg = obj.iface.x86_reg_access.get_freg(fnum)
        int_val = freg.low | (freg.high << 64)
        if x:
            ret += "%016x" % int_val
            tag = (fpu_tag >> (fnum * 2)) & 3
            tag_desc = ("valid", "zero", "special", "empty")[tag]
            ret += " %s" % tag_desc
        elif i:
            ret += "%d" % int_val
        elif f:
            ret += fp_to_string.fp_to_string("ed", int_val, 20)
        elif b:
            ret += fp_to_string.fp_to_binstring("ed", int_val)
        ret += "\n"
    ret += "\n"
    ret += "fcw = %d %d %d -- %d %d %d %d %d %d = %s\n" % (
       (fpu_control >> 12) & 1,
       (fpu_control >> 10) & 3,
       (fpu_control >> 8) & 3,
       (fpu_control >> 5) & 1,
       (fpu_control >> 4) & 1,
       (fpu_control >> 3) & 1,
       (fpu_control >> 2) & 1,
       (fpu_control >> 1) & 1,
       (fpu_control >> 0) & 1,
       hex_str(fpu_control, 8))
    ret += "      X R P    P U O Z D I\n"
    ret += "        C C    M M M M M M\n\n"

    ret += "fsw = %d %d %d %d %d %d %d %d %d %d %d %d %d %d = %s\n" % (
       (fpu_status >> 15) & 1,
       (fpu_status >> 14) & 1,
       (fpu_status >> 11) & 7,
       (fpu_status >> 10) & 1,
       (fpu_status >> 9) & 1,
       (fpu_status >> 8) & 1,
       (fpu_status >> 7) & 1,
       (fpu_status >> 6) & 1,
       (fpu_status >> 5) & 1,
       (fpu_status >> 4) & 1,
       (fpu_status >> 3) & 1,
       (fpu_status >> 2) & 1,
       (fpu_status >> 1) & 1,
       (fpu_status >> 0) & 1,
       hex_str(fpu_status, 8))
    ret += "      B C T C C C E S P U O Z D I\n"
    ret += "        3 O 2 1 0 S F E E E E E E\n"
    ret += "          P\n\n"

    ret += "ftw = %s\n" % hex_str(fpu_tag, 8)

    aa64 = cpu_has_longmode(obj)
    ret += "\nLast operation:\n"
    ret += "    Instruction  0x%04x:%s\n" % (fpu_env.last_instr_sel, local_string(fpu_env.last_instr_ptr, aa64))
    ret += "    Opcode       0x%x\n" % fpu_env.opc
    ret += "    Operand      0x%04x:%s\n" % (fpu_env.last_operand_sel, local_string(fpu_env.last_operand_ptr, aa64))
    return ret

def current_x86_mode(obj):
    exec_mode = obj.iface.x86_reg_access.get_exec_mode()
    xmode_info = obj.iface.x86_reg_access.get_xmode_info()

    if exec_mode == X86_Exec_Mode_Real:
        if xmode_info.cs_d:
            mode = "32-bit legacy real mode"
        else:
            mode = "16-bit legacy real mode"
    elif exec_mode == X86_Exec_Mode_V86:
        cr4 = obj.iface.x86_reg_access.get_cr(X86_Cr4)
        if cr4 & 1:
            mode = "legacy ev86 mode"
        else:
            mode = "legacy v86 mode"
    elif exec_mode == X86_Exec_Mode_Prot:
        if xmode_info.cs_d:
            cr4 = obj.iface.x86_reg_access.get_cr(X86_Cr4)
            if cr4 & (1<< 5):
                mode = "32-bit legacy protected mode with PAE"
            else:
                mode = "32-bit legacy protected mode"
        else:
            mode = "16-bit legacy protected mode"
    elif exec_mode == X86_Exec_Mode_Compat:
        if xmode_info.cs_d:
            mode = "32-bit compatibility mode"
        else:
            mode = "16-bit compatibility mode"
    elif exec_mode == X86_Exec_Mode_64:
        cr4 = obj.iface.x86_reg_access.get_cr(X86_Cr4)
        if cr4 & (1 << 12):
            mode = "5-level mode"
        else:
            mode = "4-level mode"

    if SIM_class_has_attribute(obj.classname, "in_smm") and SIM_get_attribute(obj, "in_smm"):
        mode += " (system management mode)"
    if SIM_class_has_attribute(obj.classname, "in_smx") and SIM_get_attribute(obj, "in_smx"):
        mode += " (in SMX operation)"
    if SIM_class_has_attribute(obj.classname, "vmx_mode"):
        if obj.vmx_mode == 1:
            mode += " (VMX root operation)"
        elif obj.vmx_mode == 2:
            mode += " (VMX non-root operation)"
    return mode

def get_pregs(obj):
    aa64 = cpu_has_longmode(obj)
    mode = current_x86_mode(obj)
    ret = mode + "\n"
    state = local_pending_exception(obj)
    if state != None:
        ret += state + "\n"
    def gpr(obj, n):
        return obj.iface.x86_reg_access.get_gpr(n)
    if aa64:
        num_gprs = get_num_gprs(obj)
        gprs = ["rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi"]
        for i in range(8, num_gprs):
            gprs.append("r" + str(i))
        num_rows = num_gprs // 2
        for i in range(num_rows):
            ret += "{:<3s} = 0x{:0>16x}".format(gprs[i], gpr(obj, i))
            ret += " " * 13
            j = i + num_rows
            ret += "{:<3s} = 0x{:0>16x}\n".format(gprs[j], gpr(obj, j))
        ret += "\n"
        ret += "rip = 0x%016x\n" % obj.iface.x86_reg_access.get_rip()
        ret += "\n"
        ret += get_eflags(obj)
    else:
        ret += "%s = 0x%s, %s = 0x%04x, %s = 0x%02x, %s = 0x%02x\n" % ("eax", hex_str(gpr(obj, 0),8), "ax", gpr(obj, 0) & 0xffff, "ah", (gpr(obj, 0) >> 8) & 0xff, "al", gpr(obj, 0) & 0xff)
        ret += "%s = 0x%s, %s = 0x%04x, %s = 0x%02x, %s = 0x%02x\n" % ("ecx", hex_str(gpr(obj, 1),8), "cx", gpr(obj, 1) & 0xffff, "ch", (gpr(obj, 1) >> 8) & 0xff, "cl", gpr(obj, 1) & 0xff)
        ret += "%s = 0x%s, %s = 0x%04x, %s = 0x%02x, %s = 0x%02x\n" % ("edx", hex_str(gpr(obj, 2),8), "dx", gpr(obj, 2) & 0xffff, "dh", (gpr(obj, 2) >> 8) & 0xff, "dl", gpr(obj, 2) & 0xff)
        ret += "%s = 0x%s, %s = 0x%04x, %s = 0x%02x, %s = 0x%02x\n" % ("ebx", hex_str(gpr(obj, 3),8), "bx", gpr(obj, 3) & 0xffff, "bh", (gpr(obj, 3) >> 8) & 0xff, "bl", gpr(obj, 3) & 0xff)
        ret += "%s = 0x%s, %s = 0x%04x\n" % ("esp", hex_str(gpr(obj, 4),8), "sp", gpr(obj, 4) & 0xffff)
        ret += "%s = 0x%s, %s = 0x%04x\n" % ("ebp", hex_str(gpr(obj, 5),8), "bp", gpr(obj, 5) & 0xffff)
        ret += "%s = 0x%s, %s = 0x%04x\n" % ("esi", hex_str(gpr(obj, 6),8), "si", gpr(obj, 6) & 0xffff)
        ret += "%s = 0x%s, %s = 0x%04x\n" % ("edi", hex_str(gpr(obj, 7),8), "di", gpr(obj, 7) & 0xffff)
        ret += "\n"
        ret += "eip = 0x%s\n" % hex_str(obj.iface.x86_reg_access.get_rip(),8)
        ret += "\n"
        ret += get_eflags(obj)

    intstate = obj.iface.x86_reg_access.get_interruptibility_state()
    if intstate != X86_Intstate_Not_Blocking:
        ret += "\n"
        if intstate == X86_Intstate_Blocking_INT_Sti:
            ret += "Interrupts temporarily masked after STI\n"
        elif intstate == X86_Intstate_Blocking_INT_Mov_Ss:
            ret += "Interrupts temporarily masked after SS write\n"
        elif intstate == X86_Intstate_Blocking_INIT:
            ret += "INIT signals are blocked\n"
        elif intstate == X86_Intstate_Blocking_SMI:
            ret += "SMI signals are blocked\n"
        elif intstate == X86_Intstate_Blocking_NMI:
            ret += "NMI signals are blocked\n"

    ret += get_uif(obj)
    return ret

def local_pregs(obj, a):
    ret = get_pregs(obj)
    if a:
        ret += get_remaining_regs(obj)
    return ret

def get_mxcsr_cmd(obj):
    has_mxcsr = cpu_has_mxcsr(obj)
    if not has_mxcsr: return ""
    mxcsr = obj.iface.x86_reg_access.get_mxcsr()
    ret = "mxcsr = %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d = 0x%08x\n" % (
        (mxcsr >> 15) & 1,
        (mxcsr >> 13) & 3,
        (mxcsr >> 12) & 1,
        (mxcsr >> 11) & 1,
        (mxcsr >> 10) & 1,
        (mxcsr >> 9) & 1,
        (mxcsr >> 8) & 1,
        (mxcsr >> 7) & 1,
        (mxcsr >> 6) & 1,
        (mxcsr >> 5) & 1,
        (mxcsr >> 4) & 1,
        (mxcsr >> 3) & 1,
        (mxcsr >> 2) & 1,
        (mxcsr >> 1) & 1,
        mxcsr & 1,
        mxcsr)
    ret += "        F R P U O Z D I D P U O Z D I\n"
    ret += "        Z C M M M M M M A E E E E E E\n"
    ret += "                        Z\n"
    return ret

def get_vreg_string(vreg, d, f, x, i, b):
    ret = ""
    for r in vreg:
        if f:
            if d:
                ret += fp_to_string.fp_to_string_fixed("d", r, 15, 23) + " "
            else:
                ret += fp_to_string.fp_to_string_fixed("s", r, 8, 16) + " "
        elif x:
            if d:
                ret += "%016x " % r
            else:
                ret += "%08x " % r
        elif i:
            if d:
                ret += "%20d " % r
            else:
                ret += "%10d " % r
        elif b:
            if d:
                ret += fp_to_string.fp_to_binstring("d", r)
            else:
                ret += fp_to_string.fp_to_binstring("s", r)
    return ret

def get_xmm_cmd(obj, s, d, f, x, i, b):
    if not cpu_has_xmm(obj):
        return "Processor does not support SSE."
    aa64 = cpu_has_longmode(obj)
    if f + x + i + b == 0:
        x = 1
    if s + d == 0:
        d = 1
    if f + x + i + b > 1:
        return "Only one flag of -f, -x, -i, -b can be given to the command"
    if s + d > 1:
        return "Only one flag of -s and -d can be given to the command"
    ret = ""

    for index in range(16 if aa64 else 8):
        xreg = obj.iface.x86_reg_access.get_xmm(index)
        ret += "xmm%2d = " % index
        if d:
            sub = (xreg.hi64, xreg.lo64)
        else:
            sub = ((xreg.hi64 >> 32) & 0xffffffff, xreg.hi64 & 0xffffffff, (xreg.lo64 >> 32) & 0xffffffff, xreg.lo64 & 0xffffffff)
        ret += get_vreg_string(sub, d, f, x, i, b) + "\n"
    return ret

def get_ymm_hi_cmd(obj, s, d, f, x, i, b):
    if f + x + i + b == 0:
        x = 1
    if s + d == 0:
        d = 1
    if f + x + i + b > 1:
        return "Only one flag of -f, -x, -i, -b can be given to the command"
    if s + d > 1:
        return "Only one flag of -s and -d can be given to the command"
    ret = ""
    for index in range(16):
        yreg = obj.iface.x86_reg_access.get_ymm(index)
        ret += "ymm_hi%2d = " % index
        if d:
            sub = (yreg.hhi64, yreg.hlo64)
        else:
            sub = ((yreg.hhi64 >> 32) & 0xffffffff, yreg.hhi64 & 0xffffffff, (yreg.hlo64 >> 32) & 0xffffffff, yreg.hlo64 & 0xffffffff)
        ret += get_vreg_string(sub, d, f, x, i, b) + "\n"
    return ret

def get_ymm_cmd(obj, s, d, f, x, i, b):
    if f + x + i + b == 0:
        x = 1
    if s + d == 0:
        d = 1
    if f + x + i + b > 1:
        return "Only one flag of -f, -x, -i, -b can be given to the command"
    if s + d > 1:
        return "Only one flag of -s and -d can be given to the command"
    ret = ""
    for index in range(16):
        yreg = obj.iface.x86_reg_access.get_ymm(index)
        ret += "ymm%2d = " % index
        if d:
            sub = (yreg.hhi64, yreg.lhi64, yreg.hlo64, yreg.llo64)
        else:
            sub = ((yreg.hhi64 >> 32) & 0xffffffff, yreg.hhi64 & 0xffffffff, (yreg.lhi64 >> 32) & 0xffffffff, yreg.lhi64 & 0xffffffff,
                 (yreg.hlo64 >> 32) & 0xffffffff, yreg.hlo64 & 0xffffffff, (yreg.llo64 >> 32) & 0xffffffff, yreg.llo64 & 0xffffffff)
        ret += get_vreg_string(sub, d, f, x, i, b) + "\n"
    return ret

def get_zmm_hi_cmd(obj, s, d, f, x, i, b):
    try:
        zmm_hi_regs = obj.zmm_h
    except AttributeError:
        return "Processor does not support ZMM."
    iter = 0
    if f + x + i + b == 0:
        x = 1
    if s + d == 0:
        d = 1
    if f + x + i + b > 1:
        return "Only one flag of -f, -x, -i, -b can be given to the command"
    if s + d > 1:
        return "Only one flag of -s and -d can be given to the command"
    ret = ""
    for reg in zmm_hi_regs:
        ret += "zmm_hi%2d = " % iter
        if d:
            sub = reg[::-1]
        else:
            sub = ((reg[3] >> 32) & 0xffffffff, reg[3] & 0xffffffff, (reg[2] >> 32) & 0xffffffff, reg[2] & 0xffffffff,
                (reg[1] >> 32) & 0xffffffff, reg[1] & 0xffffffff, (reg[0] >> 32) & 0xffffffff, reg[0] & 0xffffffff)
        ret += get_vreg_string(sub, d, f, x, i, b) + "\n"
        iter = iter + 1
    return ret

def get_zmm_cmd(obj, base, s, d, f, x, i, b):
    try:
        zmm_regs = obj.zmm[base:]
    except AttributeError:
        return "Processor does not support ZMM."
    iter = base
    if f + x + i + b == 0:
        x = 1
    if s + d == 0:
        d = 1
    if f + x + i + b > 1:
        return "Only one flag of -f, -x, -i, -b can be given to the command"
    if s + d > 1:
        return "Only one flag of -s and -d can be given to the command"
    ret = ""
    for reg in zmm_regs:
        ret += "zmm%2d = " % iter
        if d:
            sub = list(reg)[::-1]
        else:
            sub = ((reg[7] >> 32) & 0xffffffff, reg[7] & 0xffffffff, (reg[6] >> 32) & 0xffffffff, reg[6] & 0xffffffff,
                (reg[5] >> 32) & 0xffffffff, reg[5] & 0xffffffff, (reg[4] >> 32) & 0xffffffff, reg[4] & 0xffffffff,
                (reg[3] >> 32) & 0xffffffff, reg[3] & 0xffffffff, (reg[2] >> 32) & 0xffffffff, reg[2] & 0xffffffff,
                (reg[1] >> 32) & 0xffffffff, reg[1] & 0xffffffff, (reg[0] >> 32) & 0xffffffff, reg[0] & 0xffffffff)
        ret += get_vreg_string(sub, d, f, x, i, b) + "\n"
        iter = iter + 1
    return ret

def get_opmask_cmd(obj):
    try:
        opmask_regs = obj.k
    except AttributeError:
        return "Processor does not support OpMask."
    iter = 0
    ret = ""
    for reg in opmask_regs:
        ret += "k%1d = %016x\n" % (iter, reg)
        iter = iter + 1
    return ret

def pregs_xmm_cmd(obj, s, d, f, x, i, b):
    print(get_xmm_cmd(obj, s, d, f, x, i, b) + "\n" + get_mxcsr_cmd(obj))

def pregs_ymm_cmd(obj, s, d, f, x, i, b):
    print(get_ymm_cmd(obj, s, d, f, x, i, b) + "\n" + get_mxcsr_cmd(obj))

def pregs_zmm_cmd(obj, s, d, f, x, i, b):
    print(get_zmm_cmd(obj, 0, s, d, f, x, i, b) + "\n" + get_opmask_cmd(obj) + "\n" + get_mxcsr_cmd(obj))

#
# Register pseudo attributes
#

def get_lower_32bits(reg, obj, idx):
    value = SIM_get_attribute(obj, reg)
    return value & 0xFFFFFFFF

def set_lower_32bits(reg, obj, val, idx):
    value = 0
    try:
        value = SIM_get_attribute(obj, reg)
    except SimExc_Attribute:
        return Sim_Set_Attribute_Not_Found
    value = value & 0xFFFFFFFF00000000
    value = value | (val & 0xFFFFFFFF)
    return SIM_set_attribute(obj, reg, value)

def get_lower_16bits(reg, obj, idx):
    value = SIM_get_attribute(obj, reg)
    return value & 0xFFFF

def set_lower_16bits(reg, obj, val, idx):
    value = 0
    try:
        value = SIM_get_attribute(obj, reg)
    except SimExc_Attribute:
        return Sim_Set_Attribute_Not_Found
    value = value & 0xFFFFFFFFFFFF0000
    value = value | (val & 0xFFFF)
    return SIM_set_attribute(obj, reg, value)

def get_lower_8bits(reg, obj, idx):
    value = SIM_get_attribute(obj, reg)
    return value & 0xFF

def set_lower_8bits(reg, obj, val, idx):
    value = 0
    try:
        value = SIM_get_attribute(obj, reg)
    except SimExc_Attribute:
        return Sim_Set_Attribute_Not_Found
    value = value & 0xFFFFFFFFFFFFFF00
    value = value | (val & 0xFF)
    return SIM_set_attribute(obj, reg, value)

def get_mid_8bits(reg, obj, idx):
    value = SIM_get_attribute(obj, reg)
    return (value >> 8) & 0xFF

def set_mid_8bits(reg, obj, val, idx):
    value = 0
    try:
        value = SIM_get_attribute(obj, reg)
    except SimExc_Attribute:
        return Sim_Set_Attribute_Not_Found
    value = value & 0xFFFFFFFFFFFF00FF
    value = value | ((val & 0xFF) << 8)
    return SIM_set_attribute(obj, reg, value)

def register_x86_pseudo_attributes(callback_data, obj):
    try:
        SIM_get_interface(obj, "x86")
    except SimExc_Lookup:
        return
    class_name = obj.classname
    if SIM_class_has_attribute(class_name, "ah"):
        return
    has_64_bit_gprs = SIM_class_has_attribute(class_name, "rax")
    if has_64_bit_gprs:
        reg_l = "r"
    else:
        reg_l = "e"
    for reg in ("ax", "bx", "cx", "dx", "di", "si", "bp", "sp", "ip"):
        if has_64_bit_gprs:
            SIM_register_typed_attribute(class_name, "e" + reg,
                                         get_lower_32bits, "r" + reg,
                                         set_lower_32bits, "r" + reg,
                                         4, "i", None,
                                         "Register e" + reg
                                         + " (lower 32 bits of "
                                         + reg_l + reg + ").")
        SIM_register_typed_attribute(class_name, reg,
                                     get_lower_16bits, reg_l + reg,
                                     set_lower_16bits, reg_l + reg,
                                     4, "i", None,
                                     "Register " + reg + " (lower 16 bits of "
                                     + reg_l + reg + ").")

    for reg in ("a", "b", "c", "d"):
        SIM_register_typed_attribute(class_name, reg + "l",
                                     get_lower_8bits, reg_l + reg + "x",
                                     set_lower_8bits, reg_l + reg + "x",
                                     4, "i", None,
                                     "Register " + reg + "l (lower 8 bits of "
                                     + reg_l + reg + "x).")
        SIM_register_typed_attribute(class_name, reg + "h",
                                     get_mid_8bits, reg_l + reg + "x",
                                     set_mid_8bits, reg_l + reg + "x",
                                     4, "i", None,
                                     "Register " + reg + "h (bits 8-15 from "
                                     + reg_l + reg + "x).")
    if has_64_bit_gprs:
        for reg in ("di", "si", "bp", "sp"):
            SIM_register_typed_attribute(class_name, reg + "l",
                                         get_lower_8bits, "r" + reg,
                                         set_lower_8bits, "r" + reg,
                                         4, "i", None,
                                         "Register " + reg
                                         + "l (lower 8 bits of "
                                         + reg_l + reg + ").")
        for i in range(8, 32):
            reg = "r" + str(i)
            if not SIM_class_has_attribute(class_name, reg):
                continue
            SIM_register_typed_attribute(class_name, reg + "d",
                                         get_lower_32bits, reg,
                                         set_lower_32bits, reg,
                                         4, "i", None,
                                         "Register " + reg
                                         + "d (lower 32 bits of " + reg + ").")
            SIM_register_typed_attribute(class_name, reg + "w",
                                         get_lower_16bits, reg,
                                         set_lower_16bits, reg,
                                         4, "i", None,
                                         "Register " + reg
                                         + "w (lower 16 bits of " + reg + ").")
            SIM_register_typed_attribute(class_name, reg + "b",
                                         get_lower_8bits, reg,
                                         set_lower_8bits, reg,
                                         4, "i", None,
                                         "Register " + reg
                                         + "b (lower 8 bits of " + reg + ").")

# Register the register pseudo-attributes handler
SIM_hap_add_callback("Core_Conf_Object_Create", register_x86_pseudo_attributes, 0)

#
# Implement X86_HLT_Instr hap.
#
if 'hap_HLT' not in globals():
    hap_HLT = SIM_hap_add_type('X86_HLT_Instr',
                               '', '', None,
                               '''Triggered when HLT instruction is executed.''', 0)
def subscribe_cpu_as_own_cstate_notifier(callback_data, obj):
    try:
        SIM_get_interface(obj, "x86")
        # Some CPU models could be built with old variant of x86/commands.py or
        # `setup_processor_ui` function may not be called for some CPU classes.
        # That means that not every CPU object with `cstate_listeners` attribute
        # will implement `x86_cstate_notification` interface (its implementation
        # is added by `setup_processor_ui` in x86/commands.py).
        SIM_get_interface(obj, "x86_cstate_notification")
    except SimExc_Lookup:
        return
    class_name = obj.classname
    if not SIM_class_has_attribute(class_name, "cstate_listeners"):
        return
    # Subscribe the CPU to C-State changes reported by same CPU.
    obj.cstate_listeners = [obj] + obj.cstate_listeners[:]
def trigger_hlt_hap(obj, cpu, state, sub_state):
    if SIM_hap_is_active(hap_HLT):
        if state == 1 and sub_state == 0 and cpu.activity_state == X86_Activity_Hlt:
            SIM_hap_occurred_always(hap_HLT, cpu, 0, [])
SIM_hap_add_callback("Core_Conf_Object_Create", subscribe_cpu_as_own_cstate_notifier, 0)

def msrs_cmd(obj):
    try:
        msr_list = obj.iface.x86_msr.get_all_valid_numbers()
        if not msr_list:
            return
    except:
        return
    print_list = []
    for msr_id in msr_list:
        name = obj.iface.x86_msr.get_name(msr_id)
        get_val = obj.iface.x86_msr.get(msr_id, Sim_X86_Msr_Int_Register_Access)
        if get_val.status == Sim_X86_Msr_Ok:
            value = "0x%x" % get_val.value
        else:
            value = "get failed"
        print_list.append([name, "0x%x"%msr_id, value])
    def sort_by_msr_num(a, b):
        if int(a[1], 16) < int(b[1], 16):
            return -1
        elif int(a[1], 16) > int(b[1], 16):
            return 1
        else:
            return 0
    print_list.sort(key = cmp_to_key(sort_by_msr_num))
    print_columns([Just_Left, Just_Right, Just_Right],
                  [[ "MSR Name", "Index", "Value" ]] + print_list)

def descriptor_description(idt, low_word, high_word, long_mode=0, low_word_2=0, high_word_2=0):
    if high_word & (1 << 15):
        if idt and long_mode:
            sel = (low_word >> 16) & 0xffff
            offs = (low_word & 0xffff) | (high_word & 0xffff0000) | (low_word_2 << 32)
            if ((high_word >> 8) & 0x1f) == 0xe:
                return ("64-bit interrupt gate", sel, offs, high_word & 7)
            elif ((high_word >> 8) & 0x1f) == 0xf:
                return ("64-bit trap gate", sel, offs, high_word & 7)
        elif idt:
            if high_word & (1 << 11):
                bit = "32-bit"
            else:
                bit = "16-bit"
            sel = (low_word >> 16) & 0xffff
            offs = (low_word & 0xffff) | (high_word & 0xffff0000)
            if ((high_word >> 8) & 7) == 5:
                return ("task gate", sel, 0)
            elif ((high_word >> 8) & 7) == 6:
                return (bit + " interrupt gate", sel, offs)
            elif ((high_word >> 8) & 7) == 7:
                return (bit + " trap gate", sel, offs)
            else:
                return None
        elif high_word & (1 << 12):

            if high_word & (1 << 22):
                bit = "32-bit"
            elif (high_word & (1 << 21)) and (high_word & (1 << 11)) and long_mode:
                bit = "64-bit"
            else:
                bit = "16-bit"
            base = ((low_word >> 16) & 0xffff) | ((high_word & 0xff) << 16) | (high_word & 0xff000000)
            limit = (low_word & 0xffff) | (high_word & 0xf0000)
            if (high_word >> 23) & 1:
                limit = limit * 4 * 1024 + 4 * 1024 - 1
            # Code/data
            type = ("read-only", "read-only (a)", "read/write", "read/write (a)", "expand down, read-only", "expand down, read-only (a)", "expand-down, read/write", "expand down, read/write (a)", "execute-only", "execute-only (a)", "execute/read", "execute/read (a)", "conforming, execute-only", "conforming, execute-only (a)", "conforming, execute/read", "conforming, execute/read (a)")[(high_word >> 8) & 0xf]
            return ("%s %s" % (bit, type), base, limit)
        else:
            # System
            if long_mode and ((high_word >> 8) & 0xf) != 2 and ((high_word_2 >> 8) & 0x1f) != 0:
                return None
            base = ((low_word >> 16) & 0xffff) | ((high_word & 0xff) << 16) | (high_word & 0xff000000)
            if long_mode:
                base = base | (low_word_2 << 32)
            limit = (low_word & 0xffff) | (high_word & 0xf0000)
            if long_mode:
                type = ("reserved", "reserved", "LDT", "reserved", "reserved", "reserved", "reserved", "reserved", "reserved", "64-bit tss (avail)", "reserved", "64-bit tss (busy)", "64-bit call gate", "reserved", "64-bit interrupt gate", "64-bit trap gate")[(high_word >> 8) & 0xf]
            else:
                type = ("reserved", "16-bit tss (avail)", "LDT", "16-bit tss (busy)", "16-bit call gate", "task gate", "16-bit interrupt gate", "16-bit trap gate", "reserved", "32-bit tss (avail)", "reserved", "32-bit tss (busy)", "32-bit call gate", "reserved", "32-bit interrupt gate", "32-bit trap gate")[(high_word >> 8) & 0xf]
            return (type, base, limit)
    else:
        return None

def cpu_long_mode(obj):
    efer = 0
    if cpu_has_efer(obj):
        efer = cpu_get_efer(obj)
    return ((efer >> 10) & 1)

def word_from_bytes(bytes, index):
    return bytes[index] | (bytes[index+1] << 8) | (bytes[index+2] << 16) | (bytes[index+3] << 24)

def idt_cmd(obj):
    print("Processor %s IDT (%s)" % (obj.name, current_x86_mode(obj)))
    if obj.cr0 & 1:
        table_base = obj.idtr_base
        table_limit = obj.idtr_limit
        in_long_mode = cpu_long_mode(obj)
        if in_long_mode:
            descriptor_size = 16
        else:
            descriptor_size = 8
        print_ist = in_long_mode
        if table_limit >= 256 * descriptor_size:
            table_limit = 256 * descriptor_size - 1
        num_bytes = 0
        bytes = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        descs = []
        for i in range(table_limit + 1):
            phys_addr = table_translate(obj, table_base + i)
            bytes[num_bytes] = table_read(obj.iface.processor_info.get_physical_memory(), phys_addr, 1)
            num_bytes = num_bytes + 1
            if num_bytes == descriptor_size:
                low_word = word_from_bytes(bytes, 0)
                high_word = word_from_bytes(bytes, 4)
                upper_low_word = word_from_bytes(bytes, 8)
                upper_high_word = word_from_bytes(bytes, 12)
                desc = descriptor_description(1, low_word, high_word, in_long_mode, upper_low_word, upper_high_word)
                if desc != None:
                    dpl = (high_word >> 13) & 3
                    if len(desc) >= 4:
                        ist = desc[3]
                    else:
                        ist = "n/a"
                    if print_ist:
                        descs.append([i // descriptor_size, desc[0], "%x" % desc[1], "%x" % desc[2], dpl, ist])
                    else:
                        descs.append([i // descriptor_size, desc[0], "%x" % desc[1], "%x" % desc[2], dpl])
                num_bytes = 0
        if print_ist:
            print_columns([Just_Left, Just_Left, Just_Left, Just_Left, Just_Left, Just_Left],
                          [ [ "Vector", "Type", "Segment", "Offset", "DPL", "IST" ] ] + descs)
        else:
            print_columns([Just_Left, Just_Left, Just_Left, Just_Left, Just_Left],
                          [ [ "Vector", "Type", "Segment", "Offset", "DPL" ] ] + descs)
    else:
        descs = []
        for i in range(256):
            physmem = obj.iface.processor_info.get_physical_memory()
            offset = table_read(physmem, obj.idtr_base + i * 4, 2)
            selector = table_read(physmem, obj.idtr_base + i * 4 + 2, 2)
            descs.append([i, "%04x" % selector, "%04x" % offset])
        print_columns([Just_Left, Just_Left, Just_Left],
                      [ [ "Vector", "Segment", "Offset" ] ] + descs)

def smmseg_cmd(obj):
    mca_cap = obj.iface.x86_msr.get(0x17D, Sim_X86_Msr_Int_Register_Access)
    if mca_cap.status != Sim_X86_Msr_Ok:
        print("Failed to get SMM_MCA_CAP msr")
        return

    if mca_cap.value & (1 << 54):
        prot_mod_base = obj.iface.x86_msr.get(0x9D, Sim_X86_Msr_Int_Register_Access)
        if prot_mod_base.status != Sim_X86_Msr_Ok:
            print('Failed to get SMM_PROT_MOD_BASE msr')
            return
        if prot_mod_base.value & 1:
            smmseg_struct = (
                ("Reserved",              0,  4, "%d",   True),
                ("SMMSEG_FEATURE_ENABLE", 4,  4, "0x%x", True),
                ("GDTR Limit",            8,  4, "0x%x", True),
                ("GDTR Base Offset",      12, 4, "0x%x", True),
                ("CS Selector",           16, 4, "0x%x", True),
                ("Reserved",              20, 4, "%d",   True),
                ("ESP Offset",            24, 4, "0x%x", True),
                ("Reserved",              28, 4, "%d",   True),
                ("IDTR Limit",            32, 4, "0x%x", True),
                ("IDTR Base Offset",      36, 4, "0x%x", True))
            t = table_parser(obj, prot_mod_base.value ^ 1, smmseg_struct)
            t.print_table()
        else:
            print('Protected SMM not enabled')
    else:
        print("CPU doesn't have protected SMM capability")

def gdt_cmd(obj):
    if obj.cr0 & 1:
        print("Processor %s GDT (%s)" % (obj.name, current_x86_mode(obj)))
        table_base = obj.gdtr_base
        table_limit = obj.gdtr_limit
        num_bytes = 0
        bytes = [0, 0, 0, 0, 0, 0, 0, 0]
        prev_bytes = [1, 1, 1, 1, 1, 1, 1, 1]
        descs = []
        descriptor_failed = 0
        in_long_mode = cpu_long_mode(obj)
        last_selector = -1
        for i in range(table_limit+1):
            try:
                phys_addr = obj.iface.x86.linear_to_physical(Sim_DI_Data, table_base + i)
            except simics.SimExc_Memory:
                descriptor_failed = 1
            if not descriptor_failed:
                bytes[num_bytes] = table_read(obj.iface.processor_info.get_physical_memory(), phys_addr, 1)
            num_bytes = num_bytes + 1
            if num_bytes == 8:
                if not descriptor_failed:
                    low_word = word_from_bytes(bytes, 0)
                    high_word = word_from_bytes(bytes, 4)
                    desc = descriptor_description(0, low_word, high_word, in_long_mode, 0xffffffff, 0xffffffff)
                    if desc != None:
                        last_selector = i - 7
                        descs.append(["%04x" % last_selector, desc[0], "%x" % desc[1], "%x" % desc[2]])
                    elif in_long_mode:
                        low_2 = word_from_bytes(prev_bytes, 0)
                        high_2 = word_from_bytes(prev_bytes, 4)
                        desc = descriptor_description(0, low_2, high_2, in_long_mode, low_word, high_word)
                        if desc != None and last_selector != (i - 15):
                            last_selector = i - 15
                            descs.append(["%04x" % (i - 15), desc[0], "%x" % desc[1], "%x" % desc[2]])
                num_bytes = 0
                descriptor_failed = 0
                prev_bytes = bytes[:]
        print_columns([Just_Left, Just_Left, Just_Left, Just_Left],
                      [ [ "Selector", "Type", "Base", "Limit" ] ] + descs)
    else:
        print("Processor %s is in %s, GDT not available" % (obj.name, current_x86_mode(obj)))

####### Helpers #######

# converts (byte0_int, byte1_int, ...) to int
def as_value(t):
    return int.from_bytes(bytes(t), "little")

def table_translate(cpu, linear_addr):
    try:
        return cpu.iface.x86.linear_to_physical(Sim_DI_Data, linear_addr)
    except SimExc_Memory:
        raise CliError("Failed to translate linear address 0x%x" % linear_addr)

def table_read(mem_space, addr, size):
    try:
        return as_value(mem_space.iface.memory_space.read(None, addr, size, 1))
    except SimExc_Memory:
        raise CliError("Failed to read memory at 0x%x" % addr)

def get_bytes(cpu, addr, l):
    s = b""
    space = cpu.physical_memory
    for i in range(l):
        c = table_read(space, addr + i, 1)
        s += b"%c" % c
    return s

def calc_sum(cpu, start, length):
    space = cpu.physical_memory
    s = 0
    for i in range(0, length):
        s += table_read(space, start + i, 1)
    return s & 0xff

def int_val(name, offset, length):
    return (name, offset, length, "%d", True)

def hex_val(name, offset, length):
    return (name, offset, length, "0x%x", True)

def str_val(name, offset, length):
    return (name, offset, length, "%s", False)

def ga_val(name, offset):
    return hex_val(name, offset, 12)

def checksum(cpu, ptr, length, offset):
    if length > 0x1000:
        chk_sum_status = "NOT CHECKED"
    elif calc_sum(cpu, ptr, length):
        chk_sum_status = "ILLEGAL"
    else:
        chk_sum_status = "Ok"
    return ("Checksum", offset, 1, "0x%%x (%s)" % chk_sum_status, True)

class table_parser:
    def __init__(self, cpu, address, table_description, table_length=None):
        self.table_description = []
        self.table_length = None
        self.add_fields(cpu, address, table_description, table_length)

    def fits_field(self, offset, length):
        if self.table_length == None:
            return True
        if offset + length <= self.table_length:
            return True
        return False

    def add_fields(self, cpu, address, more_table_description, table_length=None):
        if not self.table_length or table_length > self.table_length:
            self.table_length = table_length
        space = cpu.physical_memory
        for (name, offset, length, syntax, is_int) in more_table_description:
            if self.fits_field(offset, length):
                if is_int:
                    v = table_read(space, address + offset, length)
                else:
                    v = get_bytes(cpu, address + offset, length).decode(
                        "ascii", "backslashreplace")
                object.__setattr__(self, name, v)
        self.table_description = self.table_description + list(more_table_description)

    def print_table(self):
        for (name, offset, length, syntax, is_int) in self.table_description:
            if self.fits_field(offset, length):
                print(("    %s " % name) + (syntax % self.__getattribute__(name)))

    def print_compact(self):
        for (name, offset, length, syntax, is_int) in self.table_description:
            if self.fits_field(offset, length):
                print(("%s=" % name), end=' ')
                print((syntax % self.__getattribute__(name)), end=' ')

def print_entry(entry_name, t):
    print("    ", end=' ')
    print(entry_name, end=' ')
    t.print_compact()
    print()

def get_scan_areas(cpu):
    space = cpu.physical_memory
    ebda_seg_pointer = 0x040e
    ebda_seg = table_read(space, ebda_seg_pointer, 2)
    to_scan = []
    if ebda_seg:
        to_scan.append(["EBDA", ebda_seg * 16, 1024])

    tom_pointer = 0x0413
    tom_seg = table_read(space, tom_pointer, 2)
    if tom_seg:
        tom_seg -= 1
        to_scan.append(["%dK" % tom_seg, tom_seg * 1024, 1024])

    if tom_seg != 639:
        to_scan.append(["639K", 639*1024, 1024])

    to_scan.append(["BIOS 0xf0000", 0xf0000, 0x10000])
    to_scan.append(["BIOS 0xe0000", 0xe0000, 0x10000])
    return to_scan

###### MP tables ######

def scan_for_mp_pointer(cpu, addr, length):
    space = cpu.physical_memory
    for o in range(0,length,16):
        v = table_read(space, addr + o, 4)
        if v == 0x5f504d5f: # _MP_
            if calc_sum(cpu, addr + o, 16) == 0:
                return addr + o
            else:
                print("MP pointer with illegal checksum @ 0x%x" % (addr + o))

def scan_mp_pointer(cpu):
    for (description, addr, length) in get_scan_areas(cpu):
        ptr = scan_for_mp_pointer(cpu, addr, length)
        if ptr:
            print("MP pointer found in %s region" % description)
            return ptr

    return None

def scan_mp_table(cpu):
    ptr = scan_mp_pointer(cpu)
    if not ptr:
        print("MP root pointer not found")
        return

    print("MP pointer @ 0x%x" % ptr)

    mp_ptr_struct = (
        ("Pointer", 4, 4, "0x%x", True),
        ("Length", 8, 1, "%d", True),
        ("Revision", 9, 1, "%d", True),
        checksum(cpu, ptr, 16, 10))

    t = table_parser(cpu, ptr, mp_ptr_struct)
    t.print_table()

    if not t.Pointer:
        print("MP configuration table header missing")
        return

    print("MP configuration table header at 0x%x" % t.Pointer)

    dump_mp_conf_table(cpu, t.Pointer)
    print()

def dump_mp_conf_table(cpu, pap):
    space = cpu.physical_memory

    mp_conf_struct_1 = (
        ("Signature", 0, 4, "0x%x", True),
        ("Base_table_length", 4, 2, "%d", True),
        ("Spec_rev", 6, 1, "%d", True))
    t1 = table_parser(cpu, pap, mp_conf_struct_1)

    mp_conf_struct_2 = (
        checksum(cpu, pap, t1.Base_table_length, 7),
        ("OEM_ID", 8, 8, "%s", False),
        ("Product_ID", 16, 12, "%s", False),
        ("OEM_table_ptr", 0x1c, 4, "0x%x", True),
        ("OEM_table_size", 0x20, 2, "%d", True),
        ("Entry_count", 0x22, 2, "%d", True),
        ("APIC_address", 0x24, 4, "0x%x", True),
        ("Extended_length", 0x28, 2, "%d", True),
        ("Extended_checksum", 0x2a, 2, "0x%x", True))
    t2 = table_parser(cpu, pap, mp_conf_struct_2)

    t1.print_table()
    t2.print_table()

    offset = 0x2c

    while offset < t1.Base_table_length:
        print("Entry at 0x%x" % offset)
        entry_type = table_read(space, pap + offset, 1)

        if entry_type == 0: # CPU
            entry_struct = (
                int_val("id", 1, 1),
                int_val("version", 2, 1),
                hex_val("flags", 3, 1),
                hex_val("signature", 4, 4),
                hex_val("features", 8, 4))
            t = table_parser(cpu, pap + offset, entry_struct)
            print_entry("CPU", t)
            offset += 20
        elif entry_type == 1: # BUS
            entry_struct = (
                int_val("id", 1, 1),
                str_val("type", 2, 6))
            t = table_parser(cpu, pap + offset, entry_struct)
            print_entry("BUS", t)
            offset += 8
        elif entry_type == 2: # I/O APIC
            entry_struct = (
                int_val("id", 1, 1),
                int_val("version", 2, 1),
                hex_val("flags", 3, 1),
                hex_val("addr", 4, 4))
            t = table_parser(cpu, pap + offset, entry_struct)
            print_entry("I/O-APIC", t)
            offset += 8
        elif entry_type == 3: # Interrupt entry
            entry_struct = (
                int_val("type", 1, 1),
                hex_val("flags", 2, 2),
                int_val("src_bus", 4, 1),
                int_val("src_irq", 5, 1),
                int_val("dst_ioapic", 6, 1),
                int_val("dst_ioapic_pin", 7, 1))
            t = table_parser(cpu, pap + offset, entry_struct)
            print_entry("IRQ", t)
            offset += 8
        elif entry_type == 4: # Local interrupt
            entry_struct = (
                int_val("type", 1, 1),
                hex_val("flags", 2, 2),
                int_val("src_bus", 4, 1),
                int_val("src_irq", 5, 1),
                int_val("dst_apic", 6, 1),
                int_val("dst_apic_lint", 7, 1))
            t = table_parser(cpu, pap + offset, entry_struct)
            print_entry("Local IRQ", t)
            offset += 8
        else:
            print("Unknown entry type %d" % entry_type)
            return

###### ACPI tables ######

def scan_for_rsdp(cpu, addr, length):
    space = cpu.physical_memory

    rsdp_signature = 0
    for c in 'RSD PTR '[::-1]: # s[::-1] reverses the string
        rsdp_signature = rsdp_signature << 8
        rsdp_signature |= ord(c)

    for o in range(0,length,8):
        v = table_read(space, addr + o, 8)
        if v == rsdp_signature:
            if calc_sum(cpu, addr + o, 20) == 0:
                return addr + o
            else:
                print("RSDP with illegal checksum @ 0x%x" % (addr + o))

def scan_rsdp(cpu):
    for (description, addr, length) in get_scan_areas(cpu):
        ptr = scan_for_rsdp(cpu, addr, length)
        if ptr:
            print("RSDP found in %s region" % description)
            return ptr
    return None

def scan_description_header(cpu, addr, do_print):
    header_struct_1 = (
        str_val("Signature", 0, 4),
        int_val("Length", 4, 4),
        int_val("Revision", 8, 1))
    h = table_parser(cpu, addr, header_struct_1)
    header_struct_2 = (
        checksum(cpu, addr, h.Length, 9),
        str_val("OEM_ID", 10, 6),
        str_val("OEM_table_id", 16, 8),
        int_val("OEM_revision", 24, 4),
        int_val("Creator_id", 28, 4),
        int_val("Creator_revision", 32, 4))
    h.add_fields(cpu, addr, header_struct_2)
    if do_print:
        print("%s @ 0x%x (%d bytes)" % (h.Signature, addr, h.Length))
    h.print_table()
    return h

def print_table_hex(cpu, addr, length):
    space = cpu.physical_memory

    limit = 100
    if length > limit:
        simics.pr(f"  Limiting output to first {limit} bytes.\n")
        length = limit

    for i in range(length):
        if (i % 16) == 0:
            simics.pr("  %04x: " % i)
        simics.pr("%02x " % table_read(space, addr + i, 1))
        if (i % 16) == 15 or i == length - 1:
            simics.pr("\n")

def scan_uefi_system_table(cpu):
    # See "Unified Extensible Firmware Interface (UEFI) Specification",
    # section "EFI System Table Location" for more information.

    import binascii
    EFI_SYSTEM_TABLE_SIGNATURE = 0x5453595320494249

    space = cpu.physical_memory

    # 4Mb boundary
    boundary_4Mb = (1024 * 1024 * 4)
    boundary_4Gb = (1024 * 1024 * 1024 * 4)
    addr = boundary_4Mb

    # Look for EFI EFI_SYSTEM_TABLE_POINTER:
    # typedef struct _EFI_SYSTEM_TABLE_POINTER {
    #   UINT64                    Signature;
    #   EFI_PHYSICAL_ADDRESS      EfiSystemTableBase;
    #   UINT32                    Crc32;
    # } EFI_SYSTEM_TABLE_POINTER;
    #
    # It may be found searching for the EFI_SYSTEM_TABLE_SIGNATURE on each 4M
    # boundary starting at the top of memory and scanning down.

    ptr = None
    while addr < boundary_4Gb:  # TODO: scan from "the top of memory"
        try:
            signature = table_read(space, addr, 8)
            if signature == EFI_SYSTEM_TABLE_SIGNATURE:
                crc32 = table_read(space, addr + 16, 4)
                data = (get_bytes(cpu, addr, 16)
                        # CRC is calculated assuming that CRC32 field is zero.
                        + b'\0\0\0\0'
                        # NB: count also 4-byte padding located at the end.
                        + get_bytes(cpu, addr + 20, 4))
                crc = binascii.crc32(data)
                if crc32 == crc:
                    ptr = table_read(space, addr + 8, 8)
                    break
        except CliError:
            pass
        addr += boundary_4Mb

    if ptr is None:
        return

    # See section "EFI Table Header":
    #
    # typedef struct {
    #   UINT64  Signature;
    #   UINT32  Revision;
    #   UINT32  HeaderSize;
    #   UINT32  CRC32;
    #   UINT32  Reserved;
    # } EFI_TABLE_HEADER;

    signature = table_read(space, ptr, 8)
    if signature != EFI_SYSTEM_TABLE_SIGNATURE:
        return

    header_size = table_read(space, ptr + 12, 4)
    crc32 = table_read(space, ptr + 16, 4)
    data = (get_bytes(cpu, ptr, 16)
            # CRC is calculated assuming that CRC32 field is zero.
            + b'\0\0\0\0'
            + get_bytes(cpu, ptr + 20, header_size - 20))
    crc = binascii.crc32(data)

    if crc32 != crc:
        print(f"Warning: EFI_SYSTEM_TABLE checksum mismatch at {ptr:#x}:"
              f" checksum from EFI_SYSTEM_TABLE - {crc32:#x},"
              f" actual checksum - {crc:#x}")

    # See "EFI System Table"
    # typedef struct {
    #   EFI_TABLE_HEADER                  Hdr;
    #   CHAR16                            *FirmwareVendor;
    #   UINT32                            FirmwareRevision;
    #   EFI_HANDLE                        ConsoleInHandle;
    #   EFI_SIMPLE_TEXT_INPUT_PROTOCOL    *ConIn;
    #   EFI_HANDLE                        ConsoleOutHandle;
    #   EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   *ConOut;
    #   EFI_HANDLE                        StandardErrorHandle;
    #   EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   *StdErr;
    #   EFI_RUNTIME_SERVICES              *RuntimeServices;
    #   EFI_BOOT_SERVICES                 *BootServices;
    #   UINTN                             NumberOfTableEntries;
    #   EFI_CONFIGURATION_TABLE           *ConfigurationTable;
    # } EFI_SYSTEM_TABLE;
    num_offset = (
        # table_header + *FirmwareVendor
          24           + 8
        # + FirmwareRevision (uin32 but 8 due to alignment of the next member)
          + 8
        # + ConsoleInHandle (EFI_HANDLE is void*)
          + 8
        # + *ConIn + ConsoleOutHandle + *ConOut + StandardErrorHandle + *StdErr
          + 8      + 8                + 8       + 8                   + 8
        # + *RuntimeServices + *BootServices
          + 8                + 8)
    NumberOfTableEntries = table_read(space, ptr + num_offset, 8)
    ConfigurationTable = table_read(space, ptr + num_offset + 8, 8)

    if NumberOfTableEntries == 0 or ConfigurationTable == 0:
        return

    try:
        # It looks like ConfigurationTable may become a virtual address once
        # the control is passed to OS (see SetVirtualAddressMap in UEFI spec.)
        # and virtual to physical address translation may not be available.
        # So here we try to fail gracefully.
        read_guid(space, ConfigurationTable)
    except CliError:
        print("Found UEFI map but failed to read UEFI ConfigurationTable."
              " Most likely the control was passed to OS. Please try"
              " to print ACPI table before the control is passed"
              " to the OS or dump the table from the OS.")
        return

    for i in range(NumberOfTableEntries):
        # See "EFI Configuration Table & Properties Table"
        # typedef struct{
        #   EFI_GUID           VendorGuid;
        #   VOID               *VendorTable;
        # } EFI_CONFIGURATION_TABLE;
        guid = read_guid(space, ConfigurationTable + 24 * i)
        ptr = table_read(space, ConfigurationTable + 16 + 24 * i, 8)

        if guid == 'EB9D2D30-2D88-11D3-9A16-0090273FC14D':
            # Found a pointer to the ACPI 1.0 specification RSDP structure
            return ptr
        elif guid == '8868E871-E4F1-11D3-BC22-0080C73C8881':
            # Found a pointer to the ACPI 2.0 or later specification RSDP structure
            return ptr

def read_guid(space, ptr):
    Data1 = table_read(space, ptr, 4)
    Data2 = table_read(space, ptr + 4, 2)
    Data3 = table_read(space, ptr + 6, 2)

    Data4 = [
        table_read(space, ptr + 8, 1),
        table_read(space, ptr + 9, 1),
        table_read(space, ptr + 10, 1),
        table_read(space, ptr + 11, 1),
        table_read(space, ptr + 12, 1),
        table_read(space, ptr + 13, 1),
        table_read(space, ptr + 14, 1),
        table_read(space, ptr + 15, 1),
        ]
    return ('%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x' % (
        Data1, Data2, Data3,
        Data4[0], Data4[1], Data4[2], Data4[3],
        Data4[4], Data4[5], Data4[6], Data4[7])).upper()

def scan_acpi_table(cpu, dsdt_filename, ssdt_filename):
    space = cpu.physical_memory
    ptr = scan_rsdp(cpu)
    if ptr:
        print("RSDP address was found by searching legacy PC BIOS map.")
    else:
        print("RSDP address was not found by searching legacy PC BIOS map.")

        ptr = scan_uefi_system_table(cpu)
        if not ptr:
            print("RSDP address was not found by searching UEFI map.")
            print("Most likely the control was passed to OS. Please try"
                  " to print ACPI table before the control is passed"
                  " to the OS or dump the table from the OS.")
            return
        else:
            print("RSDP address was found by searching UEFI map.")

    # Tables RSDP, RSDT, APIC, FACP, FACS completely parsed. Raw data
    # from tables DSDT and SSDT are either dumped to screen or sent to
    # file. Other tables are hex-dumped to screen.

    print("RSDP @ 0x%x" % ptr)

    rsdt_addr = table_read(space, ptr + 16, 4)
    xsdt_addr = table_read(space, ptr + 24, 8)
    print("   RSDT @ 0x%x" % rsdt_addr)
    print("   XSDT @ 0x%x" % xsdt_addr)

    rsdt_hdr = scan_description_header(cpu, rsdt_addr, True)
    for table_ptr in range(rsdt_addr + 36, rsdt_addr + rsdt_hdr.Length, 4):
        table_addr = table_read(space, table_ptr, 4)
        hdr = scan_description_header(cpu, table_addr, True)

        if hdr.Signature == "FACP":
            facp_struct = (
                hex_val("FACS_addr", 36, 4),
                hex_val("DSDT_addr", 40, 4),
                hex_val("reserved1", 44, 1),
                int_val("Preferred_PM_profile", 45, 1),
                int_val("SCI_INT", 46, 2),
                hex_val("SMI_CMD", 48, 4),
                hex_val("ACPI_ENABLE", 52, 1),
                hex_val("ACPI_DISABLE", 53, 1),
                hex_val("S4BIOS_REQ", 54, 1),
                hex_val("PSTATE_CNT", 55, 1),
                hex_val("PM1a_EVT_BLK", 56, 4),
                hex_val("PM1b_EVT_BLK", 60, 4),
                hex_val("PM1a_CNT_BLK", 64, 4),
                hex_val("PM1b_CNT_BLK", 68, 4),
                hex_val("PM2_CNT_BLK", 72, 4),
                hex_val("PM_TMR_BLK", 76, 4),
                hex_val("GPE0_BLK", 80, 4),
                hex_val("GPE1_BLK", 84, 4),
                int_val("PM1_ENT_LEN", 88, 1),
                int_val("PM1_CNT_LEN", 89, 1),
                int_val("PM2_CNT_LEN", 90, 1),
                int_val("PM_TMR_LEN", 91, 1),
                int_val("GPE0_BLK_LEN", 92, 1),
                int_val("GPE1_BLK_LEN", 93, 1),
                int_val("GPE1_BASE", 94, 1),
                hex_val("CST_CNT", 95, 1),
                int_val("P_LVL2_LAT", 96, 2),
                int_val("P_LVL3_LAT", 98, 2),
                int_val("FLUSH_SIZE", 100, 2),
                int_val("FLUSH_STRIDE", 102, 2),
                int_val("DUTY_OFFSET", 104, 1),
                int_val("DUTY_WIDTH", 105, 1),
                int_val("DAY_ALRM", 106, 1),
                int_val("MON_ALRM", 107, 1),
                int_val("CENTURY", 108, 1),
                int_val("IA32_BOOT_ARCH", 109, 2),
                hex_val("reserved2", 111, 1),
                hex_val("Flags", 112, 4),
                ga_val("RESET_REG", 116),
                hex_val("RESET_VALUE", 128, 1),
                hex_val("reserved3", 129, 3),
                hex_val("X_FACS", 132, 8),
                hex_val("X_DSDT", 140, 8),
                ga_val("X_PM1a_EVT_BLK", 148),
                ga_val("X_PM1b_EVT_BLK", 160),
                ga_val("X_PM1a_CNT_BLK", 172),
                ga_val("X_PM1b_CNT_BLK", 184),
                ga_val("X_PM2_CNT_BLK", 196),
                ga_val("X_PM_TMR_BLK", 208),
                ga_val("X_GPE0_BLK", 220),
                ga_val("X_GPE1_BLK", 232))

            facp = table_parser(cpu, table_addr, facp_struct, hdr.Length)
            facp.print_table()

            facs_struct = (
                str_val("Signature", 0, 4),
                int_val("Length", 4, 4),
                hex_val("HardwareSignature", 8, 4),
                hex_val("Firmware_Waking_Vector", 12, 4),
                hex_val("Global_Lock", 16, 4),
                hex_val("Flags", 20, 4),
                hex_val("X_Firmware_Waking_Vector", 24, 8),
                int_val("Version", 32, 1),
                int_val("reserved", 33, 3),
                hex_val("OSPM_Flags", 36, 4))
            facs = table_parser(cpu, facp.FACS_addr, facs_struct)
            print("%s @ 0x%x (%d bytes)" % (facs.Signature, facp.FACS_addr, facs.Length))
            facs.print_table()

            dsdt_hdr = scan_description_header(cpu, facp.DSDT_addr, True)
            if dsdt_filename:
                f = open(dsdt_filename, "wb")
                for b in range(facp.DSDT_addr, facp.DSDT_addr + dsdt_hdr.Length, 1):
                    b_val = table_read(space, b, 1)
                    f.write(b"%c" % b_val)
                f.close()
                print("    Content written to '%s'" % dsdt_filename)
            else:
                print_table_hex(cpu, facp.DSDT_addr, dsdt_hdr.Length)

        elif hdr.Signature == "SSDT":
            if ssdt_filename:
                f = open(ssdt_filename, "wb")
                for b in range(table_addr, table_addr + hdr.Length, 1):
                    b_val = table_read(space, b, 1)
                    f.write(b"%c" % b_val)
                f.close()
                print("    Content written to '%s'" % ssdt_filename)
            else:
                print_table_hex(cpu, table_addr, hdr.Length)

        elif hdr.Signature == "APIC":
            table_read(space, table_addr + 36, 4)
            table_read(space, table_addr + 40, 4)
            ptr = 44
            while ptr < hdr.Length:
                entry_struct = (
                    int_val("Type", 0, 1),
                    int_val("Length", 1, 1))
                e = table_parser(cpu, table_addr + ptr, entry_struct)
                if e.Type == 0:
                    e.add_fields(cpu, table_addr + ptr, (
                            int_val("ACPI_id", 2, 1),
                            int_val("APIC_id", 3, 1),
                            hex_val("flags", 4, 4)))
                    print_entry("Local APIC", e)
                elif e.Type == 1:
                    e.add_fields(cpu, table_addr + ptr, (
                            int_val("id", 2, 1),
                            hex_val("addr", 4, 4),
                            int_val("gsi_base", 8, 4)))
                    print_entry("I/O-APIC", e)
                elif e.Type == 2:
                    e.add_fields(cpu, table_addr + ptr, (
                            int_val("bus", 2, 1),
                            int_val("src", 3, 1),
                            int_val("gsi", 4, 4),
                            hex_val("flags", 8, 2)))
                    print_entry("IRQ", e)
                elif e.Type == 3:
                    e.add_fields(cpu, table_addr + ptr, (
                            hex_val("flags", 2, 2),
                            int_val("gsi", 4, 4)))
                    print_entry("NMI", e)
                elif e.Type == 4:
                    e.add_fields(cpu, table_addr + ptr, (
                            int_val("APIC_id", 2, 1),
                            hex_val("flags", 3, 2),
                            int_val("lint", 5, 1)))
                    print_entry("Local APIC NMI", e)
                elif e.Type == 5:
                    e.add_fields(cpu, table_addr + ptr, (
                            hex_val("addr", 4, 8)))
                    print_entry("Local APIC Address", e)
                elif e.Type == 6:
                    e.add_fields(cpu, table_addr + ptr, (
                            int_val("id", 2, 1),
                            int_val("gsi_base", 4, 4),
                            hex_val("addr", 8, 8)))
                    print_entry("I/O SAPIC", e)
                elif e.Type == 7:
                    e.add_fields(cpu, table_addr + ptr, (
                            int_val("ACPI_id", 2, 1),
                            int_val("SAPIC_id", 3, 1),
                            int_val("SAPIC_id", 4, 1),
                            hex_val("flags", 8, 4),
                            int_val("uid", 12, 4),
                            str_val("uid_str", 16, e.Length - 16)))
                    print_entry("Local SAPIC", e)
                elif e.Type == 8:
                    e.add_fields(cpu, table_addr + ptr, (
                            hex_val("flags", 2, 2),
                            int_val("int_type", 4, 1),
                            int_val("dst_id", 5, 1),
                            int_val("dst_eid", 6, 1),
                            int_val("vector", 7, 1),
                            int_val("gsi", 8, 4),
                            hex_val("pis_flags", 12, 4)))
                    print_entry("Platform Interrupt Source", e)
                elif e.Type == 9:
                    e.add_fields(cpu, table_addr + ptr, (
                            int_val("id", 4, 4),
                            hex_val("flags", 8, 4),
                            int_val("ACPI_uid", 12, 4)))
                    print_entry("Local x2APIC", e)
                elif e.Type == 10:
                    e.add_fields(cpu, table_addr + ptr, (
                            hex_val("flags", 2, 2),
                            int_val("ACPI_uid", 4, 4),
                            int_val("lint", 8, 1)))
                    print_entry("Local x2APIC NMI", e)
                else:
                    print_entry("Unknown entry", e)
                ptr += e.Length

        else:
            print_table_hex(cpu, table_addr, hdr.Length)

    if xsdt_addr:
        scan_description_header(cpu, xsdt_addr, True)

def field0_sort(a, b):
    if a[0] > b[0]:
        return 1
    elif a[0] < b[0]:
        return -1
    else:
        return 0

def vmcs_cmd(obj, vmcs_ptr):
    if not SIM_class_has_attribute(obj.classname, "vmx_mode"):
        print("Processor does not support VMX")
        return
    if obj.vmx_mode == 0 and vmcs_ptr == None:
        print("Not in VMX operation mode and no pointer given")
        return

    if vmcs_ptr == None:
        vmcs_ptr = obj.current_vmcs_ptr

    if vmcs_ptr == 0xffffffffffffffff or (vmcs_ptr & 0xfff) != 0:
        print("Invalid VMCS pointer")
        return

    field_vals = []

    if vmcs_ptr in obj.active_vmcs:
        print("Active VMCS content from CPU registers and/or memory")

        vmcs_cache = dict([tuple(x) for x in obj.vmcs_cache])
        field_vals = vmcs_cache[vmcs_ptr]
    else:
        print("VMCS content from memory")

        for index in range(len(obj.vmcs_field)):
            (name, encoding, offset, size, attr) = obj.vmcs_field[index]
            if name == None:
                continue

            paddr = vmcs_ptr + offset
            value = table_read(obj.iface.processor_info.get_physical_memory(), paddr, size)
            field_vals.append((index, value))

    all_fields = []
    for (index, f) in field_vals:
        (name, encoding, offset, size, attr) = obj.vmcs_field[index]
        if size == 1:
            f = "0x%02x" % f
        elif size == 2:
            f = "0x%04x" % f
        elif size == 4:
            f = "0x%08x" % f
        elif size == 8:
            f = "0x%016x" % f
        all_fields.append([name, f])

    all_fields.sort(key = cmp_to_key(field0_sort))

    print_columns([Just_Left, Just_Left],
                  [ [ "Name", "Value" ] ] + all_fields)

def vmx_cap_cmd(obj):
    if not SIM_class_has_attribute(obj.classname, "vmx_mode") or \
    not (obj.iface.x86_cpuid_query.cpuid_query(1, 0).c & (1 << 5)):
        print("Processor does not support VMX")
        return

    def get_msr_basic(obj, msr_num):
        if not msr_num in obj.iface.x86_msr.get_all_valid_numbers():
            print("Error! No MSR {0:#x} present in CPU model".format(msr_num))
            return 0
        msr = obj.iface.x86_msr.get(msr_num, Sim_X86_Msr_Attribute_Access)
        if msr.status != Sim_X86_Msr_Ok:
            print("Error! Failed to get MSR {0:#x}".format(msr_num))
            return 0
        return msr.value
    get_msr = lambda num: get_msr_basic(obj = obj, msr_num = num)

    cr0Bits = [
        "PE",    #bit 0
        "MP",    #bit 1
        "EM",    #bit 2
        "TS",    #bit 3
        "ET",    #bit 4
        "NE",    #bit 5
        "rsvd",  #bit 6
        "rsvd",  #bit 7
        "rsvd",  #bit 8
        "rsvd",  #bit 9
        "rsvd",  #bit 10
        "rsvd",  #bit 11
        "rsvd",  #bit 12
        "rsvd",  #bit 13
        "rsvd",  #bit 14
        "rsvd",  #bit 15
        "WP",    #bit 16
        "rsvd",  #bit 17
        "AM",    #bit 18
        "rsvd",  #bit 19
        "rsvd",  #bit 20
        "rsvd",  #bit 21
        "rsvd",  #bit 22
        "rsvd",  #bit 23
        "rsvd",  #bit 24
        "rsvd",  #bit 25
        "rsvd",  #bit 26
        "rsvd",  #bit 27
        "rsvd",  #bit 28
        "NW",    #bit 29
        "CD",    #bit 30
        "PG"     #bit 31
    ]

    cr4Bits = [
        "VME",          #bit 0
        "PVI",          #bit 1
        "TSD",          #bit 2
        "DE",           #bit 3
        "PSE",          #bit 4
        "PAE",          #bit 5
        "MCE",          #bit 6
        "PGE",          #bit 7
        "PCE",          #bit 8
        "OSFXSR",       #bit 9
        "OSXMMEXCEPT",  #bit 10
        "UMIP",         #bit 11
        "LA57",         #bit 12
        "VMXE",         #bit 13
        "SMXE",         #bit 14
        "SEE",          #bit 15
        "FSGBASE",      #bit 16
        "PCIDE",        #bit 17
        "OSXSAVE",      #bit 18
        "rsvd",         #bit 19
        "SMEP",         #bit 20
        "SMAP",         #bit 21
        "PKE",          #bit 22
        "CET",          #bit 23
        "PKS",          #bit 24
        "rsvd",         #bit 25
        "rsvd",         #bit 26
        "rsvd",         #bit 27
        "LAM_SUP",      #bit 28
        "rsvd",         #bit 29
        "rsvd",         #bit 30
        "rsvd",         #bit 31
    ]


    VMX_MSR_BASE = 0x480

    VMX_BASIC_INFORMATION_MSR                            = (VMX_MSR_BASE + 0)
    VMX_EXECUTION_PIN_CONTROLS_MSR                       = (VMX_MSR_BASE + 1)
    VMX_EXECUTION_PROC_CONTROLS_MSR                      = (VMX_MSR_BASE + 2)
    VMX_EXIT_CONTROLS_MSR                                = (VMX_MSR_BASE + 3)
    VMX_ENTRY_CONTROLS_MSR                               = (VMX_MSR_BASE + 4)
    VMX_MISCELLANEOUS_DATA_MSR                           = (VMX_MSR_BASE + 5)
    VMX_EXECUTION_CR0_ZERO_ALLOWED_GUEST_HOST_MASK_MSR   = (VMX_MSR_BASE + 6)
    VMX_EXECUTION_CR0_ONE_ALLOWED_GUEST_HOST_MASK_MSR    = (VMX_MSR_BASE + 7)
    VMX_EXECUTION_CR4_ZERO_ALLOWED_GUEST_HOST_MASK_MSR   = (VMX_MSR_BASE + 8)
    VMX_EXECUTION_CR4_ONE_ALLOWED_GUEST_HOST_MASK_MSR    = (VMX_MSR_BASE + 9)
    VMX_VMCS_ENUMERATION_MSR                             = (VMX_MSR_BASE + 0xA)
    VMX_SECONDARY_EXECUTION_CONTROLS_MSR                 = (VMX_MSR_BASE + 0xB)
    VMX_VPID_EPT_CAP_MSR                                 = (VMX_MSR_BASE + 0xC)
    VMX_EXECUTION_TRUE_PIN_CONTROLS_MSR                  = (VMX_MSR_BASE + 0xD)
    VMX_EXECUTION_TRUE_PROC_CONTROLS_MSR                 = (VMX_MSR_BASE + 0xE)
    VMX_TRUE_EXIT_CONTROLS_MSR                           = (VMX_MSR_BASE + 0xF)
    VMX_TRUE_ENTRY_CONTROLS_MSR                          = (VMX_MSR_BASE + 0x10)
    VMX_VMFUNC_MSR                                       = (VMX_MSR_BASE + 0x11)

    class msr_obj_t:
        def __init__(self, num, val):
            self.num = num
            self.val = val
        def __getitem__(self, index):
            if isinstance(index, int):
                return (self.val >> index) & 1
            elif isinstance(index, slice):
                start, stop, step = index.indices(64)
                assert step == 1  # we don't support anything else
                mask = 2**(stop - start) -1
                return (self.val >> start) & mask
            else:
                raise TypeError("index must be int or slice")
        def __int__(self):
            return self.val
        def allowed(self, bit):
            return '%s %s' % ("" if self[bit] else "0",
                              "1" if self[32+bit] else "")

    #report information from IA32_VMX_BASIC MSR
    print("Basic VMX information")
    vmx_basic_msr = msr_obj_t(
        num = VMX_BASIC_INFORMATION_MSR,
        val = get_msr(VMX_BASIC_INFORMATION_MSR))

    print("IA32_VMX_BASIC {0:#x} MSR value: {1:#018x}".format(vmx_basic_msr.num, vmx_basic_msr.val))
    print("    VMCS revision: 0x%x\n"
          "    VMCS region size: 0x%x\n"
          "    physical address width: %s\n"
          "    dual monitor treatment support: %s\n"
          "    memory type used: %u\n"
          "    information in the VM-exit instruction-information field on VM exits due to INS and OUTS: %s\n"
          "    may any default1 VMX control be cleared to zero: %s\n" %
           (vmx_basic_msr[0:31],
            vmx_basic_msr[32:45],
            "32 bits" if vmx_basic_msr[48] else "equals to CPU physical-address width",
            "yes" if vmx_basic_msr[49] else "no",
            vmx_basic_msr[50:54],
            "yes" if vmx_basic_msr[54] else "no",
            "yes" if vmx_basic_msr[55] else "no",))

    isDefault1toNullSupported = vmx_basic_msr[55]

    #report information on Pin-Based VM-Execution Controls
    print("Pin-Based VM-Execution Controls")
    vmx_pinbased_ctls_msr = msr_obj_t(
        num = VMX_EXECUTION_PIN_CONTROLS_MSR,
        val = get_msr(VMX_EXECUTION_PIN_CONTROLS_MSR))

    print("IA32_VMX_PINBASED_CTLS ({0:#x}) MSR value: {1:#018x}".format(
        vmx_pinbased_ctls_msr.num,
        vmx_pinbased_ctls_msr.val
        ))
    if isDefault1toNullSupported:
        print("Read VMX features support information from IA32_VMX_TRUE_PINBASED_CTLS MSR")
        vmx_true_pinbased_ctls_msr = msr_obj_t(
            num = VMX_EXECUTION_TRUE_PIN_CONTROLS_MSR,
            val = get_msr(VMX_EXECUTION_TRUE_PIN_CONTROLS_MSR))

        print("IA32_VMX_TRUE_PINBASED_CTLS ({0:#x}) MSR value: {1:#018x}".format(
            vmx_true_pinbased_ctls_msr.num,
            vmx_true_pinbased_ctls_msr.val
            ))
        pinbased_ctls_to_use = vmx_true_pinbased_ctls_msr
    else:
        print("Read VMX features support information from IA32_VMX_PINBASED_CTLS MSR")
        pinbased_ctls_to_use = vmx_pinbased_ctls_msr

    print("Supported values")
    for bit,info in [(0, 'External-interrupt exiting'),
                     (3, 'NMI exiting'),
                     (5, 'Virtual NMIs'),
                     (6, 'Activate VMX-preemption timer'),
                     (7, 'Process posted interrupts')]:
        print("    %s: %s" % (info, pinbased_ctls_to_use.allowed(bit)))
    print()

    #report information on Primary Processor-Based VM-Execution Controls
    print("Primary Processor-Based VM-Execution Controls")
    vmx_procbased_ctls_msr = msr_obj_t(
        num = VMX_EXECUTION_PROC_CONTROLS_MSR,
        val = get_msr(VMX_EXECUTION_PROC_CONTROLS_MSR))

    print("IA32_VMX_PROCBASED_CTLS ({0:#x}) MSR value: {1:#018x}".format(
        vmx_procbased_ctls_msr.num,
        vmx_procbased_ctls_msr.val
        ))
    if isDefault1toNullSupported:
        print("Read VMX features support information from IA32_VMX_TRUE_PROCBASED_CTLS MSR")
        vmx_true_procbased_ctls_msr = msr_obj_t(
            num = VMX_EXECUTION_TRUE_PROC_CONTROLS_MSR,
            val = get_msr(VMX_EXECUTION_TRUE_PROC_CONTROLS_MSR))

        print("IA32_VMX_TRUE_PROCBASED_CTLS ({0:#x}) MSR value: {1:#018x}".format(
            vmx_true_procbased_ctls_msr.num,
            vmx_true_procbased_ctls_msr.val
            ))
        procbased_ctls_to_use = vmx_true_procbased_ctls_msr
    else:
        print("Read VMX features support information from IA32_VMX_PROCBASED_CTLS MSR")
        procbased_ctls_to_use = vmx_procbased_ctls_msr

    print('Supported values')
    for bit,info in [(2, 'interrupt-window exiting'),
                     (3, 'use TSC offsetting'),
                     (7, 'HLT exiting'),
                     (9, 'INVLPG exiting'),
                     (10, 'MWAIT exiting'),
                     (11, 'RDPMC exiting'),
                     (12, 'RDTSC exiting'),
                     (15, 'CR3-load exiting'),
                     (16, 'CR3-store exiting'),
                     (19, 'CR8-load exiting'),
                     (20, 'CR8-store exiting'),
                     (21, 'use TPR shadow'),
                     (22, 'NMI-window exiting'),
                     (23, 'MOV-DR exiting'),
                     (24, 'unconditional I/O exiting'),
                     (25, 'use I/O bitmaps'),
                     (27, 'monitor trap flag'),
                     (28, 'use MSR bitmaps'),
                     (29, 'MONITOR exiting'),
                     (30, 'PAUSE exiting'),
                     (31, 'activate secondary controls')]:
        print("    %s: %s" % (info, procbased_ctls_to_use.allowed(bit)))
    print()

    #report features controlled by Secondary Processor-Based VM-Execution Controls
    isSecCtlsSupported = ((procbased_ctls_to_use[32+31]) != 0)
    if not isSecCtlsSupported:
        print("Secondary processor-based vm-execution controls are not supported")
    else:
        print("Secondary Processor-Based VM-Execution Controls")
        vmx_procbased_ctls2_msr = msr_obj_t(
            num = VMX_SECONDARY_EXECUTION_CONTROLS_MSR,
            val = get_msr(VMX_SECONDARY_EXECUTION_CONTROLS_MSR))

        print("IA32_VMX_PROCBASED_CTLS2 ({0:#x}) MSR value: {1:#018x}".format(
            vmx_procbased_ctls2_msr.num,
            vmx_procbased_ctls2_msr.val
            ))
        print('Supported values')
        for bit,info in [(0, 'virtualize APIC accesses'),
                         (1, 'enable EPT'),
                         (2, 'descriptor-table exiting'),
                         (3, 'enable RDTSCP'),
                         (4, 'virtualize x2APIC mode'),
                         (5, 'enable VPID'),
                         (6, 'WBINVD exiting'),
                         (7, 'unrestricted guest'),
                         (8, 'APIC-register virtualization'),
                         (9, 'virtual-interrupt delivery'),
                         (10, 'PAUSE-loop exiting'),
                         (11, 'RDRAND exiting'),
                         (12, 'enable INVPCID'),
                         (13, 'enable VM functions'),
                         (14, 'enable VMCS shadowing'),
                         (15, 'enable ENCLS exiting'),
                         (16, 'enable RDSEED exiting'),
                         (17, 'enable PML'),
                         (18, 'enable EPT violation #VE'),
                         (19, 'conceal VMX non-root from PT'),
                         (20, 'enable XSAVES/XRSTORS'),
                         (22, 'enable mode-based execution for EPT'),
                         (23, 'enable SPP'),
                         (24, 'Intel PT uses guest physical addresses'),
                         (25, 'use TSC scaling'),
                         (26, 'enable user wait and pause'),
                         (28, 'enable ENCLV exiting')]:
            print('    %s: %s' % (info, vmx_procbased_ctls2_msr.allowed(bit)))
        print()

    #report information on VM-Exit Controls
    print("VM-Exit Controls")
    vmx_exit_ctls_msr = msr_obj_t(
        num = VMX_EXIT_CONTROLS_MSR,
        val = get_msr(VMX_EXIT_CONTROLS_MSR))

    print("IA32_VMX_EXIT_CTLS ({0:#x}) MSR value: {1:#018x}".format(
        vmx_exit_ctls_msr.num,
        vmx_exit_ctls_msr.val
        ))
    if isDefault1toNullSupported:
        print("Read VMX features support information from IA32_VMX_TRUE_EXIT_CTLS MSR")
        vmx_true_exit_ctls_msr = msr_obj_t(
            num = VMX_TRUE_EXIT_CONTROLS_MSR,
            val = get_msr(VMX_TRUE_EXIT_CONTROLS_MSR))

        print("IA32_VMX_TRUE_EXIT_CTLS ({0:#x}) MSR value: {1:#018x}".format(
            vmx_true_exit_ctls_msr.num,
            vmx_true_exit_ctls_msr.val
            ))
        exit_ctls_to_use = vmx_true_exit_ctls_msr
    else:
        print("Read VMX features support information from IA32_VMX_EXIT_CTLS MSR")
        exit_ctls_to_use = vmx_exit_ctls_msr

    print('Supported values')
    for bit,info in [(2, 'save debug controls'),
                     (9, 'host address-space size'),
                     (12, 'load IA32_PERF_GLOBAL_CTRL'),
                     (15, 'acknowledge interrupt on exit'),
                     (18, 'save IA32_PAT'),
                     (19, 'load IA32_PAT'),
                     (20, 'save IA32_EFER'),
                     (21, 'load IA32_EFER'),
                     (22, 'save VMX-preemption timer value'),
                     (23, 'clear IA32_BNDCFGS'),
                     (24, 'conceal VM-exits from PT'),
                     (25, 'clear IA32_RTIT_CTL'),
                     (28, 'load CET state'),
                     (29, 'load PKRS')]:
        print('    %s: %s' % (info, exit_ctls_to_use.allowed(bit)))
    print()

    #report information on VM-Entry Controls
    print("VM-Entry Controls")
    vmx_entry_ctls_msr = msr_obj_t(
        num = VMX_ENTRY_CONTROLS_MSR,
        val = get_msr(VMX_ENTRY_CONTROLS_MSR))

    print("IA32_VMX_ENTRY_CTLS ({0:#x}) MSR value: {1:#018x}".format(
        vmx_entry_ctls_msr.num,
        vmx_entry_ctls_msr.val
        ))
    if isDefault1toNullSupported:
        print("Read VMX features support information from IA32_VMX_TRUE_ENTRY_CTLS MSR\n")
        vmx_true_entry_ctls_msr = msr_obj_t(
            num = VMX_TRUE_ENTRY_CONTROLS_MSR,
            val = get_msr(VMX_TRUE_ENTRY_CONTROLS_MSR))

        print("IA32_VMX_TRUE_ENTRY_CTLS ({0:#x}) MSR value: {1:#018x}".format(
            vmx_true_entry_ctls_msr.num,
            vmx_true_entry_ctls_msr.val
            ))
        entry_ctls_to_use = vmx_true_entry_ctls_msr
    else:
        print("Read VMX features support information from IA32_VMX_ENTRY_CTLS MSR\n")
        entry_ctls_to_use = vmx_entry_ctls_msr

    print('Supported values')
    for bit,info in [(2, 'load debug controls'),
                     (9, 'IA-32e mode guest'),
                     (10, 'entry to SMM'),
                     (11, 'deactivate dual-monitor treatment'),
                     (13, 'load IA32_PERF_GLOBAL_CTRL'),
                     (14, 'load IA32_PAT'),
                     (15, 'load IA32_EFER'),
                     (16, 'load IA32_BNDCFGS'),
                     (17, 'conceal VM-entries from PT'),
                     (18, 'load IA32_RTIT_CTL'),
                     (20, 'load CET state'),
                     (22, 'load PKRS')]:
        print('    %s: %s' % (info, entry_ctls_to_use.allowed(bit)))
    print()

    print("Miscellaneous data")
    vmx_misc_msr = msr_obj_t(
        num = VMX_MISCELLANEOUS_DATA_MSR,
        val = get_msr(VMX_MISCELLANEOUS_DATA_MSR))

    print("IA32_VMX_MISC ({0:#x}) MSR value: {1:#018x}".format(
        vmx_misc_msr.num,
        vmx_misc_msr.val
        ))
    print("Detailed information:\n"
          "    the rate of the VMX-preemption timer relative to the TSC: 0x%x\n"
          "    VM exits store IA32_EFER.LMA into the \"IA-32e mode guest\" VM-entry control: %s\n"
          "    activity states support:\n"
          "        HLT: %s\n"
          "        shutdown: %s\n"
          "        wait-for-SIPI: %s\n"
          "    can RDMSR instruction be used in SMM to read the IA32_SMBASE MSR (MSR address 9EH): %s\n"
          "    the number of CR3-target values supported: %u\n"
          "    the recommended maximum number of MSRs that should appear in MSRs lists: %u (value of the field: %u)\n"
          "    IA32_SMM_MONITOR_CTL can be set to 1: %s\n"
          "    VMWRITE can write any supported field: %s\n"
          "    MSEG revision: 0x%x\n" %

          (vmx_misc_msr[0:5],
           "yes" if vmx_misc_msr[5] else "no",
           "yes" if vmx_misc_msr[6] else "no",
           "yes" if vmx_misc_msr[7] else "no",
           "yes" if vmx_misc_msr[8] else "no",
           "yes" if vmx_misc_msr[15] else "no",
           vmx_misc_msr[16:25],
           512*(vmx_misc_msr[25:28] + 1), vmx_misc_msr[25:28],
           "yes" if vmx_misc_msr[28] else "no",
           "yes" if vmx_misc_msr[29] else "no",
           vmx_misc_msr[32:64])
          )

    #report information from IA32_VMX_CR0_FIXED0, IA32_VMX_CR0_FIXED1, IA32_VMX_CR4_FIXED0, IA32_VMX_CR4_FIXED1 MSRs
    print("VMX-fixed bits in CR0 and CR4")
    vmx_cr0_fixed0_msr = msr_obj_t(
        num = VMX_EXECUTION_CR0_ZERO_ALLOWED_GUEST_HOST_MASK_MSR,
        val = get_msr(VMX_EXECUTION_CR0_ZERO_ALLOWED_GUEST_HOST_MASK_MSR))

    vmx_cr0_fixed1_msr = msr_obj_t(
        num = VMX_EXECUTION_CR0_ONE_ALLOWED_GUEST_HOST_MASK_MSR,
        val = get_msr(VMX_EXECUTION_CR0_ONE_ALLOWED_GUEST_HOST_MASK_MSR))

    vmx_cr4_fixed0_msr = msr_obj_t(
        num = VMX_EXECUTION_CR4_ZERO_ALLOWED_GUEST_HOST_MASK_MSR,
        val = get_msr(VMX_EXECUTION_CR4_ZERO_ALLOWED_GUEST_HOST_MASK_MSR))

    vmx_cr4_fixed1_msr = msr_obj_t(
        num = VMX_EXECUTION_CR4_ONE_ALLOWED_GUEST_HOST_MASK_MSR,
        val = get_msr(VMX_EXECUTION_CR4_ONE_ALLOWED_GUEST_HOST_MASK_MSR))

    print("IA32_VMX_CR0_FIXED0 ({0:#x}) MSR value: {1:#018x}.".format(vmx_cr0_fixed0_msr.num, vmx_cr0_fixed0_msr.val))
    print("IA32_VMX_CR0_FIXED1 ({0:#x}) MSR value: {1:#018x}.".format(vmx_cr0_fixed1_msr.num, vmx_cr0_fixed1_msr.val))
    print("IA32_VMX_CR4_FIXED0 ({0:#x}) MSR value: {1:#018x}.".format(vmx_cr4_fixed0_msr.num, vmx_cr4_fixed0_msr.val))
    print("IA32_VMX_CR4_FIXED1 ({0:#x}) MSR value: {1:#018x}.".format(vmx_cr4_fixed1_msr.num, vmx_cr4_fixed1_msr.val))

    print(" Bit # |  CR0|  is fixed |         CR4|  is fixed |")
    for i in range(0, 32):
        print("%6d |" % i, end=' ')
        print("%4s|"  % cr0Bits[i] if i < len(cr0Bits) else "rsvd", end=' ')
        if (vmx_cr0_fixed0_msr[i] == 0) and (vmx_cr0_fixed1_msr[i] == 0):
            print("   to 0   |", end=' ')
        elif (vmx_cr0_fixed0_msr[i] != 0) and (vmx_cr0_fixed1_msr[i] != 0):
            print("   to 1   |", end=' ')
        elif (vmx_cr0_fixed0_msr[i] == 0) and (vmx_cr0_fixed1_msr[i] != 0):
            print("   not    |", end=' ')
        else:
            print(("\n*** Oops, bug! Bits %d in IA32_VMX_CR0_FIXED0"
                " and IA32_VMX_CR0_FIXED1 are inconsistent\n" % i), end=' ')
        print("%11s|" % cr4Bits[i] if i < len(cr4Bits) else "rsvd", end=' ')
        if (vmx_cr4_fixed0_msr[i] == 0) and (vmx_cr4_fixed1_msr[i] == 0):
            print("   to 0   |")
        elif (vmx_cr4_fixed0_msr[i] != 0) and (vmx_cr4_fixed1_msr[i] != 0):
            print("   to 1   |")
        elif (vmx_cr4_fixed0_msr[i] == 0) and (vmx_cr4_fixed1_msr[i] != 0):
            print("   not    |")
        else:
            print(("\n*** Oops, bug! Bits %d in IA32_VMX_CR4_FIXED0"
                " and in IA32_VMX_CR4_FIXED1 are inconsistent\n" % i), end=' ')

    #report information from IA32_VMX_VMCS_ENUM MSR
    print("\nVMCS enumeration")
    vmx_vmcs_enum_msr = msr_obj_t(
        num = VMX_VMCS_ENUMERATION_MSR,
        val = get_msr(VMX_VMCS_ENUMERATION_MSR))

    print("IA32_VMX_VMCS_ENUM ({0:#x}) MSR value: {1:#018x}.".format(
        vmx_vmcs_enum_msr.num,
        vmx_vmcs_enum_msr.val
        ))

    print("The highest index value used for any VMCS encoding: %u\n" % (vmx_vmcs_enum_msr[1:10],))

    #report information from IA32_VMX_EPT_VPID_CAP MSR
    print("VPID and EPT capabilities")
    if not (procbased_ctls_to_use[63] and (vmx_procbased_ctls2_msr[33] or vmx_procbased_ctls2_msr[37])):
        print("CPU does not support VPID and EPT\n")
    else:
        vmx_ept_vpid_cap_msr = msr_obj_t(
            num = VMX_VPID_EPT_CAP_MSR,
            val = get_msr(VMX_VPID_EPT_CAP_MSR))

        print("IA32_VMX_EPT_VPID_CAP ({0:#x}) MSR value: {1:#018x}.".format(
            vmx_ept_vpid_cap_msr.num,
            vmx_ept_vpid_cap_msr.val
            ))
        print( "    EPT execute-only translation support: %s\n"
               "    support for EPT page-walk length 4: %s\n"
               "    EPT paging-structure uncacheable (UC) memory type support: %s\n"
               "    EPT paging-structure write-back (WB) memory type support: %s\n"
               "    EPT 2-Mbyte pages support: %s\n"
               "    EPT 1-Gbyte pages support: %s\n"
               "    accessed and dirty flags for EPT support: %s\n"
               "    INVEPT instruction support: %s"
               " (single-context: %s, all-context: %s)\n"
               "    INVVPID instruction support: %s"
               " (individual-address: %s, single-context: %s,"
               " all-context: %s, single-context-retaining-globals: %s)\n" %
               ("yes" if vmx_ept_vpid_cap_msr[0] else "no",
               "yes" if vmx_ept_vpid_cap_msr[6] else "no",
               "yes" if vmx_ept_vpid_cap_msr[8] else "no",
               "yes" if vmx_ept_vpid_cap_msr[14] else "no",
               "yes" if vmx_ept_vpid_cap_msr[16] else "no",
               "yes" if vmx_ept_vpid_cap_msr[17] else "no",
               "yes" if vmx_ept_vpid_cap_msr[21] else "no",
               "yes" if vmx_ept_vpid_cap_msr[20] else "no",
               "yes" if vmx_ept_vpid_cap_msr[25] else "no",  "yes" if vmx_ept_vpid_cap_msr[26] else "no",
               "yes" if vmx_ept_vpid_cap_msr[32] else "no",
               "yes" if vmx_ept_vpid_cap_msr[40] else "no",  "yes" if vmx_ept_vpid_cap_msr[41] else "no",
               "yes" if vmx_ept_vpid_cap_msr[42] else "no",  "yes" if vmx_ept_vpid_cap_msr[43] else "no",)
              )

    print("VM functions")
    if not (procbased_ctls_to_use[63] and (vmx_procbased_ctls2_msr[45])):
        print("CPU does not support VM functions\n")
    else:
        vmx_vmfunc_msr = msr_obj_t(
            num = VMX_VMFUNC_MSR,
            val = get_msr(VMX_VMFUNC_MSR))
        print("IA32_VMX_VMFUNC ({0:#x}) MSR value: {1:#018x}.".format(
            vmx_vmfunc_msr.num,
            vmx_vmfunc_msr.val))
        # TODO: split into individual VM-functions
        print("    Supported VM-functions mask: %#x\n" % vmx_vmfunc_msr.val)

    print('Supported VMCS fields')
    all_fields = []
    vmcs_fields = list(obj.vmcs_layout)
    vmcs_fields.sort(key = lambda x : x[0])
    for (index, name, size, offset, attr) in vmcs_fields:
        if (index & 1) == 1:
            # Skip the "high" fields
            continue
        def field_attr_string(attr):
            # Keep in sync with vmcs_field_attribute_t
            attr_str = []
            if attr & 1:
                attr_str.append('In Memory')
            if attr & 2:
                attr_str.append('In Register')
            if attr & 4:
                attr_str.append('Write on VM-exit')
            if attr & 8:
                attr_str.append('Read Only')
            return ', '.join(attr_str)
        all_fields.append([hex(index), name, size, hex(offset),
                           field_attr_string(attr)])
    print_columns([Just_Left, Just_Left, Just_Left, Just_Left, Just_Left],
                  [ [ 'Index', 'Name', 'Size', 'Offset', 'Attr' ] ] + all_fields)

def mtrr_type_name(encoding):
    if encoding == 0:
        return "uncacheable"
    elif encoding == 1:
        return "write-combining"
    elif encoding == 4:
        return "write-through"
    elif encoding == 5:
        return "write-protect"
    elif encoding == 6:
        return "write-back"
    else:
        return "unknown (0x%x)" % encoding

def memory_config_cmd(obj):
    if (SIM_class_has_attribute(obj.classname, "syscfg")):
        print("SYSCFG register")
        print("\tTop of memory 2 register: %s"
              % ("enabled" if obj.syscfg & (1 << 21) else "disabled"))
        print("\tTop of memory and I/O range registers: %s"
              % ("enabled" if obj.syscfg & (1 << 20) else "disabled"))
        print("\tRdDram and WrDram modification: %s"
               % ("enabled" if obj.syscfg & (1 << 19) else "disabled"))
        print("\tRdDram and WrDram attributes: %s"
               % ("enabled" if obj.syscfg & (1 << 18) else "disabled"))
    if (SIM_class_has_attribute(obj.classname, "hwcr")):
        print("HWCR register")
        print("\t32-bit address wrap: %s"
               % ("disabled" if obj.hwcr & (1 << 17) else "enabled"))
    if (SIM_class_has_attribute(obj.classname, "top_mem")):
        print("TOP_MEM register")
        print("\tTop of memory: 0x%x (%d Mb)" % (obj.top_mem, obj.top_mem // (1024 * 1024)))
    if (SIM_class_has_attribute(obj.classname, "top_mem2")):
        print("TOP_MEM2 register")
        print("\tTop of memory 2: 0x%x (%d Mb)" % (obj.top_mem2, obj.top_mem2 // (1024 * 1024)))
    if (SIM_class_has_attribute(obj.classname, "iorr_base0")):
        for i in range(2):
            valid = (obj.iorr_mask0 >> 11) & 1
            if valid:
                print("IORR%d valid" % i)
                print("\tBase: 0x%x" % (SIM_get_attribute(obj, "iorr_base%d" % i) & 0xfffffffffffff000))
                print("\tMask: 0x%x" % (SIM_get_attribute(obj, "iorr_mask%d" % i) & 0xfffffffffffff000))
                print("\tRead from: %s"
                      % ("DRAM" if SIM_get_attribute(obj, "iorr_base%d" % i)
                         & (1 << 4)
                        else "I/O"))
                print("\tWrite to: %s"
                      % ("DRAM" if SIM_get_attribute(obj, "iorr_base%d" % i)
                         & (1 << 3)
                          else "I/O"))
            else:
                print("IORR%d invalid" % i)
    if (SIM_class_has_attribute(obj.classname, "mtrr_def_type")):
        print("MTRRdefType register")
        print("\tMTRRs: %s"
              % ("enabled" if obj.mtrr_def_type & (1 << 11) else "disabled"))
        print("\tFixed range MTRRs: %s"
              % ("enabled" if obj.mtrr_def_type & (1 << 10) else "disabled"))
        print("\tDefault memory type: %s" % mtrr_type_name(obj.mtrr_def_type & 0xff))
    if (SIM_class_has_attribute(obj.classname, "mtrr_base0")):
        for i in range(8):
            valid = (SIM_get_attribute(obj, "mtrr_mask%d" % i) >> 11) & 1
            if valid:
                print("MTRR%d valid" % i)
                print("\tBase: 0x%x" % (SIM_get_attribute(obj, "mtrr_base%d" % i) & 0xfffffffffffff000))
                print("\tMask: 0x%x" % (SIM_get_attribute(obj, "mtrr_mask%d" % i) & 0xfffffffffffff000))
                print("\tType: %s" % mtrr_type_name(SIM_get_attribute(obj, "mtrr_base%d" % i) & 0xff))
            else:
                print("MTRR%d invalid" % i)
    if (SIM_class_has_attribute(obj.classname, "mtrr_fix_64k_00000")):
        print("Fixed MTRRs")
        for (sub_len, base) in ((64, 0x00000), (16, 0x80000), (16, 0xa0000), (4, 0xc0000), (4, 0xc8000), (4, 0xd0000), (4, 0xd8000), (4, 0xe0000), (4, 0xe8000), (4, 0xf0000), (4, 0xf8000)):
            mtrr_val = SIM_get_attribute(obj, "mtrr_fix_%dk_%05x" % (sub_len, base))
            for i in range(8):
                sub_field = (mtrr_val >> (i * 8)) & 0xff
                if (SIM_class_has_attribute(obj.classname, "syscfg")):
                    print("\t0x%08x - 0x%08x %s read: %s write: %s"
                          % (base + i * sub_len * 1024,
                             base + (i + 1) * sub_len * 1024 - 1,
                             mtrr_type_name(sub_field & 7),
                             "memory" if ((sub_field & (1 << 4))
                                          and (obj.syscfg & (1 << 18)))
                              else "I/O",
                             "memory" if ((sub_field & (1 << 3))
                                          and (obj.syscfg & (1 << 18)))
                              else "I/O"))
                else:
                    print("\t0x%08x - 0x%08x %s"
                          % (base + i * sub_len * 1024,
                             base + (i + 1) * sub_len * 1024 - 1,
                             mtrr_type_name(sub_field)))
    if (SIM_class_has_attribute(obj.classname, "smm_mask")):
        if obj.smm_mask & 1:
            print("SMM ASeg enabled")
            print("\tType: %s" % mtrr_type_name((obj.smm_mask >> 8) & 7))
        else:
            print("SMM ASeg disabled")
        if obj.smm_mask & 2:
            print("SMM TSeg enabled")
            print("\tType: %s" % mtrr_type_name((obj.smm_mask >> 12) & 7))
            print("\tBase: 0x%x" % obj.smm_base)
            print("\tMask: 0x%x" % obj.smm_mask)
        else:
            print("SMM TSeg disabled")


def read_linear(obj, lina, len):
    bytes = [0] * len
    for i in range(len):
        phys_addr = table_translate(obj, lina + i)
        bytes[i] = table_read(obj.iface.processor_info.get_physical_memory(), phys_addr, 1)
    if len == 1:
        return bytes[0]
    elif len == 2:
        return bytes[0] | (bytes[1] << 8)
    elif len == 4:
        return word_from_bytes(bytes, 0)
    elif len == 8:
        return word_from_bytes(bytes, 0) | (word_from_bytes(bytes, 4) << 32)

def tss_cmd(obj):
    print("Processor %s TSS (%s)" % (obj.name, current_x86_mode(obj)))
    tr = obj.tr
    tr = tr[:10]
    (sel, d, dpl, g, p, s, type, base, limit, valid) = tr

    if not valid:
        print("TSS not valid")

    in_long_mode = cpu_long_mode(obj)
    if in_long_mode and (type == 9 or type == 11):
        print("64-bit TSS")
        tss_fields = (("RSP0", 4 + 0*8, 8),
                      ("RSP1", 4 + 1*8, 8),
                      ("RSP2", 4 + 2*8, 8),
                      ("IST1", 28 + 1*8, 8),
                      ("IST2", 28 + 2*8, 8),
                      ("IST3", 28 + 3*8, 8),
                      ("IST4", 28 + 4*8, 8),
                      ("IST5", 28 + 5*8, 8),
                      ("IST6", 28 + 6*8, 8),
                      ("IST7", 28 + 7*8, 8),
                      ("I/O map base address", 102, 2))
    elif not in_long_mode and (type == 9 or type == 11):
        print("32-bit TSS")
        tss_fields = (("Previous task", 0, 2),
                      ("ESP0", 4, 4),
                      ("SS0", 8, 2),
                      ("ESP1", 12, 4),
                      ("SS1", 16, 2),
                      ("ESP2", 20, 4),
                      ("SS2", 24, 2),
                      ("CR3", 28, 4),
                      ("EIP", 32, 4),
                      ("EFLAGS", 36, 4),
                      ("EAX", 40, 4),
                      ("ECX", 44, 4),
                      ("EDX", 48, 4),
                      ("EBX", 52, 4),
                      ("ESP", 56, 4),
                      ("EBP", 60, 4),
                      ("ESI", 64, 4),
                      ("EDI", 68, 4),
                      ("ES", 72, 2),
                      ("CS", 76, 2),
                      ("SS", 80, 2),
                      ("DS", 84, 2),
                      ("FS", 88, 2),
                      ("GS", 92, 2),
                      ("LDT", 96, 2),
                      ("I/O map base address", 102, 2))
    elif not in_long_mode and (type == 1 or type == 3):
        print("16-bit TSS")
        tss_fields = (("Previous task", 0, 2),
                      ("SP0", 2, 2),
                      ("SS0", 4, 2),
                      ("SP1", 6, 2),
                      ("SS1", 8, 2),
                      ("SP2", 10, 2),
                      ("SS2", 12, 2),
                      ("IP", 14, 2),
                      ("FLAGS", 16, 2),
                      ("AX", 18, 2),
                      ("CX", 20, 2),
                      ("DX", 22, 2),
                      ("BX", 24, 2),
                      ("SP", 26, 2),
                      ("BP", 28, 2),
                      ("SI", 30, 2),
                      ("DI", 32, 2),
                      ("ES", 34, 2),
                      ("CS", 36, 2),
                      ("SS", 38, 2),
                      ("DS", 40, 2),
                      ("LDT", 42, 2))
    else:
        print("Cannot print TSS of type %d" % type)
        return

    descs = []
    for (name, offset, length) in tss_fields:
        val = read_linear(obj, base + offset, length)
        descs.append([name, "0x%x" % val])
    print_columns([Just_Left, Just_Left],
                  [ [ "Field", "Value" ] ] + descs)

def get_physical_address_mask(cpu):
    mask = (1 << cpu.physical_bits) - 1
    if cpu.vmx_mode == 2 and cpu.in_seam_mode:
        VMCS_EPT_PTR = 0x201a
        eptp = vmread(cpu, VMCS_EPT_PTR)
        levels = ((eptp & 0x38) >> 3) + 1
        if levels == 4:
            mask |= 1 << 47
    return mask

def parse_paging_entry(cpu, entry, entry_size):
    addr = entry & get_physical_address_mask(cpu) & 0x7ffffffffffff000
    if entry_size == 8:
        nx = (entry >> 63) & 1
    else:
        nx = 0
    p = (entry & 1)
    r_w = (entry >> 1) & 1
    u_s = (entry >> 2) & 1
    a = (entry >> 5) & 1
    d = (entry >> 6) & 1
    g = (entry >> 8) & 1
    ps_pat = (entry >> 7) & 1
    pkey = (entry >> 59) & 0xf
    return { "addr": addr, "p": p, "a": a, "d": d, "g": g, "nx": nx,
             "ps_pat": ps_pat, "r_w": r_w, "u_s": u_s,
             "pkey": pkey }

def vmread(cpu, enc):
    for (key, v) in cpu.vmcs_content:
        if key == enc:
            return v

def ept_tablewalk(cpu, gpa, indent_count):
    verbose = True
    VMCS_EPT_PTR = 0x201a
    indent = " " * indent_count

    def parse_ept_entry(cpu, entry, entry_size):
        phys_mask = (1 << cpu.physical_bits) - 1
        if cpu.physical_bits < 52:
            addr = entry & phys_mask & 0xfffffffff000
        else:
            addr = entry & phys_mask & 0x1fffffffffff000
        x = (entry >> 2) & 1
        w = (entry >> 1) & 1
        r = (entry >> 0) & 1
        large = (entry >> 7) & 1
        return { "addr": addr, "x": x, "w": w, "r": r, "large": large }

    eptp = vmread(cpu, VMCS_EPT_PTR)
    eptp_base = eptp & ~0xfff
    levels = ((eptp & 0x38) >> 3) + 1
    if levels == 5:
        # fields:   shift, name, large_bit, mask, page_size, size_name, rsvd_mask, rsvd_mask_large
        level_info = ((48, "EPT PML5E", 0, 0x1ff, None, None, 0xf8, 0xf8),
                      (39, "EPT PML4E", 0, 0x1ff, None, None, 0xf8, 0xf8),
                      (30, "EPT PDPTE", 0, 0x1ff, 1*1024*1024*1024, "1GiB", 0x78, 0x3ffff000),
                      (21, "EPT PDE", 1, 0x1ff, 2*1024*1024, "2MiB", 0x78, 0x1ff000),
                      (12, "EPT PTE", 0, 0x1ff, 4*1024, "4KiB", 0x0, 0x0))
    elif levels == 4:
        # fields:   shift, name, large_bit, mask, page_size, size_name, rsvd_mask, rsvd_mask_large
        level_info = ((39, "EPT PML4E", 0, 0x1ff, None, None, 0xf8, 0xf8),
                      (30, "EPT PDPTE", 0, 0x1ff, 1*1024*1024*1024, "1GiB", 0x78, 0x3ffff000),
                      (21, "EPT PDE", 1, 0x1ff, 2*1024*1024, "2MiB", 0x78, 0x1ff000),
                      (12, "EPT PTE", 0, 0x1ff, 4*1024, "4KiB", 0x0, 0x0))
    entry_size = 8
    ptr = eptp_base

    x = 1
    w = 1
    r = 1

    for level in range(levels):
        select = (gpa >> level_info[level][0]) & level_info[level][3]
        addr = ptr + select * entry_size
        if verbose:
            print("%s%s" % (indent, level_info[level][1]))
            print("%s    entry at 0x%x" % (indent, addr))

        entry = table_read(cpu.iface.processor_info.get_physical_memory(), addr, entry_size)
        if verbose:
            print("%s    entry 0x%x" % (indent, entry))

        parse_entry = parse_ept_entry(cpu, entry, entry_size)

        x &= parse_entry["x"]
        w &= parse_entry["w"]
        r &= parse_entry["r"]

        if verbose:
            print("%s    addr = 0x%x x = %d w = %d r = %d" % (indent, parse_entry["addr"], parse_entry["x"], parse_entry["w"], parse_entry["r"]))

        if not parse_entry["x"] and not parse_entry["w"] and not parse_entry["r"]:
            if verbose:
                print("%s    Not present" % (indent))
            return None

        rsvd_mask = level_info[level][7] if parse_entry["large"] else level_info[level][6]
        if entry & rsvd_mask != 0:
            print("%sReserved bits set: %#x" % (indent, (entry & rsvd_mask)))
        if level_info[level][4] != None:
            phys = parse_entry["addr"] + (gpa & (level_info[level][4] - 1))
        if parse_entry["large"]:
            if verbose:
                print("%s    %s page" % (indent, level_info[level][5]))
            return phys

        ptr = parse_entry["addr"]

    return phys

def cpu_in_non_root_ept(cpu):
    VMCS_PROC_BASED_VM_EXEC_CTRLS = 0x4002
    VMCS_SEC_PROC_BASED_CTLS = 0x401e

    if (hasattr(cpu, "vmx_mode") and cpu.vmx_mode == 2
        and (vmread(cpu, VMCS_PROC_BASED_VM_EXEC_CTRLS) & (1 << 31))
        and (vmread(cpu, VMCS_SEC_PROC_BASED_CTLS) & (1 << 1))):
        return True
    else: return False


def tablewalk_cmd(cpu, gpa, address):
    if gpa:
        ept_enabled = cpu_in_non_root_ept(cpu)
        if not ept_enabled:
            print("CPU is not using EPT, physical address: %#x" % address)
            return
        pa = ept_tablewalk(cpu, address, 0)
        if pa != None:
            print("Physical address: %#x" % pa)
    else:
        linear_to_physical_cmd(cpu, address)

def linear_to_physical_cmd(cpu, lina):
    true_pa_data = cpu.iface.x86.linear_to_physical(Sim_DI_Data, lina)
    true_pa_code = cpu.iface.x86.linear_to_physical(Sim_DI_Instruction, lina)
    if true_pa_data == -1: true_pa_data = None
    if true_pa_code == -1: true_pa_code = None

    if cpu_has_efer(cpu): efer = cpu_get_efer(cpu)
    else: efer = 0

    ept_enabled = cpu_in_non_root_ept(cpu)
    gpa = linear_to_physical_ex(cpu, lina, cpu.cr0, cpu.cr3, cpu.cr4, efer, ept_enabled)

    if gpa == None: return
    if ept_enabled: pa = ept_tablewalk(cpu, gpa, 0)
    else: pa = gpa

    if pa != None:
        print("Physical address: %#x" % pa)

    if (true_pa_data != true_pa_code or true_pa_data != pa):
        print("Warning: translation results from linear_to_physical are different.")
        print("PA for code: %s" % hex_str_or_none(true_pa_code))
        print("PA for data: %s" % hex_str_or_none(true_pa_data))
        print("This usually means that current CPU mode may not actually permit",\
              "access, or new page attributes are active.",\
              "Tablewalk output above may be incorrect.")

def mask_by_maxphyaddr(addr):
    max_physaddr_width = 52
    return addr & (bit(max_physaddr_width) - 1)

def linear_to_physical_ex(cpu, lina, cr0, cr3, cr4, efer, ept_enabled):
    verbose = True
    mode_long = 0
    mode_pae = 0
    mode_classic = 0
    mode_la57 = 0
    paging = (cr0 >> 31) & 1
    smep_enable = bool(cr4 & bit(20))
    smap_enable = bool(cr4 & bit(21))

    if (efer >> 10) & 1:
        mode_long = 1

    if ept_enabled:
        print("Extended Page Translation, ", end=' ')

    # Reduce confusion: only use "guest physical" terminology if EPT is active
    if ept_enabled:
        guest_prefix = "guest "
    else:
        guest_prefix = ""

    if paging:
        if mode_long:
            if cr4 & bit(12):
                if verbose:
                    print("LA57 mode paging")
                mode_la57 = 1
                mode_long = 0
            else:
                if verbose:
                    print("Long mode paging")
                mode_long = 1
        elif (cr4 >> 5) & 1:
            if verbose:
                print("PAE paging")
            mode_pae = 1
        else:
            if verbose:
                print("Classic paging")
            mode_classic = 1
        if verbose:
            print("CR3 0x%x" % cr3)
    else:
        print("Paging disabled")

    prot_key_enable = bool(mode_long and (cr4 & bit(22)))
    cr3 = mask_by_maxphyaddr(cr3)

    # level_info format:
    # (addr_shift, name, large_page_support, addr_mask, reserved_mask)
    if mode_la57:
        level_info = ((48, "PML5", 0, 0x1ff, 0x80),
                      (39, "PML4", 0, 0x1ff, 0x80),
                      (30, "PDPT", 1, 0x1ff, 0x3fffe000),
                      (21, "PD", 1, 0x1ff, 0x1fe000),
                      (12, "PT", 0, 0x1ff, 0x0))
        levels = 5
        entry_size = 8
        large_ps = 2
        ptr = cr3 & 0xfffffffffffff000
        u_s = 1
        r_w = 1
        nx = 0

    elif mode_pae:
        level_info = ((30, "PDPT", 0, 0x3, 0x1e6), (21, "PD", 1, 0x1ff, 0x1fe000), (12, "PT", 0, 0x1ff, 0x0))
        levels = 3
        entry_size = 8
        large_ps = 2
        ptr = cr3 & 0xffffffffffffffe0
        u_s = 1
        r_w = 1
        nx = 0

    elif mode_long:
        level_info = ((39, "PML4", 0, 0x1ff, 0x80), (30, "PDPT", 1, 0x1ff, 0x3fffe000), (21, "PD", 1, 0x1ff, 0x1fe000), (12, "PT", 0, 0x1ff, 0x0))
        levels = 4
        entry_size = 8
        large_ps = 2
        ptr = cr3 & 0xfffffffffffff000
        u_s = 1
        r_w = 1
        nx = 0

    elif mode_classic:
        level_info = ((22, "PD", 1, 0x3ff, 0x3e0000), (12, "PT", 0, 0x3ff, 0x0))
        levels = 2
        entry_size = 4
        large_ps = 4
        ptr = cr3 & 0xfffffffffffff000
        u_s = 1
        r_w = 1
        nx = 0

    else:
        if ept_enabled: print("Guest physical address %#x" % lina)
        return lina

    for level in range(levels):
        select = (lina >> level_info[level][0]) & level_info[level][3]
        addr = ptr + select * entry_size
        if verbose:
            print("%s" % level_info[level][1])
            print("    entry at " + guest_prefix + "physical 0x%x" % (addr))

        if ept_enabled:
            paddr = ept_tablewalk(cpu, addr, 4)
            if paddr == None:
                return None
            print("    entry at physical 0x%x" % paddr)
        else:
            paddr = addr

        entry = table_read(cpu.iface.processor_info.get_physical_memory(), paddr, entry_size)
        if verbose:
            print("    entry 0x%x" % entry)

        parse_entry = parse_paging_entry(cpu, entry, entry_size)

        u_s &= parse_entry["u_s"]
        r_w &= parse_entry["r_w"]
        nx |= parse_entry["nx"]

        print("    addr = 0x%x u_s = %d r_w = %d p = %d" % (parse_entry["addr"],
                                                            parse_entry["u_s"],
                                                            parse_entry["r_w"],
                                                            parse_entry["p"]))
        if not parse_entry["p"]:
            print("    Not present")
            return None
        has_reserved_bits = (mode_classic and parse_entry["ps_pat"]) or \
                            (mode_pae and  (level == 0 or parse_entry["ps_pat"])) or \
                            (mode_long and (level == 0 or parse_entry["ps_pat"]))
        if has_reserved_bits and (entry & level_info[level][4] != 0):
            print("    Reserved bits set: %#x" % (entry & level_info[level][4]))
        if prot_key_enable and level != 0:
            pkey = parse_entry["pkey"]
            ad = (cpu.pkru >> 2*pkey) & 1
            wd = (cpu.pkru >> (2*pkey+1)) & 1
            print("    Protection key %d, AD = %d, WD = %d" % (pkey, ad, wd))

        if level_info[level][2] and level_info[level][0] == 30 and parse_entry["ps_pat"]:
            if verbose:
                print("    1Gb page")
            phys = parse_entry["addr"] + (lina & ((1 * 1024 * 1024 * 1024) - 1))
            break
        elif level_info[level][2] and parse_entry["ps_pat"]:
            if verbose:
                print("    %dMb page" % large_ps)
            phys = parse_entry["addr"] + (lina & ((large_ps * 1024 * 1024) - 1))
            break
        else:
            phys = parse_entry["addr"] + (lina & ((4 * 1024) - 1))
            ptr = parse_entry["addr"]

        if level == levels-1:
            if verbose:
                print("    4kb page")

    nxe = (efer >> 11) & 1

    if not nxe: nx = 0
    # Inform that this user page is not accessible
    # from superuser mode because of SMEP and/or SMAP
    if (not nx) and u_s and smep_enable:
        print("SMEP prevents superuser execute for this page")
    if u_s and smap_enable:
        print("SMAP may prevent superuser load/store for this user page")

    if ept_enabled:
        print("Guest physical address %#x (%s, %s%s)" %
              (phys, ("supervisor", "user")[u_s],
               ("read", "read/write")[r_w], ("/execute", "")[nx]))
    return phys

def x86_diff_regs(obj):
    if cpu_has_longmode(obj):
        aa64 = 1
    else:
        aa64 = 0
    if aa64:
        return (["rax", "rbx", "rcx", "rdx",
                 "rsp", "rbp", "rsi", "rdi"]
                + ["r" + str(i) for i in range(8, get_num_gprs(obj))]
                + ["eflags",
                   "cr0", "cr2", "cr3", "cr4",
                   "cs", "cs_attr", "cs_base", "cs_limit",
                   "ds", "ds_attr", "ds_base", "ds_limit",
                   "ss", "ss_attr", "ss_base", "ss_limit",
                   "es", "es_attr", "es_base", "es_limit",
                   "fs", "fs_attr", "fs_base", "fs_limit",
                   "gs", "gs_attr", "gs_base", "gs_limit"])
    else:
        return ["eax", "ebx", "ecx", "edx",
                "esp", "ebp", "esi", "edi",
                "eflags",
                "cr0", "cr2", "cr3", "cr4",
                "cs", "cs_attr", "cs_base", "cs_limit",
                "ds", "ds_attr", "ds_base", "ds_limit",
                "ss", "ss_attr", "ss_base", "ss_limit",
                "es", "es_attr", "es_base", "es_limit",
                "fs", "fs_attr", "fs_base", "fs_limit",
                "gs", "gs_attr", "gs_base", "gs_limit"]

def local_x86_diff_regs(obj):
    return x86_diff_regs(obj)

def x86_get_info(obj):
    additional = [("Port space", obj.port_space)]
    if SIM_class_has_attribute(obj.classname, "physical_dram"):
        additional += [("DRAM space", obj.physical_dram)]
    if hasattr(obj, 'pause_slow_cycles'):
        if (obj.pause_slow_cycles == 0 and obj.rdtsc_slow_cycles == 0 and
            obj.port_io_slow_cycles == 0 and
            obj.one_step_per_string_instruction == False):
            timing = "Classic"
        elif (obj.pause_slow_cycles == int(obj.freq_mhz * 10) and
              obj.rdtsc_slow_cycles == int(obj.freq_mhz * 10) and
              obj.port_io_slow_cycles == int(obj.freq_mhz * 10) and
              obj.one_step_per_string_instruction == True):
            timing = "VMP"
        else:
            timing = "Custom"
        additional += [("Timing", timing)]
        if obj.one_step_per_string_instruction:
            string_instr_behavior = "one step for all iterations"
        else:
            string_instr_behavior = "one step per iteration"
        additional += [("PAUSE stall cycles", obj.pause_slow_cycles),
                       ("RDTSC stall cycles", obj.rdtsc_slow_cycles),
                       ("Port I/O stall cycles", obj.port_io_slow_cycles),
                       ("String instruction behavior", string_instr_behavior)]

    orig = sim_commands.common_processor_get_info(obj)
    if SIM_class_has_attribute(obj.classname, "init_vm_monitor"):
        vmp_enabled = obj.init_vm_monitor
        if vmp_enabled:
            vmp_reason = obj.vm_disable_reason
            vmp_string = ""
            if vmp_reason != None:
                vmp_string = " (not engaged: %s)" % vmp_reason

        def replace(info_item):
            # Old base versions has execution mode entry
            if vmp_enabled and info_item[0] == "Execution mode":
                return (info_item[0], f"VMP{vmp_string}")
            # New base versions has vmp status instead
            elif info_item[0] == "VMP status":
                vmp_status = f"Enabled{vmp_string}" if vmp_enabled else "Disabled"
                return (info_item[0], vmp_status)
            else:
                return info_item

        orig = [replace(x) for x in orig]
    return [ (None, orig + additional) ]

def x86_get_opcode_length_info(cpu):
    ret = opcode_length_info_t()
    ret.min_alignment = 1
    ret.max_length = 15
    ret.avg_length = 4
    return ret

def all_vmp_processors():
    return [x for x in SIM_get_all_processors() if (hasattr(x.iface, 'vmp')
            or (hasattr(x.iface, 'x86') and hasattr(x, 'vm_features')))]

def vmp_exit_trace_buffer_cmd(size):
    for cpu in all_vmp_processors():
        cpu.vmp_exit_circ_buf = size

def vmp_exit_trace_summary_cmd(sort_vmret, sort_rip, sort_mem, per_processor,
                               limit):
    def print_data(freqdict, vmret_mode = False):
        # TODO VMRET names are copied from apps-python/vmp_common.py
        # We need a single source for this list, not three as it is now!
        vmp_rets = {
            1: "VMRET_NOP",
            2: "VMRET_TRIPLE_FAULT",
            3: "VMRET_INIT",
            4: "VMRET_STARTUP_IPI",
            5: "VMRET_IRQ_WINDOW",
            6: "VMRET_TASK_SWITCH",
            7: "VMRET_CPUID",
            8: "VMRET_HLT",
            9: "VMRET_INVD",
            10: "VMRET_INVLPG",
            11: "VMRET_RDPMC",
            12: "VMRET_RDTSC",
            13: "VMRET_VMCALL",
            14: "VMRET_VMCLEAR",
            15: "VMRET_VMLAUNCH",
            16: "VMRET_VMPTRLD",
            17: "VMRET_VMPTRST",
            18: "VMRET_VMREAD",
            19: "VMRET_VMRESUME",
            20: "VMRET_VMWRITE",
            21: "VMRET_VMXOFF",
            22: "VMRET_VMXON",
            23: "VMRET_CTRL_REG_ACCESS",
            24: "VMRET_MOV_DR",
            25: "VMRET_IO",
            26: "VMRET_RDMSR",
            27: "VMRET_WRMSR",
            28: "VMRET_INV_GUEST_STATE",
            29: "VMRET_MSR_LOAD_FAILURE",
            30: "VMRET_MWAIT",
            31: "VMRET_MONITOR",
            32: "VMRET_PAUSE",
            33: "VMRET_MACHINE_CHECK",
            34: "VMRET_TPR_BELOW_THRESH",
            35: "VMRET_EXC",
            36: "VMRET_INTERNAL_ERROR",
            37: "VMRET_ENTRY_ERROR",
            38: "VMRET_PHYSPAGE_MISSING",
            39: "VMRET_PAGE_FAULT",
            40: "VMRET_CR0_VALUE_UNSUPPORTED",
            41: "VMRET_STEP_TIMER",
            42: "VMRET_AGAIN",
            43: "VMRET_EFER_VALUE_UNSUPPORTED",
            44: "VMRET_VMXON_FAILURE",
            45: "VMRET_OUT_OF_MEMORY",
            46: "VMRET_PROTECTION",
            47: "VMRET_GUEST_STATE_UNSUPPORTED",
            48: "VMRET_ICEBP",
            49: "VMRET_RANGE_BREAKPOINT",
            50: "VMRET_TICK_TIMER",
            51: "VMRET_DBG_SWITCH_CPU",
            52: "VMRET_TRACE_BUF_FULL",
            53: "VMRET_NMI_WINDOW",
            54: "VMRET_INVEPT",
            55: "VMRET_RDTSCP",
            56: "VMRET_INVVPID",
            57: "VMRET_WBINVD",
            58: "VMRET_XSETBV",
            59: "VMRET_GDTR_IDTR_ACCESS",
            60: "VMRET_LDTR_TR_ACCESS",
            61: "VMRET_RDRAND",
            62: "VMRET_INVPCID",
            63: "VMRET_VMFUNC",
            80: "VMRET_GUEST_EPT_VIOLATION",
            81: "VMRET_GUEST_EPT_MISCONFIG",
            82: "VMRET_FPU_DISABLED",
            83: "VMRET_SIGNALLED",
            84: "VMRET_PREEMPTION_TIMER",
        }

        # Sort by frequency and keep only top entries
        pairs = sorted(list(freqdict.items()), reverse=True, key=lambda x: x[1])
        pairs = pairs[:limit]

        if len(pairs) == 0:
            print("Not enough data has been collected")
            return

        # Find optimal print columns widths to fit all entries nicely
        col2_width = max([len(str(pair[1])) for pair in pairs])
        if vmret_mode:
            fn = lambda x: vmp_rets.get(x, "VMRET_%d" % x)
            col1_width = max([len(fn(pair[0])) for pair in pairs])
            fmt_str = "%%-%ds: %%%dd" % (col1_width, col2_width)
        else:
            fn = lambda x: x
            col1_width = max([len("%#x" % (pair[0])) for pair in pairs])
            fmt_str = "%%#%dx: %%%dd" % (col1_width, col2_width)

        # Finally, print the columns
        for pair in pairs:
            print(fmt_str % (fn(pair[0]), pair[1]))

    def print_requested():
        if sort_vmret:
            print("Sorted by VMRET:")
            print_data(vmrets, vmret_mode = True)
            print()
        if sort_rip:
            print("Sorted by RIP:")
            print_data(rips)
            print()
        if sort_mem:
            print("Sorted by data addresses:")
            print_data(mems)
            print()

    # If nothing is asked, set the default mode: VMRETs only
    if not (sort_vmret or sort_rip or sort_mem):
        sort_vmret = 1

    # Events aggregated by vmret code, RIP and memory address
    vmrets = dict()
    rips = dict()
    mems = dict()

    if limit is not None:
        commentstr = " (up to top %d entries shown)" % limit
    else:
        commentstr = ''
    for cpu in all_vmp_processors():
        for item in cpu.vmp_exit_circ_buf:
            (step, cycle, rip, memaddr, code, vector) = item
            # Convert signed values fom attribute to unsigned Python values
            rip = rip & 0xffffffffffffffff
            memaddr = memaddr & 0xffffffffffffffff
            if code == 0: # omit empty trace entries
                continue
            if sort_vmret:
                vmrets[code] = vmrets.get(code, 0) + 1
            if sort_rip:
                rips[rip] = rips.get(rip, 0) + 1
            if sort_mem and memaddr != 0: # Skip entries without memrefs
                mems[memaddr] = mems.get(memaddr, 0) + 1
        if per_processor:
            print("%s%s:" % (cpu.name, commentstr))
            print_requested()
            # Reset stats for the next CPU
            vmrets = dict()
            rips = dict()
            mems = dict()

    if not per_processor:
        print("Aggregated data for %d processors%s"% (len(all_vmp_processors()),
                                                      commentstr))
        print_requested()

def print_vmp_disabled_stats_cmd():
    for cpu in all_vmp_processors():
        reasons = dict()
        total = 0
        for item in cpu.vmp_disabled_reasons_stats:
            (reason, count) = item
            reasons[reason] = reasons.get(reason, 0) + count
            total += count

        vmp_steps = cpu.turbo_stat["vmp_run_steps"]
        reasons["VMP active"] = vmp_steps
        total += vmp_steps
        pairs = sorted(list(reasons.items()), reverse=True, key=lambda x: x[1])

        print("VMP disabled reasons for %s" % cpu.name)
        if total == 0:
            print("Not enough data")
            continue
        for p in pairs:
            percent = float(p[1])/total * 100
            print("%20s: %2.2f%%, %d steps" % (p[0], percent, p[1]))
        print()

def clear_vmp_disabled_stats_cmd():
    for cpu in all_vmp_processors():
        cpu.vmp_disabled_reasons_stats = []
        cpu.turbo_stat["vmp_run_steps"] = 0

def enable_lbrs_cmd():
    cpuset = [x for x in SIM_object_iterator(None)
              if hasattr(x, 'lbr_enabled')]

    for cpu in cpuset:
        cpu.lbr_enabled = True

    if len(cpuset) > 0:
        print("Successfully enabled LBRs on %d processors"
              % len(cpuset))
    else:
        print("No processors with LBRs support found")

def disable_lbrs_cmd():
    cpuset = [x for x in SIM_object_iterator(None)
              if hasattr(x, 'lbr_enabled')]

    for cpu in cpuset:
        cpu.lbr_enabled = False

    if len(cpuset) > 0:
        print("Successfully disabled LBRs on %d processors"
              % len(cpuset))
    else:
        print("No processors with LBRs support found")

def wait_for_processor_reset_command(obj):
    if not check_script_branch_command("wait-for-processor-reset"):
        return
    # This is a workaround for deprecation of sb_wait_for_hap (SIMICS-21805).
    # Calling sb_wait_for_hap_internal would allow avoiding the deprecation
    # warning, but would introduce a version dependency between cpu and base
    # package.
    old_deprecation_level = conf.sim.deprecation_level
    conf.sim.deprecation_level = 0
    sb_wait_for_hap('%s.wait-for-processor-reset' % obj.name,
                    "X86_Processor_Reset", obj, 0)
    conf.sim.deprecation_level = old_deprecation_level


processor_reset_tracked = {}

def break_processor_reset_callback(udata, cpu, hard_reset):
    assert(cpu in processor_reset_tracked)
    SIM_break_simulation("X86 processor reset")

def break_processor_reset_command(obj):
    global processor_reset_tracked
    # No need to track twice
    if obj in processor_reset_tracked:
        return
    handle = SIM_hap_add_callback_obj("X86_Processor_Reset", obj, 0,
                                      break_processor_reset_callback, obj)
    processor_reset_tracked[obj] = handle

def unbreak_processor_reset_command(obj):
    global processor_reset_tracked
    if obj not in processor_reset_tracked:
        print("Breaking on x86 processor reset in {} not enabled".format(obj))
    else:
        SIM_hap_delete_callback_id("X86_Processor_Reset",
                                   processor_reset_tracked[obj])
        del processor_reset_tracked[obj]

def register_info_command(x86_model):
    new_info_command(x86_model, x86_get_info)

def register_status_command(x86_model):
    if hasattr(sim_commands, "default_processor_get_status"):
        new_status_command(x86_model, sim_commands.default_processor_get_status)

def register_x86_commands(x86_model, cpuid_command):
    register_x86_base_commands(x86_model)
    register_pregs_sse(x86_model)
    register_pregs_fpu(x86_model)
    register_pregs_xmm(x86_model)
    register_pregs_ymm(x86_model)
    register_pregs_zmm(x86_model)
    register_msrs(x86_model)
    register_print_msrs(x86_model)
    register_msr_access_commands(x86_model)
    register_print_gdt(x86_model)
    register_print_smmseg(x86_model)
    register_print_vmcs(x86_model)
    register_print_vmx_cap(x86_model)
    register_print_tss(x86_model)
    register_tablewalk(x86_model)
    register_info_command(x86_model)
    register_status_command(x86_model)
    register_pregs_mpx(x86_model)
    register_print_cpuid(x86_model, cpuid_command)

def register_x86_base_commands(x86_model):
    new_command("wait-for-processor-reset", wait_for_processor_reset_command,
                [],
                cls = x86_model,
                type  = ["CLI"],
                short = "wait for a processor reset",
                see_also = ["script-branch"],
                doc = """
Postpones execution of a script branch until a processor reset occurs.
""")

    new_command("break-processor-reset", break_processor_reset_command,
                [],
                cls = x86_model,
                type  = ["CLI"],
                short = "break on processor reset",
                doc = """
Set the simulation to stop when this core receives the INIT or RESET signal.
""")
    new_command("unbreak-processor-reset", unbreak_processor_reset_command,
                [],
                cls = x86_model,
                type  = ["CLI"],
                short = "stop breaking on processor reset",
                doc = """
Set the simulation to no longer stop when this core receives the INIT or RESET
signal.
""")

    new_command("print-idt", idt_cmd,
                [],
                cls = x86_model,
                type = ["Processors"],
                short = "print IDT",
                doc = """
Print all descriptors in the Interrupt Descriptor Table (IDT).
""")

    new_command("print-mp-tables", scan_mp_table,
                [],
                cls = x86_model,
                type = ["Processors"],
                short = "print MP tables",
                doc = """
Print all MultiProcessor specification (MPS) tables. Typical x86
firmware (BIOS) sets up such tables during boot, meaning that the
command will not be able to find anything useful if run too early.
""")

    new_command("print-acpi-tables", scan_acpi_table,
                [arg(filename_t(), "dsdt_file", "?", None),
                 arg(filename_t(), "ssdt_file", "?", None)],
                cls = x86_model,
                type = ["Processors"],
                short = "print ACPI tables",
                doc = """
Print all Advanced Configuration and Power Interface (ACPI)
tables. Typical x86 firmware (BIOS) sets up such tables during boot,
meaning that the command will not be able to find anything useful if
run too early.

This command relies on locating the RSDP using the IA-PC method from the
ACPI specification. If the system BIOS puts the RSDP outside of the areas
defined by the IA-PC method, then the command will fail due to the RSDP
not being found.

Raw data from the DSDT and/or the SSDT tables are written to the given file
name using <arg>dsdt_file</arg> and <arg>ssdt_file</arg> respectively.
""")

    new_command("memory-configuration", memory_config_cmd,
                [],
                cls = x86_model,
                type = ["Processors", "Memory"],
                short = "print memory configuration",
                doc = """
Print the processors memory configuration. Depending on the processor type, this may
include MTRR information, HyperTransport routing information, DRAM configuration,
and other memory configuration related information.
""")

    register_cr_tracker(x86_model)
    register_msr_tracker(x86_model)
    register_msr_read_tracker(x86_model)

def register_pregs_fpu(x86_model):
    new_command("pregs-fpu", pregs_fpu_cmd,
                [arg(flag_t, "-f"), arg(flag_t, "-x"),
                 arg(flag_t, "-i"), arg(flag_t, "-b")],
                cls = x86_model,
                type = ["Processors", "Registers"],
                short = "print the x87 registers",
                doc = """
Prints the x87 floating-point registers, using one of the formatting flags.

The <tt>-f</tt> flag prints the floating-point values of the registers. The
<tt>-x</tt> flag prints the contents of the registers as hexadecimal integers.
The <tt>-i</tt> flag prints the contents of the registers as decimal integers.
With the <tt>-b</tt> flag, the registers are printed in binary floating point
form.
""")

def register_pregs_xmm(x86_model):
    new_command("pregs-xmm", pregs_xmm_cmd,
                [arg(flag_t, "-s"), arg(flag_t, "-d"), arg(flag_t, "-f"),
                 arg(flag_t, "-x"), arg(flag_t, "-i"), arg(flag_t, "-b")],
                cls = x86_model,
                type = ["Processors", "Registers"],
                short = "print the xmm registers",
                doc = """
Prints the contents of the XMM registers.<br/>

The subregister size is selected using the either the <tt>-s</tt> flag
(32-bit), or the <tt>-d</tt> flag (64-bit).

The formatting flags select the formatting of the output. The <tt>-f</tt> flag
prints the floating-point values of the registers as decimal numbers. The
<tt>-x</tt> flag prints the contents of the registers as hexadecimal integers.
The <tt>-i</tt> flag prints the contents of the registers as decimal integers.
With the <tt>-b</tt> flag, the registers are printed in binary floating point
form.
""")

def register_pregs_ymm(x86_model):
    new_command("pregs-ymm", pregs_ymm_cmd,
                [arg(flag_t, "-s"), arg(flag_t, "-d"), arg(flag_t, "-f"),
                 arg(flag_t, "-x"), arg(flag_t, "-i"), arg(flag_t, "-b")],
                cls = x86_model,
                type = ["Processors", "Registers"],
                short = "print the ymm registers",
                doc = """
Prints the contents of the YMM registers.<br/>

The subregister size is selected using the either the <tt>-s</tt> flag
(32-bit), or the <tt>-d</tt> flag (64-bit).

The formatting flags select the formatting of the output. The <tt>-f</tt> flag
prints the floating-point values of the registers as decimal numbers. The
<tt>-x</tt> flag prints the contents of the registers as hexadecimal integers.
The <tt>-i</tt> flag prints the contents of the registers as decimal integers.
With the <tt>-b</tt> flag, the registers are printed in binary floating point
form.
""")

def register_pregs_zmm(x86_model):
    new_command("pregs-zmm", pregs_zmm_cmd,
                [arg(flag_t, "-s"), arg(flag_t, "-d"), arg(flag_t, "-f"),
                 arg(flag_t, "-x"), arg(flag_t, "-i"), arg(flag_t, "-b")],
                cls = x86_model,
                type = ["Processors", "Registers"],
                short = "print the zmm registers",
                doc = """
Prints the contents of the ZMM registers.

The subregister size is selected using the either the <tt>-s</tt> flag
(32-bit), or the <tt>-d</tt> flag (64-bit).

The formatting flags select the formatting of the output. The <tt>-f</tt> flag
prints the floating-point values of the registers as decimal numbers. The
<tt>-x</tt> flag prints the contents of the registers as hexadecimal integers.
The <tt>-i</tt> flag prints the contents of the registers as decimal integers.
With the <tt>-b</tt> flag, the registers are printed in binary floating point
form.
""")

    # Make sure to register the global and namespace commands once
    if not simics_command_exists("trace-vmread"):
        register_vmcs_trackers()

def register_pregs_sse(x86_model):
    new_command("pregs-sse", pregs_xmm_cmd,
                [arg(flag_t, "-s"), arg(flag_t, "-d"), arg(flag_t, "-f"),
                 arg(flag_t, "-x"), arg(flag_t, "-i"), arg(flag_t, "-b")],
                cls = x86_model,
                type = ["Processors", "Registers"],
                short = "print the sse registers",
                doc = """
Prints the contents of the SSE registers.<br/>

The subregister size is selected using the either the <tt>-s</tt> flag
(32-bit), or the <tt>-d</tt> flag (64-bit).

The formatting flags select the formatting of the output. The <tt>-f</tt> flag
prints the floating-point values of the registers as decimal numbers. The
<tt>-x</tt> flag prints the contents of the registers as hexadecimal integers.
The <tt>-i</tt> flag prints the contents of the registers as decimal integers.
With the <tt>-b</tt> flag, the registers are printed in binary floating point
form.
""")

def register_pregs_mpx(x86_model):
    new_command("pregs-mpx", 
                lambda obj: print("Processor does not support MPX\n"),
                [],
                cls = x86_model,
                short = "deprecated",
                doc = "Deprecated, MPX no longer supported")

def get_msr_override_value(obj, address):
    if not address or not obj.msr_space:
        return None

    m = obj.msr_space.iface.memory_space.space_lookup(
        generic_transaction_t(physical_address = address * 8),
        map_info_t())
    if not m:
        return None

    if not any((m.map_info.base, m.map_info.start, m.map_info.length)):
        return None

    return [m.object, m.port, m.target_object,\
        m.map_info.base, m.map_info.start, m.map_info.length,
        m.map_info.function]

def build_msr_override_message(obj, map_list_value):
    (object, port, target_object, info_base, info_start, info_length,
        info_function) = map_list_value
    string = f"Overridden in VP, object - {SIM_object_name(object)}"
    if port:
        string += f" port {port}"
    if target_object:
        string += f" target object {target_object}"
    string += (f"\nMapping details:\n\tbase {hex(info_base)}"
        f" start {hex(info_start)} length {hex(info_length)}"
        f" function #{info_function}")

    string += "\nTo delete override issue command below:\n"
    string += (f"{SIM_object_name(obj.msr_space)}.del-map base = {hex(info_base)}"
        f" device = {SIM_object_name(object)} function = {info_function}\n")
    return string

@dataclass(slots=True)
class MsrInfo:
    msr_addr:int
    scope:str
    pseudo:bool
    noattr:bool
    nonovr:bool
    name:str
    desc:str or None
    ignore_reset:bool
    source:str
    read_mask:int
    write_mask:int

    def __init__(self,  entry):
        (self.msr_addr, self.scope, self.pseudo, self.noattr,
         self.nonovr, self.name, self.desc, self.ignore_reset,
         _, self.source, self.read_mask, self.write_mask) = entry

    @staticmethod
    def get_value(obj, addr):
        res = obj.iface.x86_msr.get(addr, Sim_X86_Msr_Attribute_Access)
        return None if res.status else (res.value)

    @staticmethod
    def get_value_str(obj, addr):
        value = MsrInfo.get_value(obj, addr)
        return f"{hex(value) if value != None else 'Failed to get value'}"

    def to_message(self, obj):
        retval =  f"Info about MSR {hex(self.msr_addr)}:\n"
        retval += f"Name: {self.name}\n"
        retval += f"Scope: {self.scope}\n"
        if self.desc:
            retval += f"Description: {self.desc}\n"
        if any([self.pseudo, self.noattr, self.nonovr, self.ignore_reset]):
            flags = {'pseudo': self.pseudo, 'no attribute': self.noattr,
                     'non-overridable': self.nonovr, 'ignore reset': self.ignore_reset}
            flags_str=", ".join([name for (name, value) in flags.items() if value])
            retval += f"Flags: {flags_str}\n"
        retval += f"Source: {self.source}\n"
        retval += f"Read mask: {hex(self.read_mask)}\n"
        retval += f"Write mask: {hex(self.write_mask)}\n"
        retval += f"Value: {MsrInfo.get_value_str(obj, self.msr_addr)}\n"
        return(retval)

    def to_value(self, obj):
        return [self.msr_addr, self.scope, self.pseudo, self.noattr,
         self.nonovr, self.name, self.desc, self.ignore_reset,
         self.source, self.read_mask, self.write_mask,
         MsrInfo.get_value(obj, self.msr_addr)]

def msr_names_expander(comp, obj):
    return get_completions(comp,
        sorted([entry[5] for entry in obj.msr_table]))

def msr_addr_expander(comp, obj):
    if not hasattr(obj, "msr_table") or not hasattr(obj, "msr_space"):
        print ("The frontend object does not have either msr_table or msr_space attribute")
        return
    msr_table_addrs = [entry[0] for entry in obj.msr_table]
    msr_overs_addrs = [e[0]//8 for e in obj.msr_space.map]
    return get_completions(comp,
        [hex(addr) for addr in msr_table_addrs+msr_overs_addrs])

def get_all_msrs_infos(obj, address, substring, name):
    retval = []
    for entry in obj.msr_table:
        msr_addr = entry[0]
        msr_name = entry[5]
        if address != None and msr_addr != address:
            continue
        if substring != None and substring not in msr_name:
            continue
        if name != None and msr_name != name:
            continue
        retval.append(MsrInfo(entry))

    return (retval)

def get_msr_info_table(obj, msr_infos):
    print_list = [(m.name, hex(m.msr_addr), MsrInfo.get_value_str(obj, m.msr_addr)) for m in msr_infos]
    print_list.sort(key = lambda x: int(x[1], 0))
    title = ["Name", "Address", "Value"]
    props = [(Table_Key_Columns, [[(Column_Key_Name, h)] for h in title])]

    return table.Table(props, print_list).to_string(no_row_column=True, rows_printed=0)

def print_msrs_cmd(obj, address, name, substring, verbose):
    msr_override_value = get_msr_override_value(obj, address)
    if msr_override_value:
        return command_return(value=msr_override_value,
            message=build_msr_override_message(obj, msr_override_value))

    msr_infos = get_all_msrs_infos(obj, address, substring, name)
    if len(msr_infos) == 0:
        return command_return(
            message=f"Implementation not found in {SIM_object_name(obj)}", value = None)
    if verbose:
        message="\n".join(i.to_message(obj) for i in msr_infos)
    else:
        message = get_msr_info_table(obj, msr_infos)
    return command_return(value = [m.to_value(obj) for m in msr_infos],
            message=message)

def msr_access_cmd(obj, address, action, value = None):
    assert action in ["get","set","read","write"]

    did_psel = False
    if not obj:
        obj = current_cpu_obj()
        did_psel = True
    try:
        msr_iface = SIM_get_interface(obj, "x86_msr")
    except SimExc_Lookup:
        return command_return(
            message=f"{obj.name} does not implement the x86_msr interface required by the command",
            value = None)

    access = Sim_X86_Msr_Attribute_Access if action in ["get", "set"] else Sim_X86_Msr_Instruction_Access

    if action in ["get", "read"]:
        res = msr_iface.get(address, access)
        message = hex(res.value) if res.status == simics.Sim_X86_Msr_Ok else f"Failed to {action}"
        value = [res.status, res.value]
    else:
        assert value != None
        res = msr_iface.set(address, value, access)
        action_past_tense = "Wrote" if action == "write" else action.capitalize()
        message = f"{action_past_tense} successfully" if res == simics.Sim_X86_Msr_Ok else f"Failed to {action}"
        value = res

    message = f"Executing on current frontend object {obj.name}\n" + message if did_psel else message
    return command_return(message=message, value=value)

def get_msr_cmd(obj, address):
    return msr_access_cmd(obj, address, "get")

def global_get_msr_cmd(cpu, address):
    return msr_access_cmd(cpu, address, "get")

def set_msr_cmd(obj, address, value):
    return msr_access_cmd(obj, address, "set", value)

def global_set_msr_cmd(cpu, address, value):
    return msr_access_cmd(cpu, address, "set", value)

def read_msr_cmd(obj, address):
    return msr_access_cmd(obj, address, "read")

def global_read_msr_cmd(cpu, address):
    return msr_access_cmd(cpu, address, "read")

def write_msr_cmd(obj, address, value):
    return msr_access_cmd(obj, address, "write", value)

def global_write_msr_cmd(cpu, address, value):
    return msr_access_cmd(cpu, address, "write", value)

def register_msr_access_commands(x86_model):
    msr_args = [arg(uint64_t, "address", "1", expander = msr_addr_expander)]
    msr_args_with_value = msr_args + [arg(uint64_t, "value", "1")]

    short_get_read = "read from a model-specific register"
    short_set_write = "write to a model-specific register"

    doc_get = "Read value of model-specific register (MSR) without" \
              " triggering side-effects."
    doc_set = "Write value to model-specific register (MSR) without" \
              " triggering side-effects."
    doc_read = "Read value of model-specific register (MSR)" \
               " the same way as RDMSR instruction would."
    doc_write = "Write value of model-specific register (MSR)" \
                " the same way as WRMSR instruction would."

    doc_address = "The <arg>address</arg> argument specifies MSR's address."
    doc_value = "The <arg>value</arg> argument specifies the value to be written" \
                " to the MSR."
    doc_expression = "When called within an expression, the result of" \
                     " corresponding function of <iface>x86_msr</iface>" \
                     " interface is returned."
    doc_cpu = "The <arg>cpu</arg> specifies target CPU. Current frontend" \
              " object is used as default."
    see_also = ["get-msr", "set-msr", "read-msr", "write-msr"]

    def create_msr_access_commands(name, cmd, args, short, doc, doc_args, see_also):
        see_also=[x for x in see_also if x != name]
        new_command(name, cmd, args, cls=x86_model, type=["Processors", "Registers"],
                    short=short, doc=f"{doc}\n\n{doc_args}\n\n{doc_expression}",
                    see_also=see_also)
        # CPU repo does not have infrastructure for global commands.
        # Register global commands on first invocation of register_x86_commands
        if not simics_command_exists(name):
            global_cmd = globals()[f'global_{cmd.__name__}']
            new_command(name, global_cmd,
                        [arg(obj_t('processor', 'processor_info'), "cpu", "?")] + args,
                        type=["Processors", "Registers"], short=short,
                        doc=f"{doc}\n\n{doc_cpu}\n\n{doc_args}\n\n{doc_expression}",
                        see_also=see_also)

    create_msr_access_commands("get-msr", get_msr_cmd, msr_args,
                               short_get_read,
                               doc_get, f"{doc_address}",
                               see_also)
    create_msr_access_commands("set-msr", set_msr_cmd, msr_args_with_value,
                               short_set_write,
                               doc_set, f"{doc_address}\n\n{doc_value}",
                               see_also)
    create_msr_access_commands("read-msr", read_msr_cmd, msr_args,
                               short_get_read,
                               doc_read, f"{doc_address}",
                               see_also)
    create_msr_access_commands("write-msr", write_msr_cmd, msr_args_with_value,
                               short_set_write,
                               doc_write, f"{doc_address}\n\n{doc_value}",
                               see_also)


def register_print_msrs(x86_model):
    new_command("print-msrs", print_msrs_cmd,
                [arg(uint64_t, "address", "?", expander = msr_addr_expander),
                 arg(str_t, "name", "?", expander = msr_names_expander),
                 arg(str_t, "substring", "?"),
                 arg(flag_t, "-verbose"),],
                cls = x86_model,
                type = ["Processors", "Registers"],
                short = "prints information about model-specific register(s)",
                doc = """
Prints information about Model-Specific Registers (MSRs).
If called without arguments, prints Name, Address and Value for all
available MSRs. By default, MSRs overridden via cpu->msr_space are excluded.\n
The <arg>address</arg> argument limits the output to a single MSR and includes
MSRs overridden via cpu->msr_space.\n
The <arg>name</arg> argument prints the MSR with the given name.\n
The <arg>substring</arg> argument prints MSRs which contain the given substring in their name.\n
The <tt>-verbose</tt> argument prints additional information: scope,
description, flags, source, read mask, write mask, current value.

The flags are:<br/>
* ignore reset - MSR retains its value after processor reset<br/>
* no attribute - MSR does not have a corresponding attribute<br/>
* non-overridable - override via cpu->msr_space is ignored<br/>
* pseudo - MSR has a corresponding attribute that is not saved/restored during checkpointing<br/>

When the command is used in an expression, a list is returned, either containing
[address, scope, pseudo, noattr, nonovr, name, description, ignore_reset,
 source, read_mask, write_mask, value] or containing
 [object, port, target_object map_info.base, map_info.start, map_info.length,
  map_info.function] if the MSR is overridden via cpu->msr_space
""")

def register_msrs(x86_model):
    new_command("msrs", msrs_cmd,
                [],
                cls = x86_model,
                type = ["Processors", "Registers"],
                short = "print MSRs",
                doc = """
Deprecated, use print-msrs instead.\n
Print model specific registers. MSRs not included in the output
are either not supported, not implemented, read-only, or must
be accessed through other attributes.
""")

def register_print_smmseg(x86_model):
    new_command("print-smmseg", smmseg_cmd,
                [],
                cls = x86_model,
                type = ["Processors"],
                short = "print SMMSEG",
                doc = """
Dump (if enabled).structure SMMSEG, with data for protected SMM mode
""")

def register_print_gdt(x86_model):
    new_command("print-gdt", gdt_cmd,
                [],
                cls = x86_model,
                type = ["Processors"],
                short = "print GDT",
                doc = """
Print all descriptors in the Global Descriptor Table (GDT). Only usable in protected mode.
""")

def vmcs_ptr_expander(comp, obj):
    return get_completions(comp,
        ['0x{:0>16x}'.format(x) for x in obj.active_vmcs])

def register_print_vmcs(x86_model):
    new_command("print-vmcs", vmcs_cmd,
                [arg(uint64_t, "vmcs-ptr", "?", expander = vmcs_ptr_expander)],
                cls = x86_model,
                type = ["Processors"],
                short = "print VMCS",
                doc = """
Print all fields in the Intel&reg; Virtual Machine Control Structure Shadowing
(Intel&reg; VMCS Shadowing). If
<arg>vmcs-ptr</arg> is given it should be a VMCS pointer and the VMCS is read
from memory. Without argument and if this command is used in VMX operation,
the current VMCS is printed.
""")

def register_print_vmx_cap(x86_model):
    new_command("print-vmx-cap", vmx_cap_cmd,
                [],
                cls = x86_model,
                type = ["Processors"],
                short = "print VMX capabilities of CPU",
                doc = """
Print VMX capabilities CPU model provides to guest.
""")

def register_print_tss(x86_model):
    new_command("print-tss", tss_cmd,
                [],
                cls = x86_model,
                type = ["Processors"],
                short = "print TSS",
                doc = """
Print the current task state structure.
""")

def register_print_cpuid(x86_model, cpuid_command):
    new_command('print-cpuid', cpuid_command,
                [arg(int_t, "leaf", "?", -1),
                 arg(flag_t, "-verbose"),
                 arg(flag_t, "-overridden"),
                 arg(flag_t, "-internal")
                ],
                cls = x86_model,
                type = ["Processors"],
                short = 'print CPUID',
                doc = """
Print CPUID table for all leaves and relevant subleaves.

The <tt>-verbose</tt> flag produces detailed information about CPUID fields that output registers contain.
The <tt>-overridden</tt> flag marks (with '*' mark) values that have been overridden by user.
The <tt>-internal</tt> flag prints additional properties of fields (implies <tt>-verbose</tt>).
All valid leaves are printed unless specific <arg>leaf</arg> is requested.
""")

def register_tablewalk(x86_model):
    new_command("tablewalk", tablewalk_cmd,
                [arg(flag_t, "-gpa"), arg(uint64_t, "address")],
                cls = x86_model,
                type = ["Processors"],
                short = "address translation tablewalk",
                doc = """
Translate a linear (i.e., after segmentation applied) or a guest physical
<arg>address</arg> to a physical address.

Traverses current paging tables and prints information about every step
along the way. By default, the command treats input address as linear; to pass
guest physical address, use the <tt>-gpa</tt> flag.""")

def register_vmp_trace_commands():
    new_command("set-vmp-exit-trace-buffer", vmp_exit_trace_buffer_cmd,
                [arg(uint64_t, "size", "?", 0)],
                type = ["Performance"],
                short = "set internal VMP profiling buffer",
                doc = """
Specify buffer capacity for all simulated processors that support VMP.
If <arg>size</arg> set to zero, the profiling is disabled. Otherwise, it is
activated. Only the most recent events are kept in the buffers.
""")

    new_command("print-vmp-exit-trace-summary", vmp_exit_trace_summary_cmd,
                [arg(flag_t, "-sort-vmret"),
                 arg(flag_t, "-sort-rip"),
                 arg(flag_t, "-sort-mem"),
                 arg(flag_t, "-per-processor"),
                 arg(int_t, "limit", "?", None),
                ],
                type = ["Performance"],
                see_also = [ "set-vmp-exit-trace-buffer"],
                short = "show collected VMP exit trace contents summary",
                doc = """
Print VMP trace buffer contents aggregated by specified manner.

The<tt>-sort-vmret</tt> flag sorts VMP exit trace by exit code (default). The
<tt>-sort-rip</tt> flag sorts VMP exit trace by target instruction pointer
value. The <tt>-sort-mem</tt> sorts VMP exit trace by data memory address.

By default, data for all processors with VMP feature is aggregated. To see
results for processors separately, use <tt>-per-processor</tt> flag.

If <arg>limit</arg> is specified, only top entries are shown.
""")

def register_vmp_disabled_reasons():
    new_command("print-vmp-disabled-reasons-stats",
                print_vmp_disabled_stats_cmd,
                [],
                type = ["Performance"],
                see_also = ["clear-vmp-disabled-reasons-stats"],
                short = "print reasons that blocked VMP",
                doc = """
Print how many steps were executed outside VMP along with corresponding reasons
(reported through vm_disable_reason attribute), compared to actual VMP steps.
Note that VMP backoff steps are not accounted there yet.

""")

    new_command("clear-vmp-disabled-reasons-stats",
                clear_vmp_disabled_stats_cmd,
                [],
                type = ["Performance"],
                see_also = ["print-vmp-disabled-reasons-stats"],
                short = "clear reasons that blocked VMP",
                doc = """
Clear statistics collected from vm_disable_reason attributes on all VMP-enabled
processors.
""")

def register_enable_lbrs_command():
    new_command("enable-lbrs", enable_lbrs_cmd,
                [],
                type = ["Processors", "Performance"],
                short = "activate precise calculation of LBRs",
                doc = """
Activate precise calculation of LBR (last branch record) counters.

By default Simics only provides enumeration for LBRs. The counters can be
enumerated and enabled but will not be updated. This is done for performance
reasons because precise implementation of LBRs disables VMP.

Run <cmd>enable-lbrs</cmd> to enable precise implementation of LBRs.
""")
    new_command("disable-lbrs", disable_lbrs_cmd,
                [],
                type = ["Processors", "Performance"],
                short = "deactivate precise calculation of LBRs",
                doc = """
Deactivate precise calculation of LBRs.
""")

def register_x86_cstate_notification_interface(x86_model):
    x86_class = SIM_get_class(x86_model)
    x86_cstate_notification_iface = x86_cstate_notification_interface_t()
    x86_cstate_notification_iface.notification = trigger_hlt_hap
    SIM_register_interface(x86_class, 'x86_cstate_notification', x86_cstate_notification_iface)

common_disass = sim_commands.make_disassembly_fun(default_instr_len=1, virtual_address_prefix = "cs", get_opcode = x86_get_opcode)
def local_disass(cpu, prefix, address, *args):
    (length, ret) = common_disass(cpu, prefix, address, *args)

    if address == cpu.iface.processor_info.get_program_counter():
        pend_str = local_pending_exception(cpu)
        if pend_str != None:
            ret += ' # ' + pend_str
    return (length, ret)

def register_processor_cli_interface(x86_model):
    x86_class = SIM_get_class(x86_model)
    processor_cli_iface = processor_cli_interface_t()
    processor_cli_iface.get_disassembly = local_disass
    processor_cli_iface.get_pregs = local_pregs
    processor_cli_iface.get_diff_regs = local_x86_diff_regs
    processor_cli_iface.get_pending_exception_string = local_pending_exception
    processor_cli_iface.get_address_prefix = local_get_address_prefix
    processor_cli_iface.translate_to_physical = local_translate_to_physical
    SIM_register_interface(x86_class, 'processor_cli', processor_cli_iface)

def register_processor_gui_interface(x86_model):
    x86_class = SIM_get_class(x86_model)
    processor_gui_iface = processor_gui_interface_t()
    SIM_register_interface(x86_class, 'processor_gui', processor_gui_iface)

def register_opcode_info_interface(x86_model):
    x86_class = SIM_get_class(x86_model)
    opc_iface = opcode_info_interface_t()
    opc_iface.get_opcode_length_info = x86_get_opcode_length_info
    SIM_register_interface(x86_class, 'opcode_info', opc_iface)

def register_aprof_views(x86_model):
    x86_class = SIM_get_class(x86_model)
    sim_commands.register_aprof_views(x86_class)

def setup_processor_ui(x86_model, cpuid_command):
    register_x86_cstate_notification_interface(x86_model)
    register_processor_cli_interface(x86_model)
    register_processor_gui_interface(x86_model)
    register_opcode_info_interface(x86_model)
    register_aprof_views(x86_model)
    register_x86_commands(x86_model, cpuid_command)

#
# -------------------- trace-segreg, break-segreg --------------------
#

# order should match the X86_Descriptor_Change hap index
segment_descriptors = "es cs ss ds fs gs tr".split()

import sim_commands
class x86_cr_tracker(sim_commands.BaseCRTracker):
    def __init__(self, stop, cmd, short, doc, type, x86_model, see_also = []):
        sim_commands.BaseCRTracker.__init__(
            self, stop, cmd,
            short, doc, iface = None, global_commands = False,
            see_also = see_also, type = type,
            cls = x86_model)
        self.hap = "X86_Descriptor_Change"
        self.map = {}
        self.catchall = {}

    def expander(self, comp, cpu):
        regs = segment_descriptors
        return get_completions(comp, regs)
    def expander_cpu(self, comp):
        return self.expander(comp, current_cpu_obj())
    def get_all_registers(self, obj):
        return list(range(len(segment_descriptors)))
    def get_register_number(self, obj, regname):
        return segment_descriptors.index(regname)
    def get_register_name(self, obj, reg):
        return segment_descriptors[reg]
    def register_number_catchable(self, obj, regno):
        return True

    def show(self, regname, obj, regno):
        if not regname:
            regname = segment_descriptors[regno]
        iface = obj.iface.int_register
        value = iface.read(iface.get_number(regname))
        print("[%s] %s <- 0x%04x" % (obj.name, regname, value))

#
# ---------- trace-vmread, break-vmread, trace-vmwrite, break-vmwrite ----------
#

class BaseVMCSTracker(sim_commands.Tracker):
    def __init__(self, stop, cmd, short, doc, type, see_also=[]):
        sim_commands.Tracker.__init__(self, stop, cmd,
                                      ((int_t,), ("field",)),
                                      (0,), short, doc,
                                      iface="x86",
                                      expander_cpu=(0,),
                                      see_also=see_also, group=type)
        self.map = {}
        self.catchall = 0

    def do_cpu_cmd(self, target_desc):
        has_vmx = False
        for cpu in SIM_get_all_processors():
            if hasattr(cpu, "vmcs_layout"):
                self.do_namespace_cmd(cpu, target_desc)
                has_vmx = True
        if not has_vmx:
            type, field, param = target_desc
            print("VMX is not supported by any processor in the system."
                  " %s is not enabled for VMCS field 0x%#x." % (
                      "Breaking" if self.stop else "Tracing", field))

    def do_cpu_uncmd(self, target_desc):
        has_vmx = False
        for cpu in SIM_get_all_processors():
            if hasattr(cpu, "vmcs_layout"):
                self.do_namespace_uncmd(cpu, target_desc)
                has_vmx = True
        if not has_vmx:
            print("VMX is not supported by any processor in the system.")

    def list(self, cpu):
        if cpu in self.catchall:
            print("[%s] %s enabled for all VMCS fields" % (
                cpu.name, "Breaking" if self.stop else "Tracing"))
        else:
            print("[%s] %s enabled for these VMCS fields:" % (
                cpu.name, "Breaking" if self.stop else "Tracing"))
            if cpu in sorted(self.map):
                for field in sorted(self.map[cpu].keys()):
                    print("  %s" % self.get_register_name(cpu, field))

    def vmcs_fields(self, cpu):
        # Skip fields that doesn't have a valid encoding.
        # Such fields are marked using bit 30.
        return [field for field in cpu.vmcs_layout if field[0] & (1 << 30) == 0]

    def vmcs_field_info(self, cpu, field):
        for f in self.vmcs_fields(cpu):
            (index, name, size, offset, attr) = f
            if index == field:
                return f

    def read_vmcs(self, cpu, field):
        (index, name, size, offset, attr) = self.vmcs_field_info(cpu, field)
        if attr & 1:  # In memory field
            paddr = cpu.current_vmcs_ptr + offset
            return table_read(obj.iface.processor_info.get_physical_memory(),
                              paddr, size)
        else: # In register field
            for f in cpu.vmcs_content:
                if f[0] == field:
                    return f[1]

    def resolve_target(self, cpu, field):
        if [f for f in self.vmcs_fields(cpu) if f[0] == field]:
            return (field, field)
        else:
            raise CliError("No '0x%x' VMCS field in %s (%s)" %
                           (field, cpu.name, cpu.classname))

    def get_all_registers(self, cpu):
        return [field[0] for field in self.vmcs_fields(cpu)]
    def get_register_number(self, cpu, field):
        return field
    def get_register_name(self, cpu, field):
        for f in self.vmcs_fields(cpu):
            if f[0] == field:
                return str(hex(f[0])) + ' "' + f[1] + '"'
    def register_number_catchable(self, cpu, field):
        return True

    def is_tracked(self, obj, target):
        fieldname, fieldno = target
        return (obj in self.catchall
                or (obj in self.map and fieldno in self.map[obj]))

    def track_all(self, obj):
        if obj in self.catchall:
            return
        if not (obj in self.map):
            self.map[obj] = {}
        for fieldno,hdl in list(self.map[obj].items()):
            SIM_hap_delete_callback_obj_id(self.hap,
                                           obj, hdl)
            del self.map[obj][fieldno]
        self.catchall[obj] = SIM_hap_add_callback_obj(
            self.hap,                      # hap
            obj,                           # trigger object
            0,                             # flags
            self.callback,                 # callback
            None)                          # user value

    def track_none(self, obj):
        if obj in self.catchall:
            SIM_hap_delete_callback_obj_id(self.hap,
                                           obj,
                                           self.catchall[obj])
            del self.catchall[obj]
        else:
            if not (obj in self.map):
                self.map[obj] = {}
            for fieldno,hdl in list(self.map[obj].items()):
                SIM_hap_delete_callback_obj_id(self.hap, obj, hdl)
                del self.map[obj][fieldno]

    def track_on(self, obj, target):
        fieldname, fieldno = target
        if obj in self.catchall:
            print(("[%s] Already %s all VMCS fields"
                   % (obj.name, "breaking on" if self.stop else "tracing")))
            return
        if self.is_tracked(obj, target):
            print(("[%s] Already %s %s"
                   % (obj.name, "breaking on" if self.stop else "tracing",
                      fieldname)))
            return
        if not self.register_number_catchable(obj, fieldno):
            print(("[%s] Cannot %s on %s"
                   % (obj.name, "break" if self.stop else "trace", fieldname)))
            return

        if not obj in self.map:
            self.map[obj] = {}
        self.map[obj][fieldno] = SIM_hap_add_callback_obj_index(
            self.hap,                      # hap
            obj,                           # trigger object
            0,                             # flags
            self.callback,                 # callback
            fieldname,                       # user value
            fieldno)                         # index

    def track_off(self, obj, target):
        fieldname, fieldno = target
        if obj in self.catchall:
            # All tracked, remove all
            self.track_none(obj)
            # Reinstall all catchable registers, except the one removed
            for r in self.get_all_registers(obj):
                if r != fieldno:
                    if self.register_number_catchable(obj, r):
                        fieldname = self.get_register_name(obj, r)
                        self.track_on(obj, (fieldname, r))
            return
        if not self.is_tracked(obj, target):
            print("[%s] Not %s %s" % (
                obj.name,
                "breaking on" if self.stop else "tracing",
                fieldname))
            return
        SIM_hap_delete_callback_obj_id(self.hap, obj, self.map[obj][fieldno])
        del self.map[obj][fieldno]

class x86_vmread_tracker(BaseVMCSTracker):
    def __init__(self, stop, cmd, short, doc, type, see_also=[]):
        BaseVMCSTracker.__init__(self, stop, cmd, short, doc, type,
                                 see_also=see_also)
        self.hap = "X86_Vmcs_Read"
        self.map = {}
        self.catchall = {}

    def show(self, regname, cpu, field):
        fieldname = self.get_register_name(cpu, field)
        print("[%s] Reading %s, value %s" % (cpu.name, fieldname,
                                number_str(self.read_vmcs(cpu, field), 16)))

class x86_vmwrite_tracker(BaseVMCSTracker):
    def __init__(self, stop, cmd, short, doc, type, see_also=[]):
        BaseVMCSTracker.__init__(self, stop, cmd, short, doc, type,
                                 see_also=see_also)
        self.hap = "X86_Vmcs_Write"
        self.map = {}
        self.catchall = {}

    def show(self, regname, cpu, field, value):
        fieldname = self.get_register_name(cpu, field)
        print("[%s] %s <- %s" % (cpu.name, fieldname, number_str(value, 16)))

def register_vmcs_trackers():
    x86_vmread_tracker(0, "trace-vmread",
                       short="trace VMCS reads",
                       type = ["Tracing"],
                       see_also=[ "break-vmread", "<bp-manager.cr>.trace",
                                    "trace-vmwrite" ],
                       doc="""
Enables and disables tracing of Intel&reg; Virtual Machine Control Structure
Shadowing (Intel&reg; VMCS Shadowing) reads.
When this is enabled, every time the specified VMCS field is read during
simulation a message is printed. The message will name the VMCS field being
read, and the value.

The <arg>field</arg> parameter specifies which VMCS field should be traced.
The available VMCS fields depend on the simulated target.

Instead of a VMCS field number, the <tt>-all</tt> flag may be given. This will
enable or disable tracing of all VMCS fields.

Using the <tt>-list</tt> argument will print out the VMCS fields accesses
currently being traced.

The non-namespace variant of this command traces VMCS field reads
on all processors that support the specified VMCS field.
The namespace variant affects only the selected processor.
""")

    x86_vmread_tracker(1, "break-vmread",
                       short="break on VMCS reads",
                       type = ["Breakpoints"],
                       see_also=[ "trace-vmread", "<bp-manager.cr>.break",
                                  "break-vmwrite" ],
                       doc="""
Enables and disables breaking simulation on Intel&reg; Virtual Machine Control
Structure Shadowing (Intel&reg; VMCS Shadowing) reads. When this is enabled,
every time the specified VMCS field is read
during simulation a message is printed. The message will name the VMCS field
being read, and the value.

The <arg>field</arg> parameter specifies which VMCS field should be traced.
The available VMCS fields depend on the simulated target.

Instead of a VMCS field number, the <tt>-all</tt> flag may be given. This will
enable or disable breaking simulation on all VMCS fields.

Using the <tt>-list</tt> argument will print out the VMCS fields accesses
on which a breakpoint will trigger.

The non-namespace variant of this command breaks the simulation on VMCS field
reads on all processors that support the specified VMCS field.
The namespace variant affects only the selected processor.
""")

    x86_vmwrite_tracker(0, "trace-vmwrite",
                        short = "trace VMCS updates",
                        type = ["Tracing"],
                        see_also = [ "break-vmwrite", "<bp-manager.cr>.trace",
                                     "trace-vmread" ],
                        doc = """
Enables and disables tracing of Intel&reg; Virtual Machine Control Structure
Shadowing (Intel&reg; VMCS Shadowing)
updates. When this is enabled, every time the specified VMCS field is updated
during simulation a message is printed. The message will name the VMCS field
being read, and the new value. The new value will be printed even if it is
identical to the previous value.

The <arg>field</arg> parameter specifies which VMCS field should be traced.
The available VMCS fields depend on the simulated target.

Instead of a VMCS field number, the <tt>-all</tt> flag may be given. This will
enable or disable tracing of all VMCS fields.

Using the <tt>-list</tt> argument will print out the VMCS fields accesses
currently being traced.

The non-namespace variant of this command traces VMCS field updates
on all processors that support the specified VMCS field.
The namespace variant affects only the selected processor.
""")

    x86_vmwrite_tracker(1, "break-vmwrite",
                   short = "break on VMCS field updates",
                   type = ["Breakpoints"],
                   see_also = [ "trace-vmwrite", "<bp-manager.cr>.break",
                                "trace-vmread" ],
                   doc = """
Enables and disables breaking simulation on Intel&reg; Virtual Machine Control
Structure Shadowing (Intel&reg; VMCS Shadowing) updates. When this is enabled,
every time the specified VMCS field is
updated during simulation a message is printed. The message will name the VMCS
field being read, and the new value. The new value will be printed even if it is
identical to the previous value.

The <arg>field</arg> parameter specifies which VMCS field should be traced.
The available VMCS fields depend on the simulated target.

Instead of a VMCS field number, the <tt>-all</tt> flag may be given. This will
enable or disable breaking simulation on all VMCS fields.

Using the <tt>-list</tt> argument will print out the VMCS fields accesses
on which a breakpoint will trigger.

The non-namespace variant of this command breaks the simulation on VMCS field
updates on all processors that support the specified VMCS field.
The namespace variant affects only the selected processor.
""")

def register_cr_tracker(x86_model):
    x86_cr_tracker(0, "trace-segreg",
                   short = "trace updates of segment registers",
                   type = ["Tracing"],
                   see_also = [ "<%s>.break-segreg" % x86_model,
                                "<bp-manager.cr>.trace" ],
                   x86_model = x86_model,
                   doc = """
Enables and disables tracing of segment registers writes.  When this
is enabled, every time the specified control <arg>register</arg> is updated
during simulation a message is printed.  The message will name the
register being updated, and the new value.  The new value will be
printed even if it is identical to the previous value.

The <i>reg-name</i> parameter specifies which segment register should
be traced.

Instead of a register name, the <tt>-all</tt> flag may be given.  This
will enable or disable tracing of all segment register.

Using the <tt>-list</tt> argument will print out the registers accesses
currently being traced.""")

    x86_cr_tracker(1, "break-segreg",
                   short = "break on control register updates",
                   type = ["Breakpoints"],
                   see_also = [ "<%s>.trace-segreg" % x86_model,
                                "<bp-manager.cr>.break" ],
                   x86_model = x86_model,
                   doc = """
Enables and disables breaking simulation on segment registers writes.
When this is enabled, every time the specified control <arg>register</arg> is
updated during simulation a message is printed.  The message will name
the register being updated, and the new value.  The new value will be
printed even if it is identical to the previous value.

The <i>reg-name</i> parameter specifies which segment register should
be traced.

Instead of a register name, the <tt>-all</tt> flag may be given.  This
will enable or disable tracing of all control register.

Using the <tt>-list</tt> argument will print out the registers accesses
on which a breakpoint will trigger.""")

#
# -------------------- trace-msr, break-msr --------------------
#
class x86_msr_tracker(sim_commands.BaseCRTracker):
    def __init__(self, stop, cmd, short, doc, type, x86_model, see_also = [],
                 is_format_supported = False):
        self.cmd = cmd
        self.stop = stop
        uncmd = "un" + cmd

        self.hap = "Core_Control_Register_Write"
        self.map = {}
        self.catchall = {}
        self.chosen_format = {}
        
        target_types_register = (int_t,)
        target_names_register = ("register",)
        expander = (self.expander,)
        register_arg = arg(target_types_register + (flag_t, flag_t),
                           target_names_register + ("-all", "-list"),
                           expander = expander + (0, 0))

        if is_format_supported:
            self.register_cmd_format_extended(cmd, uncmd, short, doc, type, 
                                              x86_model, see_also, register_arg)
        else:
            self.register_cmd_traditional(cmd, uncmd, short, doc, type,
                                          x86_model, see_also, register_arg)

    def register_cmd_traditional(self, cmd, uncmd, short, doc, type, 
                                 x86_model, see_also, register_arg):
        args = [register_arg]
        new_command(cmd, self.do_cls_cmd, args, type=type,
                    short=short, see_also = see_also, doc=doc,
                    cls=x86_model)
        new_command(uncmd, self.do_cls_uncmd, args, type=type,
                    short=short, see_also = see_also,
                    doc_with='<' + x86_model + '>.' + cmd, cls=x86_model)

    def register_cmd_format_extended(self, cmd, uncmd, short, doc, type, 
                                     x86_model, see_also, register_arg):
        format_parameter_desc_str = """

The optional <arg>format</arg> parameter specifies which trace output formatting
will be used.

Using the <tt>traditional</tt> argument preserves the original output formatting.
This is default value of argument.

Using the <tt>tracer-compatible</tt> argument will print traces in tracer 
compatible format.
"""
        format_types = (string_set_t(['traditional', 'tracer-compatible']),)
        format_names = ("format",)
        format_arg = arg(format_types, format_names, "?", default = None)

        new_command(cmd, self.do_cls_cmd, [register_arg, format_arg], type=type,
                    short=short, see_also = see_also, cls=x86_model,
                    doc=(doc + format_parameter_desc_str))

        new_command(uncmd, self.do_cls_uncmd, [register_arg], type=type,
                    short=short, see_also = see_also,
                    doc=doc, cls=x86_model)

    def do_cls_cmd(self, obj, target_desc, format = None):
        self.set_format_output(format)
        self.do_namespace_cmd(obj, target_desc)

    def set_format_output(self, format):
        if format and format[1] == 'tracer-compatible':
            self.chosen_format = "[{0}] [msr-trace] 'w' name={1} offset={2} val={3}"
        else:
            self.chosen_format = "[{0}] {1} <- {3}"

    def expander(self, comp, cpu):
        # The str_ordered_as_num class makes CLI sort completion strings
        # by MSR number instead of alphabetically. E.g., completions
        # ["0x1", "0x10", "0x8"] will be sorted like ["0x1", "0x8", "0x10"].
        # Such sorting makes more sense for the user in this case.
        @functools.total_ordering
        class str_ordered_as_num(str):
            def __new__(cls, s, num):
                obj = str.__new__(cls, s)
                obj.num = num
                return obj
            def __lt__(self, other):
                return self.num < other.num
            def __hash__(self):
                return super().__hash__()

        regs = cpu.iface.x86_msr.get_all_valid_numbers()
        regs_string = [str_ordered_as_num(hex(x), x) for x in regs]
        return get_completions(comp, regs_string)

    def msr_to_register_name(self, obj, msrname):
        return msrname

    def msr_to_register_number(self, obj, msrno):
        msrname = obj.iface.x86_msr.get_name(msrno)
        return self.get_register_number(obj, msrname)

    def track_all(self, obj):
        if obj in self.catchall:
            return
        if not (obj in self.map):
            self.map[obj] = {}
        for regno,hdl in list(self.map[obj].items()):
            SIM_hap_delete_callback_obj_id(self.hap,
                                           obj, hdl)
            del self.map[obj][regno]
        for msrno in obj.iface.x86_msr.get_all_valid_numbers():
            try:
                regno = self.msr_to_register_number(obj, msrno)
            except Exception as e:
                print("Cannot %s on MSR 0x%x: %s"
                    % ("break" if self.stop else "trace", msrno, str(e)))
                continue
            regname = self.get_register_name(obj, regno)
            self.track_on(obj, (regname, regno))

        # For correct printing if tracking of all registers is enabled
        self.catchall[obj] = 1

    def resolve_target(self, obj, msrno):
        try:
            regno = self.msr_to_register_number(obj, msrno)
        except:
            regno = obj.iface.int_register_v2.add_msr_dummy(msrno)
            print("No MSR 0x%x in %s (%s). Assigning control register %d"
                  " and %s it in case if MSR is implemented externally"
                  % (msrno, obj.name, obj.classname, regno,
                     "breaking on" if self.stop else "tracing"))
        regname = self.get_register_name(obj, regno)
        return (regname, regno)

    def track_none(self, obj):
        if obj in self.catchall:
            del self.catchall[obj]
        if not (obj in self.map):
            return
        for regno,hdl in list(self.map[obj].items()):
            SIM_hap_delete_callback_obj_id(self.hap, obj, hdl)
            del self.map[obj][regno]

    def track_off(self, obj, target):
        regname, regno = target
        if obj in self.catchall:
            del self.catchall[obj]
        if not self.is_tracked(obj, target):
            print("[%s] Not %s %s" % (
                obj.name,
                "breaking on" if self.stop else "tracing",
                regname))
            return
        SIM_hap_delete_callback_obj_id(self.hap, obj, self.map[obj][regno])
        del self.map[obj][regno]

    def show(self, regname, obj, regno, value = None):
        if not regname:
            regname = self.get_register_name(obj, regno)
            if regname is None:
                regname = "unknown register %d" % regno

        if not value:
            value = obj.iface.int_register.read(regno)

        if value < 0:
            value += 1 << 64

        print(self.chosen_format.format(obj.name, regname, number_str(regno, 16), 
                                        number_str(value, 16)))

def register_msr_tracker(x86_model):
    x86_msr_tracker(0, "trace-msr",
                    short = "trace model specific register updates",
                    type = ["Tracing"],
                    see_also = [ f"<{x86_model}>.break-msr", "<bp-manager.cr>.trace",
                                 f"<{x86_model}>.trace-msr-read" ],
                    x86_model = x86_model,
                    is_format_supported = True,
                    doc = """
Deprecated, use bp.msr.trace instead.\n
Enables and disables tracing of model specific register updates.  When this
is enabled, every time the specified model specific register is updated by
guest software via WRMSR instruction a message is printed. The message will
name the register being updated, and the new value. The new value will be
printed even if it is identical to the previous value.

The <arg>register</arg> parameter specifies which model specific register
should be traced.

Instead of a register name, the <tt>-all</tt> flag may be given. This
will enable or disable tracing of all model specific registers.

Using the <tt>-list</tt> argument will print out the registers accesses
currently being traced.""")

    x86_msr_tracker(1, "break-msr",
                    short = "break on model specific register updates",
                    type = ["Breakpoints"],
                    see_also = [ f"<{x86_model}>.trace-msr", "<bp-manager.cr>.break",
                                 f"<{x86_model}>.break-msr-read" ],
                    x86_model = x86_model,
                    doc = """
Deprecated, use bp.msr.break instead.\n
Enables and disables breaking simulation on model-specific registers writes.
When this is enabled, every time the specified model specific register is
updated by guest software via WRMSR instruction a message is printed.
The message will name the register being updated, and the new value. The new
value will be printed even if it is identical to the previous value.

The <arg>register</arg> parameter specifies which model specificfc register
should be traced.

Instead of a register name, the <tt>-all</tt> flag may be given.  This
will enable or disable tracing of all control register.

Using the <tt>-list</tt> argument will print out the registers accesses
on which a breakpoint will trigger.""")

#
# -------------------- trace-msr-read, break-msr-read --------------------
#
class x86_msr_read_tracker(x86_msr_tracker):
    def __init__(self, stop, cmd, short, doc, type, x86_model, see_also = [],
                 is_format_supported = False):
        x86_msr_tracker.__init__(self, stop, cmd, short, doc, type,
                                 x86_model, see_also = see_also, 
                                 is_format_supported = is_format_supported)
        self.hap = "Core_Control_Register_Read"

    def set_format_output(self, format):
        if format and format[1] == 'tracer-compatible':
            self.chosen_format = "[{0}] [msr-trace] 'r' name={1} offset={2} val={3}"
        else:
            self.chosen_format = "[{0}] Reading {1}, value = {3}"

def register_msr_read_tracker(x86_model):
    x86_msr_read_tracker(0, "trace-msr-read",
                    short = "trace model specific register reads",
                    type = ["Tracing"],
                    see_also = [ f"<{x86_model}>.break-msr-read",
                                 "<bp-manager.cr>.trace",
                                 f"<{x86_model}>.trace-msr" ],
                    x86_model = x86_model,
                    is_format_supported = True,
                    doc = """
Deprecated, use bp.msr.trace instead.\n
Enables and disables tracing of model specific register reads. When this
is enabled, every time the specified register is read by guest software via
RDMSR instruction a message is printed. The message will name the register
being read, and the value.

The <arg>register</arg> parameter specifies which model specific register
should be traced.

Instead of a register name, the <tt>-all</tt> flag may be given.  This
will enable or disable tracing of all model specific registers.

Using the <tt>-list</tt> argument will print out the registers accesses
currently being traced.""")

    x86_msr_read_tracker(1, "break-msr-read",
                    short = "break on model specific register updates",
                    type = ["Breakpoints"],
                    see_also = [ f"<{x86_model}>.trace-msr-read",
                                 "<bp-manager.cr>.break",
                                 f"<{x86_model}>.break-msr" ],
                    x86_model = x86_model,
                    doc = """
Deprecated, use bp.msr.break instead.\n
Enables and disables breaking simulation on model specific register reads.
When this is enabled, every time the specified register is read by guest
software via RDMSR instruction a message is printed. The message will name the
register being read, and the value.

The <arg>register</arg> parameter specifies which model specific register
should be traced.

Instead of a register name, the <tt>-all</tt> flag may be given.  This
will enable or disable tracing of all model specific register.

Using the <tt>-list</tt> argument will print out the registers accesses
on which a breakpoint will trigger.""")

# Global commands registration

# A hack to prevent re-registration of global commands. Check if it is already
# defined. A redefinition of a CLI command breaks test-runner.
# A real solution is to move global commands to apps-python in Base

from cli_impl import all_commands
if not all_commands.exists("set-vmp-exit-trace-buffer"):
    register_vmp_trace_commands()

if not all_commands.exists("print-vmp-disabled-reasons-stats"):
    register_vmp_disabled_reasons()

if not all_commands.exists("enable-lbrs"):
    register_enable_lbrs_command()

def setup_all_ui_for_classes(cpu_classes):
    for cpu_class in cpu_classes:
        setup_processor_ui(cpu_class, generated_cpuid_print.print_cpuid)
