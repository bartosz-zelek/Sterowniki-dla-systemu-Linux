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


import update_checkpoint

def remove_injection_attr(obj):
    update_checkpoint.remove_attr(obj, "pending_exception_injection")

def remove_smi_pending(obj):
    update_checkpoint.remove_attr(obj, "smi_pending")

def monitor_bit_from_ia32_misc_enable(obj):
    if obj.classname in ('x86-core', 'x86-core2', 'x86-nehalem', 'x86-intel64', 'x86-nehalem-xeon', 'x86QSP1'):
        misc_enable = getattr(obj, "ia32_misc_enable")
        misc_enable |= (1 << 18) # The monitor/mwait bit now lives in
                                 # the IA32_MISC_ENABLE MSR
        setattr(obj, "ia32_misc_enable", misc_enable)

def remove_mai_attrs(obj):
    update_checkpoint.remove_attr(obj, "reorder_buffer_size")

def remove_fpu_pointer_stack(obj):
    if hasattr(obj, "fpu_commit_last_instr"):
        commit_last = getattr(obj, "fpu_commit_last_instr")
        if commit_last:
            instr_selector = getattr(obj, "fpu_last_instr_selector1")
            instr_pointer = getattr(obj, "fpu_last_instr_pointer1")
            opcode = getattr(obj, "fpu_last_opcode1")
            operand_selector = getattr(obj, "fpu_last_operand_selector1")
            operand_pointer = getattr(obj, "fpu_last_operand_pointer1")
        else:
            instr_selector = getattr(obj, "fpu_last_instr_selector0")
            instr_pointer = getattr(obj, "fpu_last_instr_pointer0")
            opcode = getattr(obj, "fpu_last_opcode0")
            operand_selector = getattr(obj, "fpu_last_operand_selector0")
            operand_pointer = getattr(obj, "fpu_last_operand_pointer0")
        update_checkpoint.remove_attr(obj, "fpu_last_instr_selector0")
        update_checkpoint.remove_attr(obj, "fpu_last_instr_selector1")
        update_checkpoint.remove_attr(obj, "fpu_last_instr_pointer0")
        update_checkpoint.remove_attr(obj, "fpu_last_instr_pointer1")
        update_checkpoint.remove_attr(obj, "fpu_last_opcode0")
        update_checkpoint.remove_attr(obj, "fpu_last_opcode1")
        update_checkpoint.remove_attr(obj, "fpu_last_operand_selector0")
        update_checkpoint.remove_attr(obj, "fpu_last_operand_selector1")
        update_checkpoint.remove_attr(obj, "fpu_last_operand_pointer0")
        update_checkpoint.remove_attr(obj, "fpu_last_operand_pointer1")
        update_checkpoint.remove_attr(obj, "fpu_commit_last_instr")
        setattr(obj, "fpu_last_instr_selector", instr_selector)
        setattr(obj, "fpu_last_instr_pointer", instr_pointer)
        setattr(obj, "fpu_last_opcode", opcode)
        setattr(obj, "fpu_last_operand_selector", operand_selector)
        setattr(obj, "fpu_last_operand_pointer", operand_pointer)

def unified_segment_register_format(obj):
    segments = ("cs", "ds", "ss", "es", "fs", "gs", "tr", "ldtr")
    for segment in segments:
        seg = getattr(obj, segment)
        if len(seg) == 10:
            seg.append(0)  # Add 'l' bit. Always zero since this must
                           # be a 32-bit architecture
        setattr(obj, segment, seg)

def remove_bsp(obj):
    update_checkpoint.remove_attr(obj, "bsp")

def remove_vm_seen_1gb(obj):
    update_checkpoint.remove_attr(obj, "vm_seen_1gb_pages")

def remove_cpu_group(obj):
    update_checkpoint.remove_attr(obj, "cpu_group")

def mperf_aperf_mod(obj):
    # These attributes are supposed to store information relevant to
    # aperf and mperf, but in reality the mechanism for using them is
    # so broken so we can just as well remove them and start the MSRs
    # with default values (based on cycle count) when restoring from
    # checkpoint.
    update_checkpoint.remove_attr(obj, "aperf_last_clear")
    update_checkpoint.remove_attr(obj, "mperf_last_clear")

def x86_register_configuration_update(classname):
    update_checkpoint.install_class_configuration_update(3534, classname,
        remove_injection_attr)
    update_checkpoint.install_class_configuration_update(4043, classname,
        remove_smi_pending)
    update_checkpoint.install_class_configuration_update(4071, classname,
        monitor_bit_from_ia32_misc_enable)
    update_checkpoint.install_class_configuration_update(4550, classname,
                                                         remove_mai_attrs)
    update_checkpoint.install_class_configuration_update(4595, classname,
                                                         remove_vm_seen_1gb)
    update_checkpoint.install_class_configuration_update(5000, classname,
        remove_fpu_pointer_stack)
    update_checkpoint.install_class_configuration_update(5000, classname,
        unified_segment_register_format)
    update_checkpoint.SIM_register_class_update(5000, classname, remove_bsp)
    update_checkpoint.SIM_register_class_update(5023, classname, remove_cpu_group)
    update_checkpoint.SIM_register_class_update(5112, classname, mperf_aperf_mod)
