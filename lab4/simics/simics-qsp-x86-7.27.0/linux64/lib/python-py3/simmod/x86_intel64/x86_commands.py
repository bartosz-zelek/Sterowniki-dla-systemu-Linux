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


from cli import (
    CliError,
    Just_Left,
    Just_Right,
    arg,
    check_script_branch_command,
    filename_t,
    flag_t,
    get_completions,
    int_t,
    new_command,
    new_info_command,
    new_status_command,
    number_str,
    print_columns,
    simics_command_exists,
    uint64_t,
    )
from script_branch import (
    sb_wait_for_hap_internal,
)
import sim_commands
import fp_to_string
import simics
from simics import *
import vmp_common
from functools import cmp_to_key

column_size = { }

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

    if (SIM_class_has_attribute(cpu.classname, "pending_reset") and
        cpu.pending_reset):
        pend.append("Pending RESET")

    if (SIM_class_has_attribute(cpu.classname, "pending_init") and
        cpu.pending_init):
        pend.append("Pending INIT")

    if (cpu.iface.x86_reg_access.get_rflags() & 0x200) and cpu.iface.x86_reg_access.get_interruptibility_state() == X86_Intstate_Not_Blocking:
        if (SIM_class_has_attribute(cpu.classname, "pending_debug_exception") and cpu.pending_debug_exception):
            pend.append("Pending debug exception")

        if (SIM_class_has_attribute(cpu.classname, "waiting_interrupt") and cpu.waiting_interrupt):
            pend.append("Pending interrupt")

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

def x86_get_opcode(cpu, bytes):
    l = [("%02x" % b) for b in bytes]
    return "%-17s" % " ".join(l)

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

#
# -------------------- local-pregs --------------------
#

def get_eflags(cpu, ef, reg_name="eflags"):
    is_nanocore = cpu_is_nanocore(cpu)
    if (ef & 0xf000) == 0xf000:
        has_pm = False
    else:
        has_pm = True
    ret = "%s = " % reg_name
    if has_pm and not is_nanocore:
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
    if is_nanocore:
        ret += "      U O - I T S Z - - - - - C\n"
    elif has_pm:
        ret += "I V V A V R - N I I O D I T S Z - A - P - C\n"
    else:
        ret += "- - - - O D I T S Z - A - P - C\n"
    ret += ""
    ret += " "*(len(reg_name) + 3)
    if is_nanocore:
        ret += "        F   F F F F           F\n"
    elif has_pm:
        ret += "D I I C M F   T O O F F F F F F   F   F   F\n"
    else:
        ret += "        F F F F F F   F   F   F\n"
    ret += ""
    ret += " "*(len(reg_name) + 3)
    if has_pm and not is_nanocore:
        ret += "  P F           P P                        \n"
        ret += " "*(len(reg_name) + 3)
        ret += "                L L                        \n"
    return ret

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

model_capabilities = {}

# List of x86 model capabilities queried by the commands
#    pm        Support for protected mode (segment registers/...)
#    ia32      Support for IA32 32-bit operation (32-bit registers/paging/...)
#    aa64      Support for Intel64 (64-bit registers/...)
#    xmm       Has SSE
#    nanocore  Nanocore
def register_capabilities(model, capabilities):
    global model_capabilities
    model_capabilities[model] = capabilities

def cpu_is_nanocore(obj):
    return model_capabilities[obj.classname].get("nanocore", False)

def cpu_is_ia32(obj):
    return model_capabilities[obj.classname].get("ia32", False)

def cpu_is_aa64(obj):
    return model_capabilities[obj.classname].get("aa64", False)

def cpu_has_pm(obj):
    return model_capabilities[obj.classname].get("pm", False)

def cpu_has_cr4(obj):
    return model_capabilities[obj.classname].get("cr4", False)

def cpu_has_xmm(obj):
    return model_capabilities[obj.classname].get("xmm", False)

def get_remaining_regs(obj, aa64):
    ret = "\n"
    ia32 = cpu_is_ia32(obj)
    has_pm = cpu_has_pm(obj)
    is_nanocore = cpu_is_nanocore(obj)
    def format_segment(obj, n, has_limit_and_attr):
        seg_reg = obj.iface.x86_reg_access.get_seg(seg_to_index[n])
        ret = "%s   = 0x%04x, base = 0x%s" % (n, seg_reg.sel, hex_str(seg_reg.base,8))
        if has_limit_and_attr:
            ret += ", limit = 0x%x, attr = 0x%x" % (seg_reg.limit, seg_reg.attr)
        ret += "\n"
        return ret
    if not is_nanocore:
        ret += format_segment(obj, "es", has_pm)
        ret += format_segment(obj, "cs", has_pm)
        ret += format_segment(obj, "ss", has_pm)
        ret += format_segment(obj, "ds", has_pm)
        if ia32:
            ret += format_segment(obj, "fs", True)
            ret += format_segment(obj, "gs", True)
            ret += format_segment(obj, "tr", True)
            ret += format_segment(obj, "ldtr", True)

    if has_pm:
        idtr = obj.iface.x86_reg_access.get_system_seg(X86_Idtr)
        ret += "idtr: base = %s, limit = %05x\n" % (local_string(idtr.base,aa64), idtr.limit)
        if not is_nanocore:
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

    if ia32 and not is_nanocore:
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
        ret += "cr4 = %d - %d %d %d %d %d %d %d %d %d %d %d = 0x%s\n" % (
            (cr4 >> 13) & 1,
            (cr4 >> 10) & 1,
            (cr4 >> 9) & 1,
            (cr4 >> 8) & 1,
            (cr4 >> 7) & 1,
            (cr4 >> 6) & 1,
            (cr4 >> 5) & 1,
            (cr4 >> 4) & 1,
            (cr4 >> 3) & 1,
            (cr4 >> 2) & 1,
            (cr4 >> 1) & 1,
            (cr4 >> 0) & 1,
            hex_str(cr4,8))

        ret += "      V   O O P P M P P D T P V\n"
        ret += "      M   S S C G C A S E S V M\n"
        ret += "      X   X F E E E E E   D I E\n"
        ret += "      E   M X\n"
        ret += "          M S\n"
        ret += "          E R\n"
        ret += "          X\n"
        ret += "          C\n"
        ret += "          P\n"
        ret += "          T\n\n"

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
    if (leaf_1.d & 1): # FPU present
        ret += get_fpu_cmd(obj, 0, 0, 0, 0)
        ret += "\n"

    if (leaf_1.d & (1 << 25)): # SSE present
        ret += get_sse_cmd(obj, 0, 0, 0, 0, 0, 0)

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

    aa64 = cpu_is_aa64(obj)
    ret += "\nLast operation:\n"
    ret += "    Instruction  0x%04x:%s\n" % (fpu_env.last_instr_sel, local_string(fpu_env.last_instr_ptr, aa64))
    ret += "    Opcode       0x%x\n" % fpu_env.opc
    ret += "    Operand      0x%04x:%s\n" % (fpu_env.last_operand_sel, local_string(fpu_env.last_operand_ptr, aa64))
    return ret

def current_x86_mode(obj):
    exec_mode = obj.iface.x86_reg_access.get_exec_mode()
    xmode_info = obj.iface.x86_reg_access.get_xmode_info()

    if cpu_is_nanocore(obj):
        mode = "Nanocore mode"
    else:
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
                mode = "32-bit legacy protected mode"
            else:
                mode = "16-bit legacy protected mode"
        elif exec_mode == X86_Exec_Mode_Compat:
            if xmode_info.cs_d:
                mode = "32-bit compatibility mode"
            else:
                mode = "16-bit compatibility mode"
        elif exec_mode == X86_Exec_Mode_64:
            mode = "64-bit mode"

    if SIM_class_has_attribute(obj.classname, "in_smm") and SIM_get_attribute(obj, "in_smm"):
        mode += " (system management mode)"
    if SIM_class_has_attribute(obj.classname, "vmx_mode"):
        if obj.vmx_mode == 1:
            mode += " (VMX root operation)"
        elif obj.vmx_mode == 2:
            mode += " (VMX non-root operation)"
    return mode

def local_pregs(obj, a):
    aa64 = cpu_is_aa64(obj)
    ia32 = cpu_is_ia32(obj)
    mode = current_x86_mode(obj)
    ret = mode + "\n"
    state = local_pending_exception(obj)
    if state != None:
        ret += state + "\n"
    def gpr(obj, n):
        return obj.iface.x86_reg_access.get_gpr(n)
    if aa64:
        ret += "rax = 0x%016x             r8  = 0x%016x\n" % (gpr(obj, 0),
                                                              gpr(obj, 8))
        ret += "rcx = 0x%016x             r9  = 0x%016x\n" % (gpr(obj, 1),
                                                              gpr(obj, 9))
        ret += "rdx = 0x%016x             r10 = 0x%016x\n" % (gpr(obj, 2),
                                                              gpr(obj, 10))
        ret += "rbx = 0x%016x             r11 = 0x%016x\n" % (gpr(obj, 3),
                                                              gpr(obj, 11))
        ret += "rsp = 0x%016x             r12 = 0x%016x\n" % (gpr(obj, 4),
                                                              gpr(obj, 12))
        ret += "rbp = 0x%016x             r13 = 0x%016x\n" % (gpr(obj, 5),
                                                              gpr(obj, 13))
        ret += "rsi = 0x%016x             r14 = 0x%016x\n" % (gpr(obj, 6),
                                                              gpr(obj, 14))
        ret += "rdi = 0x%016x             r15 = 0x%016x\n" % (gpr(obj, 7),
                                                              gpr(obj, 15))
        ret += "\n"
        ret += "rip = 0x%016x\n" % obj.iface.x86_reg_access.get_rip()
        ret += "\n"
        ef = obj.iface.x86_reg_access.get_rflags()
        ret += get_eflags(obj, ef)
    elif ia32:
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
        ef = obj.iface.x86_reg_access.get_rflags()
        ret += get_eflags(obj, ef)
    else: # 16-bit
        ret += "%s = 0x%04x, %s = 0x%02x, %s = 0x%02x\n" % ("ax", gpr(obj, 0), "ah", (gpr(obj, 0) >> 8) & 0xff, "al", gpr(obj, 0) & 0xff)
        ret += "%s = 0x%04x, %s = 0x%02x, %s = 0x%02x\n" % ("cx", gpr(obj, 1), "ch", (gpr(obj, 1) >> 8) & 0xff, "cl", gpr(obj, 1) & 0xff)
        ret += "%s = 0x%04x, %s = 0x%02x, %s = 0x%02x\n" % ("dx", gpr(obj, 2), "dh", (gpr(obj, 2) >> 8) & 0xff, "dl", gpr(obj, 2) & 0xff)
        ret += "%s = 0x%04x, %s = 0x%02x, %s = 0x%02x\n" % ("bx", gpr(obj, 3), "bh", (gpr(obj, 3) >> 8) & 0xff, "bl", gpr(obj, 3) & 0xff)
        ret += "%s = 0x%04x\n" % ("sp", gpr(obj, 4))
        ret += "%s = 0x%04x\n" % ("bp", gpr(obj, 5))
        ret += "%s = 0x%04x\n" % ("si", gpr(obj, 6))
        ret += "%s = 0x%04x\n" % ("di", gpr(obj, 7))
        ret += "\n"
        ret += "ip = 0x%s\n" % hex_str(obj.iface.x86_reg_access.get_rip(),4)
        ret += "\n"
        ef = obj.iface.x86_reg_access.get_rflags()
        ret += get_eflags(obj, ef, reg_name="flags")

    intstate = obj.iface.x86_reg_access.get_interruptibility_state()
    if intstate != X86_Intstate_Not_Blocking:
        ret += "\n"
        if intstate == X86_Intstate_Blocking_INT_Sti:
            ret += "Interrupts temporarily masked after STI\n"
        elif intstate == X86_Intstate_Blocking_INT_Mov_Ss:
            ret += "Interrupts temporarily masked after SS write\n"
        elif intstate == X86_Intstate_Blocking_INIT:
            ret += "Interrupts temporarily masked due to INIT\n"
        elif intstate == X86_Intstate_Blocking_SMI:
            ret += "Interrupts temporarily masked due to SMI\n"
        elif intstate == X86_Intstate_Blocking_NMI:
            ret += "Interrupts temporarily masked due to NMI\n"
    if a:
        ret += get_remaining_regs(obj, aa64)
    return ret

def pregs_sse_cmd(obj, s, d, f, x, i, b):
    print(get_sse_cmd(obj, s, d, f, x, i, b))

def get_sse_cmd(obj, s, d, f, x, i, b):
    if not cpu_has_xmm(obj):
        return "Processor does not support SSE."
    aa64 = cpu_is_aa64(obj)
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
        for r in sub:
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
        ret += "\n"
    mxcsr = obj.iface.x86_reg_access.get_mxcsr()
    ret += "\nmxcsr = %d %d %d %d %d %d %d %d - %d %d %d %d %d %d = 0x%08x\n" % (
        (mxcsr >> 15) & 1,
        (mxcsr >> 13) & 3,
        (mxcsr >> 12) & 1,
        (mxcsr >> 11) & 1,
        (mxcsr >> 10) & 1,
        (mxcsr >> 9) & 1,
        (mxcsr >> 8) & 1,
        (mxcsr >> 7) & 1,
        (mxcsr >> 5) & 1,
        (mxcsr >> 4) & 1,
        (mxcsr >> 3) & 1,
        (mxcsr >> 2) & 1,
        (mxcsr >> 1) & 1,
        mxcsr & 1,
        mxcsr)
    ret += "        F R P U O Z D I   P U O Z D I\n"
    ret += "        Z C M M M M M M   E E E E E E\n"
    return ret

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
    if not hasattr(obj.iface, "x86"):
        return
    class_name = obj.classname
    if SIM_class_has_attribute(class_name, "ah"):
        return
    is_x86_64 = SIM_class_has_attribute(class_name, "rax")
    if is_x86_64:
        reg_l = "r"
    else:
        reg_l = "e"
    for reg in ("ax", "bx", "cx", "dx", "di", "si", "bp", "sp", "ip"):
        if is_x86_64:
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
    if is_x86_64:
        for reg in ("di", "si", "bp", "sp"):
            SIM_register_typed_attribute(class_name, reg + "l",
                                         get_lower_8bits, "r" + reg,
                                         set_lower_8bits, "r" + reg,
                                         4, "i", None,
                                         "Register " + reg
                                         + "l (lower 8 bits of "
                                         + reg_l + reg + ").")
        for reg in ("r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"):
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
    if calc_sum(cpu, ptr, length):
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
    for i in range(length):
        if (i % 16) == 0:
            simics.pr("  %04x: " % i)
        simics.pr("%02x " % table_read(space, addr + i, 1))
        if (i % 16) == 15 or i == length - 1:
            simics.pr("\n")

def scan_uefi_system_table(cpu):
    """17.4.2 EFI System Table Location

    The EFI system table can be located by an off-target hardware debugger by
    searching for the EFI_SYSTEM_TABLE_POINTER structure. The
    EFI_SYSTEM_TABLE_POINTER structure is located on a 4M boundary as close to
    the top of physical memory as feasible. It may be found searching for the
    EFI_SYSTEM_TABLE_SIGNATURE on each 4M boundary starting at the top of memory
    and scanning down. When the signature is found, the entire structure must
    verified using the Crc32 field. The 32-bit CRC of the entire structure is
    calculated assuming the Crc32 field is zero. This value is then written to
    the Crc32 field.

    typedef struct _EFI_SYSTEM_TABLE_POINTER {
      UINT64                    Signature;
      EFI_PHYSICAL_ADDRESS      EfiSystemTableBase;
      UINT32                    Crc32;
    } EFI_SYSTEM_TABLE_POINTER;

    Signature               A constant UINT64 that has the value
                            EFI_SYSTEM_TABLE_SIGNATURE (see the EFI 1.0
                            specification).
    EfiSystemTableBase      The physical address of the EFI system table.
    Crc32                   A 32-bit CRC value that is used to verify
                            the EFI_SYSTEM_TABLE_POINTER structure is valid

    #define EFI_SYSTEM_TABLE_SIGNATURE      0x5453595320494249
    typedef UINT64                          EFI_PHYSICAL_ADDRESS

    """

    import binascii
    EFI_SYSTEM_TABLE_SIGNATURE = 0x5453595320494249

    space = cpu.physical_memory

    # 4Mb boundary
    boundary_4Mb = (1024 * 1024 * 4)
    boundary_4Gb = (1024 * 1024 * 1024 * 4)
    addr = boundary_4Mb

    ptr = None
    while addr < boundary_4Gb:  # TODO: scan from "the top of memory"
        try:
            signature = table_read(space, addr, 8)
            if signature == EFI_SYSTEM_TABLE_SIGNATURE:
                crc32_tp = table_read(space, addr + 16, 4)
                data = (get_bytes(cpu, addr, 16)
                        + b'\0\0\0\0'
                        # NB: count also 4-byte padding located at the end:
                        + get_bytes(cpu, addr + 20, 4))
                crc32_tp_comp = binascii.crc32(data)
                if crc32_tp == crc32_tp_comp:
                    ptr = table_read(space, addr + 8, 8)
                    break
        except CliError:  # we get CliError if we failed to read the memory
            pass
        addr += boundary_4Mb

    if ptr is None:
        return

    signature = table_read(space, ptr, 8)
    if signature != EFI_SYSTEM_TABLE_SIGNATURE:
        return

    header_size = table_read(space, ptr + 12, 4)
    crc32_t = table_read(space, ptr + 16, 4)
    data = (get_bytes(cpu, ptr, 16)
            + b'\0\0\0\0'
            + get_bytes(cpu, ptr + 20, header_size - 20))
    crc32_t_comp = binascii.crc32(data)

    if crc32_t != crc32_t_comp:
        print("Warning: EFI_SYSTEM_TABLE checksum mismatch at {0:#x}:"
              " checksum from EFI_SYSTEM_TABLE - {1:#x},"
              " actual checksum - {2:#x}".format(
                  ptr, crc32_t, crc32_t_comp))

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
        guid = read_guid(space, ConfigurationTable + 24 * i)

        if guid == 'EB9D2D30-2D88-11D3-9A16-0090273FC14D':
            # Found a pointer to the ACPI 1.0 specification RSDP structure
            return table_read(space, ConfigurationTable + 16 + 24 * i, 8)
        if guid == '8868E871-E4F1-11D3-BC22-0080C73C8881':
            # Found a pointer to the ACPI 2.0 or later spec RSDP structure
            return table_read(space, ConfigurationTable + 16 + 24 * i, 8)

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
        print("RSDP address was found by searching legacy PC BIOS map")
    else:
        print("RSDP address was not found by searching legacy PC BIOS map")

        ptr = scan_uefi_system_table(cpu)
        if not ptr:
            print("RSDP address was not found by searching UEFI map")
            return
        else:
            print("RSDP address was found by searching UEFI map")

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

    read_from_memory = True
    field_vals = []

    if vmcs_ptr == None:
        read_from_memory = False
        vmcs_ptr = obj.current_vmcs_ptr
        field_vals = obj.vmcs_content

    if vmcs_ptr == 0xffffffffffffffff or (vmcs_ptr & 0xfff) != 0:
        print("Invalid VMCS pointer")
        return

    if read_from_memory:
        print("VMCS content from memory")
    else:
        print("Active VMCS content from CPU registers and/or memory")

    field_info = obj.vmcs_layout
    field_info_dict = {}
    for (index, name, size, offset, attr) in field_info:
        if (index & 1) == 1:
            # Skip the "high" fields
            continue
        field_info_dict[index] = (name, size)
        if read_from_memory or (attr & 1):
            paddr = vmcs_ptr + offset
            value = table_read(obj.iface.processor_info.get_physical_memory(), paddr, size)
            field_vals.append((index, value))

    all_fields = []
    for (index, f) in field_vals:
        (name, size) = field_info_dict[index]
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
    print("Processor supports VMX.")

    def get_msr_basic(obj, msr_num):
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
        "rsvd",         #bit 11
        "rsvd",         #bit 12
        "VMXE",         #bit 13
        "SMXE",         #bit 14
        "rsvd",         #bit 15
        "FSGBASE",      #bit 16
        "PCIDE",        #bit 17
        "OSXSAVE",      #bit 18
        "rsvd",         #bit 19
        "SMEP",         #bit 20
        "SMAP",         #bit 21
        "rsvd",         #bit 22
        "rsvd",         #bit 23
        "rsvd",         #bit 24
        "rsvd",         #bit 25
        "rsvd",         #bit 26
        "rsvd",         #bit 27
        "rsvd",         #bit 28
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


    #report information from IA32_VMX_BASIC MSR
    print("\nBasic VMX information.")
    vmx_basic_msr = msr_obj_t(
        num = VMX_BASIC_INFORMATION_MSR,
        val = get_msr(VMX_BASIC_INFORMATION_MSR))

    print("VMX_BASIC {0:#x} MSR value: {1:#018x}".format(vmx_basic_msr.num, vmx_basic_msr.val))
    print(("Detailed information: \n"
          "  VMCS revision : 0x%x, \n"
          "  VMCS region size : 0x%x, \n"
          "  physical address width : %s, \n"
          "  dual monitor treatment support : %s, \n"
          "  memory type used : %u, \n"
          "  information in the VM-exit instruction-information field on VM exits due to INS and OUTS : %s, \n"
          "  may any default1 VMX control be cleared to 0 : %s." %
           (vmx_basic_msr[0:31],
            vmx_basic_msr[32:45],
            "32 bits" if vmx_basic_msr[48] else "equals to CPU physical-address width",
            "yes" if vmx_basic_msr[49] else "no",
            vmx_basic_msr[50:54],
            "yes" if vmx_basic_msr[54] else "no",
            "yes" if vmx_basic_msr[55] else "no",)))

    isDefault1toNullSupported = vmx_basic_msr[55]

    #report information on Pin-Based VM-Execution Controls
    print("\nPin-Based VM-Execution Controls.")
    vmx_pinbased_ctls_msr = msr_obj_t(
        num = VMX_EXECUTION_PIN_CONTROLS_MSR,
        val = get_msr(VMX_EXECUTION_PIN_CONTROLS_MSR))

    print("VMX_PINBASED_CTLS ({0:#x}) MSR value: {1:#018x}".format(
        vmx_pinbased_ctls_msr.num,
        vmx_pinbased_ctls_msr.val
        ))
    if isDefault1toNullSupported:
        print("Read VMX features support information from IA32_VMX_TRUE_PINBASED_CTLS MSR.")
        vmx_true_pinbased_ctls_msr = msr_obj_t(
            num = VMX_EXECUTION_TRUE_PIN_CONTROLS_MSR,
            val = get_msr(VMX_EXECUTION_TRUE_PIN_CONTROLS_MSR))

        print("VMX_TRUE_PINBASED_CTLS ({0:#x}) MSR value : {1:#018x}".format(
            vmx_true_pinbased_ctls_msr.num,
            vmx_true_pinbased_ctls_msr.val
            ))
        pinbased_ctls_to_use = vmx_true_pinbased_ctls_msr
    else:
        print("Read VMX features support information from IA32_VMX_PINBASED_CTLS MSR.")
        pinbased_ctls_to_use = vmx_pinbased_ctls_msr

    print(("Detailed information: \n"
           "    external-interrupt exiting (supported values) : %s %s, \n"
           "    NMI exiting (supported values) : %s %s, \n"
           "    Virtual NMIs (supported values) : %s %s, \n"
           "    Activate VMX-preemption timer (supported values) : %s %s." %

           ("" if pinbased_ctls_to_use[0] else "0", "1" if pinbased_ctls_to_use[32+0] else "",
           "" if pinbased_ctls_to_use[3] else "0", "1" if pinbased_ctls_to_use[32+3] else "",
           "" if pinbased_ctls_to_use[5] else "0", "1" if pinbased_ctls_to_use[32+5] else "",
           "" if pinbased_ctls_to_use[6] else "0", "1" if pinbased_ctls_to_use[32+6] else "",)))


    #report information on Primary Processor-Based VM-Execution Controls
    print("\nPrimary Processor-Based VM-Execution Controls.")
    vmx_procbased_ctls_msr = msr_obj_t(
        num = VMX_EXECUTION_PROC_CONTROLS_MSR,
        val = get_msr(VMX_EXECUTION_PROC_CONTROLS_MSR))

    print("VMX_PROCBASED_CTLS ({0:#x}) MSR value: {1:#018x}".format(
        vmx_procbased_ctls_msr.num,
        vmx_procbased_ctls_msr.val
        ))
    if isDefault1toNullSupported:
        print("Read VMX features support information from IA32_VMX_TRUE_PROCBASED_CTLS MSR.")
        vmx_true_procbased_ctls_msr = msr_obj_t(
            num = VMX_EXECUTION_TRUE_PROC_CONTROLS_MSR,
            val = get_msr(VMX_EXECUTION_TRUE_PROC_CONTROLS_MSR))

        print("VMX_TRUE_PROCBASED_CTLS ({0:#x}) MSR value : {1:#018x}".format(
            vmx_true_procbased_ctls_msr.num,
            vmx_true_procbased_ctls_msr.val
            ))
        procbased_ctls_to_use = vmx_true_procbased_ctls_msr
    else:
        print("Read VMX features support information from IA32_VMX_PROCBASED_CTLS MSR.")
        procbased_ctls_to_use = vmx_procbased_ctls_msr

    print(("Detailed information: \n"
          "    interrupt-window exiting (supported values) : %s %s, \n"
          "    use TSC offsetting (supported values) : %s %s, \n"
          "    HLT exiting (supported values) : %s %s, \n"
          "    INVLPG exiting (supported values) : %s %s, \n"
          "    MWAIT exiting (supported values) : %s %s, \n"
          "    RDPMC exiting (supported values) : %s %s, \n"
          "    RDTSC exiting (supported values) : %s %s, \n"
          "    CR3-load exiting (supported values) : %s %s, \n"
          "    CR3-store exiting (supported values) : %s %s, \n"
          "    CR8-load exiting (supported values) : %s %s, \n"
          "    CR8-store exiting (supported values) : %s %s, \n"
          "    use TPR shadow (supported values) : %s %s, \n"
          "    NMI-window exiting (supported values) : %s %s, \n"
          "    MOV-DR exiting (supported values) : %s %s, \n"
          "    unconditional I/O exiting (supported values) : %s %s, \n"
          "    use I/O bitmaps (supported values) : %s %s, \n"
          "    monitor trap flag (supported values) : %s %s, \n"
          "    use MSR bitmaps (supported values) : %s %s, \n"
          "    MONITOR exiting (supported values) : %s %s, \n"
          "    PAUSE exiting (supported values) : %s %s, \n"
          "    activate secondary controls (supported values) : %s %s. \n" %

           ("" if procbased_ctls_to_use[2] else "0",  "1" if procbased_ctls_to_use[32+2] else "",
           "" if procbased_ctls_to_use[3] else "0",  "1" if procbased_ctls_to_use[32+3] else "",
           "" if procbased_ctls_to_use[7] else "0",  "1" if procbased_ctls_to_use[32+7] else "",
           "" if procbased_ctls_to_use[9] else "0",  "1" if procbased_ctls_to_use[32+9] else "",
           "" if procbased_ctls_to_use[10] else "0",  "1" if procbased_ctls_to_use[32+10] else "",
           "" if procbased_ctls_to_use[11] else "0",  "1" if procbased_ctls_to_use[32+11] else "",
           "" if procbased_ctls_to_use[12] else "0",  "1" if procbased_ctls_to_use[32+12] else "",
           "" if procbased_ctls_to_use[15] else "0",  "1" if procbased_ctls_to_use[32+15] else "",
           "" if procbased_ctls_to_use[16] else "0",  "1" if procbased_ctls_to_use[32+16] else "",
           "" if procbased_ctls_to_use[19] else "0",  "1" if procbased_ctls_to_use[32+19] else "",
           "" if procbased_ctls_to_use[20] else "0",  "1" if procbased_ctls_to_use[32+20] else "",
           "" if procbased_ctls_to_use[21] else "0",  "1" if procbased_ctls_to_use[32+21] else "",
           "" if procbased_ctls_to_use[22] else "0",  "1" if procbased_ctls_to_use[32+22] else "",
           "" if procbased_ctls_to_use[23] else "0",  "1" if procbased_ctls_to_use[32+23] else "",
           "" if procbased_ctls_to_use[24] else "0",  "1" if procbased_ctls_to_use[32+24] else "",
           "" if procbased_ctls_to_use[25] else "0",  "1" if procbased_ctls_to_use[32+25] else "",
           "" if procbased_ctls_to_use[27] else "0",  "1" if procbased_ctls_to_use[32+27] else "",
           "" if procbased_ctls_to_use[28] else "0",  "1" if procbased_ctls_to_use[32+28] else "",
           "" if procbased_ctls_to_use[29] else "0",  "1" if procbased_ctls_to_use[32+29] else "",
           "" if procbased_ctls_to_use[30] else "0",  "1" if procbased_ctls_to_use[32+30] else "",
           "" if procbased_ctls_to_use[31] else "0",  "1" if procbased_ctls_to_use[32+31] else "",)
          ))

    #report features controlled by Secondary Processor-Based VM-Execution Controls
    isSecCtlsSupported = ((procbased_ctls_to_use[32+31]) != 0)
    if not isSecCtlsSupported:
        print("\nSecondary processor-based vm-execution controls are not supported.")
    else:
        print("\nSecondary Processor-Based VM-Execution Controls.")
        vmx_procbased_ctls2_msr = msr_obj_t(
            num = VMX_SECONDARY_EXECUTION_CONTROLS_MSR,
            val = get_msr(VMX_SECONDARY_EXECUTION_CONTROLS_MSR))

        print("VMX_PROCBASED_CTLS2 ({0:#x}) MSR value: {1:#018x}".format(
            vmx_procbased_ctls2_msr.num,
            vmx_procbased_ctls2_msr.val
            ))
        print(("Detailed information: \n"
            "\t virtualize APIC accesses (supported values) : %s %s, \n"
            "\t enable EPT (supported values) : %s %s, \n"
            "\t descriptor-table exiting (supported values) : %s %s, \n"
            "\t enable RDTSCP (supported values) : %s %s, \n"
            "\t virtualize x2APIC mode (supported values) : %s %s, \n"
            "\t enable VPID (supported values) : %s %s, \n"
            "\t WBINVD exiting (supported values) : %s %s, \n"
            "\t unrestricted guest (supported values) : %s %s, \n"
            "\t PAUSE-loop exiting (supported values) : %s %s. \n"
            "\t RDRAND exiting (supported values) : %s %s. \n"
            "\t enable INVPCID (supported values) : %s %s. \n"
            "\t enable VM functions (supported values) : %s %s." %

            ("" if vmx_procbased_ctls2_msr[0] else "0",  "1" if vmx_procbased_ctls2_msr[32+0] else "",
             "" if vmx_procbased_ctls2_msr[1] else "0",  "1" if vmx_procbased_ctls2_msr[32+1] else "",
             "" if vmx_procbased_ctls2_msr[2] else "0",  "1" if vmx_procbased_ctls2_msr[32+2] else "",
             "" if vmx_procbased_ctls2_msr[3] else "0",  "1" if vmx_procbased_ctls2_msr[32+3] else "",
             "" if vmx_procbased_ctls2_msr[4] else "0",  "1" if vmx_procbased_ctls2_msr[32+4] else "",
             "" if vmx_procbased_ctls2_msr[5] else "0",  "1" if vmx_procbased_ctls2_msr[32+5] else "",
             "" if vmx_procbased_ctls2_msr[6] else "0",  "1" if vmx_procbased_ctls2_msr[32+6] else "",
             "" if vmx_procbased_ctls2_msr[7] else "0",  "1" if vmx_procbased_ctls2_msr[32+7] else "",
             "" if vmx_procbased_ctls2_msr[10] else "0",  "1" if vmx_procbased_ctls2_msr[32+10] else "",
             "" if vmx_procbased_ctls2_msr[11] else "0",  "1" if vmx_procbased_ctls2_msr[32+11] else "",
             "" if vmx_procbased_ctls2_msr[12] else "0",  "1" if vmx_procbased_ctls2_msr[32+12] else "",
             "" if vmx_procbased_ctls2_msr[13] else "0",  "1" if vmx_procbased_ctls2_msr[32+13] else "",)
            ))

    #report information on VM-Exit Controls
    print("\nVM-Exit Controls.")
    vmx_exit_ctls_msr = msr_obj_t(
        num = VMX_EXIT_CONTROLS_MSR,
        val = get_msr(VMX_EXIT_CONTROLS_MSR))

    print("VMX_EXIT_CTLS ({0:#x}) MSR value: {1:#018x}".format(
        vmx_exit_ctls_msr.num,
        vmx_exit_ctls_msr.val
        ))
    if isDefault1toNullSupported:
        print("Read VMX features support information from IA32_VMX_TRUE_EXIT_CTLS MSR.")
        vmx_true_exit_ctls_msr = msr_obj_t(
            num = VMX_TRUE_EXIT_CONTROLS_MSR,
            val = get_msr(VMX_TRUE_EXIT_CONTROLS_MSR))

        print("VMX_TRUE_EXIT_CTLS ({0:#x}) MSR value: {1:#018x}".format(
            vmx_true_exit_ctls_msr.num,
            vmx_true_exit_ctls_msr.val
            ))
        exit_ctls_to_use = vmx_true_exit_ctls_msr
    else:
        print("Read VMX features support information from IA32_VMX_EXIT_CTLS MSR.")
        exit_ctls_to_use = vmx_exit_ctls_msr

    print(("Detailed information: \n"
           "    save debug controls (supported values) : %s %s, \n"
           "    host address-space size (supported values) : %s %s, \n"
           "    load IA32_PERF_GLOBAL_CTRL (supported values) : %s %s, \n"
           "    acknowledge interrupt on exit (supported values) : %s %s, \n"
           "    save IA32_PAT (supported values) : %s %s, \n"
           "    load IA32_PAT (supported values) : %s %s, \n"
           "    save IA32_EFER (supported values) : %s %s, \n"
           "    load IA32_EFER (supported values) : %s %s, \n"
           "    save VMX-preemption timer value (supported values) : %s %s. \n" %

            ("" if exit_ctls_to_use[2] else "0",  "1" if exit_ctls_to_use[32+2] else "",
             "" if exit_ctls_to_use[9] else "0",  "1" if exit_ctls_to_use[32+9] else "",
             "" if exit_ctls_to_use[12] else "0",  "1" if exit_ctls_to_use[32+12] else "",
             "" if exit_ctls_to_use[15] else "0",  "1" if exit_ctls_to_use[32+15] else "",
             "" if exit_ctls_to_use[18] else "0",  "1" if exit_ctls_to_use[32+18] else "",
             "" if exit_ctls_to_use[19] else "0",  "1" if exit_ctls_to_use[32+19] else "",
             "" if exit_ctls_to_use[20] else "0",  "1" if exit_ctls_to_use[32+20] else "",
             "" if exit_ctls_to_use[21] else "0",  "1" if exit_ctls_to_use[32+21] else "",
             "" if exit_ctls_to_use[22] else "0",  "1" if exit_ctls_to_use[32+22] else "",)
          ))

    #report information on VM-Entry Controls
    print("\nVM-Entry Controls.")
    vmx_entry_ctls_msr = msr_obj_t(
        num = VMX_ENTRY_CONTROLS_MSR,
        val = get_msr(VMX_ENTRY_CONTROLS_MSR))

    print("VMX_ENTRY_CTLS ({0:#x}) MSR value: {1:#018x}".format(
        vmx_entry_ctls_msr.num,
        vmx_entry_ctls_msr.val
        ))
    if isDefault1toNullSupported:
        print("Read VMX features support information from IA32_VMX_TRUE_ENTRY_CTLS MSR.\n")
        vmx_true_entry_ctls_msr = msr_obj_t(
            num = VMX_TRUE_ENTRY_CONTROLS_MSR,
            val = get_msr(VMX_TRUE_ENTRY_CONTROLS_MSR))

        print("VMX_TRUE_ENTRY_CTLS ({0:#x}) MSR value: {1:#018x}".format(
            vmx_true_entry_ctls_msr.num,
            vmx_true_entry_ctls_msr.val
            ))
        entry_ctls_to_use = vmx_true_entry_ctls_msr
    else:
        print("Read VMX features support information from IA32_VMX_ENTRY_CTLS MSR.\n")
        entry_ctls_to_use = vmx_entry_ctls_msr

    print(("Detailed information: \n"
          "\t load debug controls (supported values) : %s %s, \n"
          "\t IA-32e mode guest (supported values) : %s %s, \n"
          "\t entry to SMM (supported values) : %s %s, \n"
          "\t deactivate dual-monitor treatment (supported values) : %s %s, \n"
          "\t load IA32_PERF_GLOBAL_CTRL (supported values) : %s %s, \n"
          "\t load IA32_PAT (supported values) : %s %s, \n"
          "\t load IA32_EFER (supported values) : %s %s." %

           ("" if entry_ctls_to_use[2] else "0",  "1" if entry_ctls_to_use[32+2] else "",
           "" if entry_ctls_to_use[9] else "0",  "1" if entry_ctls_to_use[32+9] else "",
           "" if entry_ctls_to_use[10] else "0",  "1" if entry_ctls_to_use[32+10] else "",
           "" if entry_ctls_to_use[11] else "0",  "1" if entry_ctls_to_use[32+11] else "",
           "" if entry_ctls_to_use[13] else "0",  "1" if entry_ctls_to_use[32+13] else "",
           "" if entry_ctls_to_use[14] else "0",  "1" if entry_ctls_to_use[32+14] else "",
           "" if entry_ctls_to_use[15] else "0",  "1" if entry_ctls_to_use[32+15] else "",)
           ))


    #report information from IA32_VMX_MISC MSR
    print("\nMiscellaneous data.")
    vmx_misc_msr = msr_obj_t(
        num = VMX_MISCELLANEOUS_DATA_MSR,
        val = get_msr(VMX_MISCELLANEOUS_DATA_MSR))

    print("VMX_MISC ({0:#x}) MSR value: {1:#018x}.".format(
        vmx_misc_msr.num,
        vmx_misc_msr.val
        ))
    print(("Detailed information: \n"
          "    the rate of the VMX-preemption timer vs that of the timestamp counter (TSC): 0x%x, \n"
          "    do VM exits store IA32_EFER.LMA into the \"IA-32e mode guest\" VM-entry control: %s, \n"
          "    activity states support : \n"
          "        HLT : %s, \n"
          "        shutdown : %s, \n"
          "        wait-for-SIPI : %s, \n"
          "    can RDMSR instruction be used in SMM to read the IA32_SMBASE MSR (MSR address 9EH) : %s, \n"
          "    the number of CR3-target values supported : %u, \n"
          "    the recommended maximum number of MSRs that should appear in MSRs lists : %u (value of the field: %u), \n"
          "    MSEG revision : 0x%x." %

          (vmx_misc_msr[0:5],
           "yes" if vmx_misc_msr[5] else "no",
           "yes" if vmx_misc_msr[6] else "no",
           "yes" if vmx_misc_msr[7] else "no",
           "yes" if vmx_misc_msr[8] else "no",
           "yes" if vmx_misc_msr[15] else "no",
           vmx_misc_msr[16:25],
           512*(vmx_misc_msr[25:28] + 1), vmx_misc_msr[25:28],
           vmx_misc_msr[32:64])
          ))


    #report information from IA32_VMX_CR0_FIXED0, IA32_VMX_CR0_FIXED1, IA32_VMX_CR4_FIXED0, IA32_VMX_CR4_FIXED1 MSRs
    print("\n\"VMX-fixed bits in CR0\" and \"VMX-fixed bits in CR4\".")
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
        print(("%6d |" % (i,)), end=' ')
        print(("%4s|"  % (cr0Bits[i] if i < len(cr0Bits) else "rsvd",)), end=' ')
        if (vmx_cr0_fixed0_msr[i] == 0) and (vmx_cr0_fixed1_msr[i] == 0):
            print(("   to 0   |"), end=' ')
        elif (vmx_cr0_fixed0_msr[i] != 0) and (vmx_cr0_fixed1_msr[i] != 0):
            print(("   to 1   |"), end=' ')
        elif (vmx_cr0_fixed0_msr[i] == 0) and (vmx_cr0_fixed1_msr[i] != 0):
            print(("   not    |"), end=' ')
        else:
            print("\nOops, bug! Bits %d in IA32_VMX_CR0_FIXED0 MSR and in IA32_VMX_CR0_FIXED1 MSR are inconsistent.\n" % (i,))
        print(("%11s|" % (cr4Bits[i] if i < len(cr4Bits) else "rsvd",)), end=' ')
        if (vmx_cr4_fixed0_msr[i] == 0) and (vmx_cr4_fixed1_msr[i] == 0):
            print("   to 0   |")
        elif (vmx_cr4_fixed0_msr[i] != 0) and (vmx_cr4_fixed1_msr[i] != 0):
            print("   to 1   |")
        elif (vmx_cr4_fixed0_msr[i] == 0) and (vmx_cr4_fixed1_msr[i] != 0):
            print("   not    |")
        else:
            print("\nOops, bug! Bits %d in IA32_VMX_CR4_FIXED0 MSR and in IA32_VMX_CR4_FIXED1 MSR are inconsistent.\n" % (i,))

    #report information from IA32_VMX_VMCS_ENUM MSR
    print("\nVMCS enumeration.")
    vmx_vmcs_enum_msr = msr_obj_t(
        num = VMX_VMCS_ENUMERATION_MSR,
        val = get_msr(VMX_VMCS_ENUMERATION_MSR))

    print("IA32_VMX_VMCS_ENUM ({0:#x}) MSR value: {1:#018x}.".format(
        vmx_vmcs_enum_msr.num,
        vmx_vmcs_enum_msr.val
        ))

    print("The highest index value used for any VMCS encoding: %u.\n" % (vmx_vmcs_enum_msr[1:10],))

    #report information from IA32_VMX_EPT_VPID_CAP MSR
    print("\nVPID and EPT capabilities.")
    if not (procbased_ctls_to_use[63] and (vmx_procbased_ctls2_msr[33] or vmx_procbased_ctls2_msr[37])):
        print("CPU doesn't support VPID and EPT.\n")
    else:
        vmx_ept_vpid_cap_msr = msr_obj_t(
            num = VMX_VPID_EPT_CAP_MSR,
            val = get_msr(VMX_VPID_EPT_CAP_MSR))

        print("IA32_VMX_EPT_VPID_CAP ({0:#x}) MSR value: {1:#018x}.".format(
            vmx_ept_vpid_cap_msr.num,
            vmx_ept_vpid_cap_msr.val
            ))
        print(("Detailed information: \n"
               "    EPT execute-only translation support: %s, \n"
               "    EPT a page-walk length of 4 support: %s, \n"
               "    EPT paging-structure uncacheable (UC) memory type support : %s, \n"
               "    EPT paging-structure write-back (WB) memory type support : %s, \n"
               "    EPT 2-Mbyte pages support : %s, \n"
               "    EPT 1-Gbyte pages support : %s, \n"
               "    accessed and dirty flags for EPT support : %s, \n"
               "    INVEPT instruction support: %s "
               "(single-context : %s, all-context : %s), \n"
               "    INVVPID instruction support : %s "
               "(individual-address: %s, single-context: %s, "
               "all-context: %s, single-context-retaining-globals: %s)." %

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
              ))

    print("")


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
        print(("\tTop of memory 2 register: %s"
               % ("enabled" if obj.syscfg & (1 << 21) else "disabled")))
        print(("\tTop of memory and I/O range registers: %s"
               % ("enabled" if obj.syscfg & (1 << 20) else "disabled")))
        print(("\tRdDram and WrDram modification: %s"
               % ("enabled" if obj.syscfg & (1 << 19) else "disabled")))
        print(("\tRdDram and WrDram attributes: %s"
               % ("enabled" if obj.syscfg & (1 << 18) else "disabled")))
    if (SIM_class_has_attribute(obj.classname, "hwcr")):
        print("HWCR register")
        print(("\t32-bit address wrap: %s"
               % ("disabled" if obj.hwcr & (1 << 17) else "enabled")))
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
                print(("\tRead from: %s"
                       % ("DRAM" if SIM_get_attribute(obj, "iorr_base%d" % i)
                                    & (1 << 4)
                          else "I/O")))
                print(("\tWrite to: %s"
                       % ("DRAM" if SIM_get_attribute(obj, "iorr_base%d" % i)
                                    & (1 << 3)
                          else "I/O")))
            else:
                print("IORR%d invalid" % i)
    if (SIM_class_has_attribute(obj.classname, "mtrr_def_type")):
        print("MTRRdefType register")
        print(("\tMTRRs: %s"
               % ("enabled" if obj.mtrr_def_type & (1 << 11) else "disabled")))
        print(("\tFixed range MTRRs: %s"
               % ("enabled" if obj.mtrr_def_type & (1 << 10) else "disabled")))
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
                    print(("\t0x%08x - 0x%08x %s read: %s write: %s"
                           % (base + i * sub_len * 1024,
                              base + (i + 1) * sub_len * 1024 - 1,
                              mtrr_type_name(sub_field & 7),
                              "memory" if ((sub_field & (1 << 4))
                                           and (obj.syscfg & (1 << 18)))
                              else "I/O",
                              "memory" if ((sub_field & (1 << 3))
                                           and (obj.syscfg & (1 << 18)))
                              else "I/O")))
                else:
                    print(("\t0x%08x - 0x%08x %s"
                           % (base + i * sub_len * 1024,
                              base + (i + 1) * sub_len * 1024 - 1,
                              mtrr_type_name(sub_field))))
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

def parse_paging_entry(cpu, entry, entry_size):
    phys_mask = (1 << cpu.physical_bits) - 1
    addr = entry & phys_mask & 0x7ffffffffffff000
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
    return { "addr": addr, "p": p, "a": a, "d": d, "g": g, "nx": nx,
             "ps_pat": ps_pat, "r_w": r_w, "u_s": u_s }

def vmread(cpu, enc):
    for (key, v) in cpu.vmcs_content:
        if key == enc:
            return v

def ept_tablewalk(cpu, gpa, verbose, indent_count):
    VMCS_EPT_PTR                    = 0x201a
    indent = " " * indent_count

    def parse_ept_entry(cpu, entry, entry_size):
        phys_mask = (1 << cpu.physical_bits) - 1
        addr = entry & phys_mask & 0xfffffffff000
        x = (entry >> 2) & 1
        w = (entry >> 1) & 1
        r = (entry >> 0) & 1
        large = (entry >> 7) & 1
        return { "addr": addr, "x": x, "w": w, "r": r, "large": large }

    eptp = vmread(cpu, VMCS_EPT_PTR)
    eptp_base = eptp & ~0xfff
    level_info = ((39, "EPT PML4E", 0, 0x1ff, None, None),
                  (30, "EPT PDPE", 0, 0x1ff, 1*1024*1024*1024, "1GiB"),
                  (21, "EPT PDE", 1, 0x1ff, 2*1024*1024, "2MiB"),
                  (12, "EPT PTE", 0, 0x1ff, 4*1024, "4KiB"))
    levels = 4
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

        if level_info[level][4] != None:
            phys = parse_entry["addr"] + (gpa & (level_info[level][4] - 1))
        if parse_entry["large"]:
            if verbose:
                print("%s    %s page" % (indent, level_info[level][5]))
            return phys

        ptr = parse_entry["addr"]

    return phys

def linear_to_physical_cmd(cpu, lina):
    if cpu_has_efer(cpu):
        efer = cpu_get_efer(cpu)
    else:
        efer = 0

    VMCS_PROC_BASED_VM_EXEC_CTRLS   = 0x4002
    VMCS_SEC_PROC_BASED_CTLS        = 0x401e

    if hasattr(cpu, "vmx_mode") and cpu.vmx_mode == 2 and (vmread(cpu, VMCS_PROC_BASED_VM_EXEC_CTRLS) & (1 << 31)) and (vmread(cpu, VMCS_SEC_PROC_BASED_CTLS) & (1 << 1)):
        ept_enabled = True
    else:
        ept_enabled = False

    gpa = linear_to_physical_ex(cpu, lina, cpu.cr0, cpu.cr3, cpu.cr4, efer, ept_enabled, 1)

    if gpa == None: return
    if ept_enabled:
        pa = ept_tablewalk(cpu, gpa, 1, 0)
    else:
        pa = gpa

    if pa != None:
        print("Physical address 0x%x" % pa)

def linear_to_physical_ex(cpu, lina, cr0, cr3, cr4, efer, ept_enabled, verbose):
    mode_long = 0
    mode_pae = 0
    mode_classic = 0
    paging = (cr0 >> 31) & 1
    if (efer >> 10) & 1:
        mode_long = 1

    if paging:
        if mode_long:
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

    if mode_pae:
        level_info = ((30, "PDP", 0, 0x3), (21, "PD", 1, 0x1ff), (12, "PT", 0, 0x1ff))
        levels = 3
        entry_size = 8
        large_ps = 2
        ptr = cr3 & 0xfffffffffffff000
        u_s = 1
        r_w = 1
        nx = 0

    elif mode_long:
        level_info = ((39, "PML4", 0, 0x1ff), (30, "PDP", 1, 0x1ff), (21, "PD", 1, 0x1ff), (12, "PT", 0, 0x1ff))
        levels = 4
        entry_size = 8
        large_ps = 2
        ptr = cr3 & 0xfffffffffffff000
        u_s = 1
        r_w = 1
        nx = 0

    elif mode_classic:
        level_info = ((22, "PD", 1, 0x3ff), (12, "PT", 0, 0x3ff))
        levels = 2
        entry_size = 4
        large_ps = 4
        ptr = cr3 & 0xfffffffffffff000
        u_s = 1
        r_w = 1
        nx = 0

    else:
        if verbose:
            print("Paging disabled")
            print("Guest physical address 0x%x" % lina)
        return lina

    for level in range(levels):
        select = (lina >> level_info[level][0]) & level_info[level][3]
        addr = ptr + select * entry_size
        if verbose:
            print("%s" % level_info[level][1])
            print("    entry at guest physical 0x%x" % (addr))

        if ept_enabled:
            paddr = ept_tablewalk(cpu, addr, verbose, 4)
        else:
            paddr = addr

        if paddr == None:
            return None

        print("    entry at physical 0x%x" % paddr)

        entry = table_read(cpu.iface.processor_info.get_physical_memory(), paddr, entry_size)
        if verbose:
            print("    entry 0x%x" % entry)

        parse_entry = parse_paging_entry(cpu, entry, entry_size)

        u_s &= parse_entry["u_s"]
        r_w &= parse_entry["r_w"]
        nx |= parse_entry["nx"]

        if verbose:
            print("    addr = 0x%x u_s = %d r_w = %d p = %d" % (parse_entry["addr"], parse_entry["u_s"], parse_entry["r_w"], parse_entry["p"]))
        if not parse_entry["p"]:
            if verbose:
                print("    Not present")
            return None

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

    if not nxe:
        nx = 0

    if verbose:
        print("Guest physical address 0x%x (%s, %s%s)" % (phys, ("supervisor", "user")[u_s], ("read", "read/write")[r_w], ("/execute", "")[nx]))
    return phys

def x86_diff_regs(obj):
    if SIM_class_has_attribute(obj.classname, "rax"):
        gen_reg = ["rax", "rbx", "rcx", "rdx",
                   "rsp", "rbp", "rsi", "rdi",
                   "r8", "r9", "r10", "r11",
                   "r12", "r13", "r14", "r15", "eflags"]
    else:
        if obj.classname == "x86-186":
            gen_reg = ["ax", "bx", "cx", "dx",
                       "sp", "bp", "si", "di", "flags"]
        else:
            gen_reg = ["eax", "ebx", "ecx", "edx",
                       "esp", "ebp", "esi", "edi", "eflags"]
    return gen_reg + ["cr0", "cr2", "cr3", "cr4",
                      "cs", "cs_attr", "cs_base", "cs_limit",
                      "ds", "ds_attr", "ds_base", "ds_limit",
                      "ss", "ss_attr", "ss_base", "ss_limit",
                      "es", "es_attr", "es_base", "es_limit",
                      "fs", "fs_attr", "fs_base", "fs_limit",
                      "gs", "gs_attr", "gs_base", "gs_limit"]

def x86_get_info(obj):
    additional = [("Port space", obj.port_space)]
    if SIM_class_has_attribute(obj.classname, "physical_dram"):
        additional += [("DRAM space", obj.physical_dram)]
    if hasattr(obj, 'pause_slow_cycles'):
        if (obj.pause_slow_cycles == 0 and obj.rdtsc_slow_cycles == 0 and
            obj.port_io_slow_cycles == 0 and
            not obj.one_step_per_string_instruction):
            timing = "Classic"
        elif (obj.pause_slow_cycles == int(obj.freq_mhz * 10) and
              obj.rdtsc_slow_cycles == int(obj.freq_mhz * 10) and
              obj.port_io_slow_cycles == int(obj.freq_mhz * 10) and
              obj.one_step_per_string_instruction):
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
        if obj.init_vm_monitor:
            vmp_reason = obj.vm_disable_reason
            vmp_string = ""
            if vmp_reason != None:
                vmp_string = " (not engaged: %s)" % vmp_reason
            vmp_status = f"Enabled{vmp_string}"
        else:
            vmp_status = "Disabled"

        def replace(info_item):
            if info_item[0] == "VMP status":
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

def wait_for_processor_reset_command(obj):
    check_script_branch_command("wait-for-processor-reset")
    sb_wait_for_hap_internal(
                    '%s.wait-for-processor-reset' % obj.name,
                    "X86_Processor_Reset", obj, 0)

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
        print("Breaking on x86 processor reset in {} not enabled".format(
            obj.name))
    else:
        SIM_hap_delete_callback_id("X86_Processor_Reset",
                                   processor_reset_tracked[obj])
        del processor_reset_tracked[obj]

def register_x86_commands(x86_model):
    register_x86_base_commands(x86_model)
    register_pregs_sse(x86_model)
    register_pregs_fpu(x86_model)
    register_msrs(x86_model)
    register_print_gdt(x86_model)
    register_print_vmcs(x86_model)
    register_print_vmx_cap(x86_model)
    register_print_tss(x86_model)
    register_tablewalk(x86_model)
    new_info_command(x86_model, x86_get_info)
    if hasattr(sim_commands, "default_processor_get_status"):
        new_status_command(x86_model, sim_commands.default_processor_get_status)

def register_x86_base_commands(x86_model):
    new_command("wait-for-processor-reset", wait_for_processor_reset_command,
                cls = x86_model,
                type  = ["CLI"],
                short = "wait for a processor reset",
                see_also = ["script-branch"],
                doc = """
Postpones execution of a script branch until a processor
receives the INIT or RESET signal.
""")

    new_command("break-processor-reset", break_processor_reset_command,
                [],
                cls = x86_model,
                type  = ["CLI", "Breakpoints"],
                short = "break on processor reset",
                doc = """
Set the simulation to stop when this core receives the INIT or RESET signal.
""")
    new_command("unbreak-processor-reset", unbreak_processor_reset_command,
                [],
                cls = x86_model,
                type  = ["CLI", "Breakpoints"],
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

    # Make sure to register the global and namespace commands once
    if not simics_command_exists("trace-vmread"):
        register_vmcs_trackers()

def register_pregs_sse(x86_model):
    new_command("pregs-sse", pregs_sse_cmd,
                [arg(flag_t, "-s"), arg(flag_t, "-d"), arg(flag_t, "-f"),
                 arg(flag_t, "-x"), arg(flag_t, "-i"), arg(flag_t, "-b")],
                cls = x86_model,
                type = ["Processors", "Registers"],
                short = "print the sse registers",
                doc = """
Prints the contents of the SSE registers, using one subregister size flag and
one formatting flag.

The subregister size is selected using the either the <tt>-s</tt> flag
(32-bit), or the <tt>-d</tt> flag (64-bit).

The formatting flags select the formatting of the output. The <tt>-f</tt> flag
prints the floating-point values of the registers as decimal numbers. The
<tt>-x</tt> flag prints the contents of the registers as hexadecimal integers.
The <tt>-i</tt> flag prints the contents of the registers as decimal integers.
With the <tt>-b</tt> flag, the registers are printed in binary floating point
form.
""")

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

def register_msrs(x86_model):
    new_command("msrs", msrs_cmd,
                [],
                cls = x86_model,
                type = ["Processors", "Registers"],
                short = "print MSRs",
                doc = """
Print model specific registers. MSRs not included in the output
are either not supported, not implemented, read-only, or must
be accessed through other attributes.
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

def register_print_vmcs(x86_model):
    new_command("print-vmcs", vmcs_cmd,
                [arg(uint64_t, "vmcs-ptr", "?", None)],
                cls = x86_model,
                type = ["Processors"],
                see_also = [f"<{x86_model}>.print-vmx-cap"],
                short = "print VMCS",
                doc = """
Print all fields in the Intel&reg; Virtual Machine Control Structure Shadowing
(Intel&reg; VMCS Shadowing). If <arg>vmcs-ptr</arg> is given it should be a VMCS
pointer and
the VMCS is read from memory. Without argument and if this command is used in
VMX operation, the current VMCS is printed.""")

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

def register_tablewalk(x86_model):
    new_command("tablewalk", linear_to_physical_cmd,
                [arg(uint64_t, "address")],
                cls = x86_model,
                short = "address translation tablewalk",
                doc = """
Translate a linear <arg>address</arg> to a physical address. Traverses the
current paging table and prints information about every step along the way.
""")

common_disass = sim_commands.make_disassembly_fun(default_instr_len=1, virtual_address_prefix = "cs", get_opcode = x86_get_opcode)
def local_disass(cpu, prefix, address, *args):
    (length, ret) = common_disass(cpu, prefix, address, *args)

    if address == cpu.iface.processor_info.get_program_counter():
        pend_str = local_pending_exception(cpu)
        if pend_str != None:
            ret += ' # ' + pend_str
    return (length, ret)

def register_processor_cli_interface(x86_model):
    processor_cli_iface = processor_cli_interface_t()
    processor_cli_iface.get_disassembly = local_disass
    processor_cli_iface.get_pregs = local_pregs
    processor_cli_iface.get_diff_regs = x86_diff_regs
    processor_cli_iface.get_pending_exception_string = local_pending_exception
    processor_cli_iface.get_address_prefix = local_get_address_prefix
    processor_cli_iface.translate_to_physical = local_translate_to_physical
    SIM_register_interface(x86_model, 'processor_cli', processor_cli_iface)

def register_processor_gui_interface(x86_model):
    processor_gui_iface = processor_gui_interface_t()
    SIM_register_interface(x86_model, 'processor_gui', processor_gui_iface)

def register_opcode_info_interface(x86_model):
    opc_iface = opcode_info_interface_t()
    opc_iface.get_opcode_length_info = x86_get_opcode_length_info
    SIM_register_interface(x86_model, 'opcode_info', opc_iface)

def register_aprof_views(x86_model):
    x86_class = SIM_get_class(x86_model)
    sim_commands.register_aprof_views(x86_class)

def setup_processor_ui(x86_model):
    register_processor_cli_interface(x86_model)
    register_processor_gui_interface(x86_model)
    register_opcode_info_interface(x86_model)
    register_aprof_views(x86_model)
    register_x86_commands(x86_model)

#
# -------------------- trace-segreg, break-segreg --------------------
#

# order should match the X86_Descriptor_Change hap index
segment_descriptors = "es cs ss ds fs gs".split()

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

def register_cr_tracker(x86_model):
    x86_cr_tracker(0, "trace-segreg",
                   short = "trace segment register updates",
                   type = ["Tracing"],
                   see_also = [ "<%s>.break-segreg" % x86_model ],
                   x86_model = x86_model,
                   doc = """
Enables and disables tracing of segment register updates.  When this
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
                   see_also = [ "<%s>.trace-segreg" % x86_model ],
                   x86_model = x86_model,
                   doc = """
Enables and disables breaking simulation on segment register updates.
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
                       type= ["Tracing"],
                       see_also=[ "break-vmread",
                                    "trace-vmwrite" ],
                       doc="""
Enables and disables tracing of Intel&reg; Virtual Machine Control Structure
Shadowing (Intel&reg; VMCS Shadowing) reads. When this is enabled, every
time the specified VMCS
field is read during simulation a message is printed. The message will name
the VMCS field being read, and the value.

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
                       see_also=[ "trace-vmread",
                                    "break-vmwrite" ],
                       doc="""
Enables and disables breaking simulation on Intel&reg; Virtual Machine Control
Structure (Intel &reg; VMCS) reads. When this is enabled, every time the
specified VMCS field is read during simulation a message is printed. The
message will name the VMCS field being read, and the value.

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
                        see_also = [ "break-vmwrite",
                                     "trace-vmread" ],
                        doc = """
Enables and disables tracing of Intel&reg; Virtual Machine Control Structure
Shadowing (Intel&reg; VMCS Shadowing) updates. When this is enabled, every
time the specified
VMCS field is updated during simulation a message is printed. The message will
name the VMCS field being read, and the new value. The new value will be
printed even if it is identical to the previous value.

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
                   see_also = [ "trace-vmwrite",
                                "trace-vmread" ],
                   doc = """
Enables and disables breaking simulation on Intel&reg; Virtual Machine Control
Structure (Intel &reg; VMCS) updates. When this is enabled, every time the
specified VMCS field is updated during simulation a message is printed. The
message will name the VMCS field being read, and the new value. The new value
will be printed even if it is identical to the previous value.

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
