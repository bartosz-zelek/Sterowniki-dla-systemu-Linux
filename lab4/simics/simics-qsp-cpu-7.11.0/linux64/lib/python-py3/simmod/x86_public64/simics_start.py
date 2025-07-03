# INTEL CONFIDENTIAL

# © 2016 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import re
import struct
import update_checkpoint

cpu_classes = ['x86-silvermont', 'x86-airmont', 'x86-sandybridge',
               'x86-haswell', 'x86-haswell-xeon', 'x86-broadwell-xeon',
               'x86-knights-landing', 'x86-knights-mill', 'x86-skylake',
               'x86QSP2', 'x86-goldmont', 'x86-goldmont-plus',
               'x86-cooper-lake', 'x86-skylake-server', 'x86-coffee-lake',
               'x86-coffee-lake-server', 'x86-experimental-fred',
               'x86-goldencove-server', 'x86-tnt', 'x86-tnt-lkf',
               'x86-snc', 'x86-snc-smt4', 'x86-snc-client', 'x86-snc-lkf',
               'x86-wlc', 'x86ex-cnl', 'x86-glc', 'x86-glc-adl', 'x86-grt-adl']

def remove_vcr_attr(obj):
    update_checkpoint.remove_attr(obj, "vcr_cache")

def remove_field_postfix_from_vmcs_names(obj):
    for i in range(len(obj.vmcs_layout)):
        field_index = obj.vmcs_layout[i][0]
        if field_index not in [0x4016, 0x4408]:
            continue

        field_name = obj.vmcs_layout[i][1]
        field_name = re.sub(' field$', '', field_name)
        obj.vmcs_layout[i][1] = field_name

def update_vmx_state(obj):
    if hasattr(obj, 'vmcs_content'):
        index_map = {
            0x0: 0x8d, 0x6c16: 0x78, 0x201a: 0x7e, 0x2c04: 0x7b,
            0x6804: 0x5b, 0x203c: 0xa1, 0xc0c: 0x10, 0x480c: 0x3f,
            0x6826: 0x6c, 0xfffffffc: 0xb4, 0x2814: 0x20, 0x482e: 0x90,
            0x2402: 0xb7, 0x6002: 0x50, 0x400a: 0x27, 0x6c0e: 0x74,
            0x2012: 0x18, 0x2034: 0x9e, 0xc04: 0xc, 0x681e: 0x68,
            0x4804: 0x3b, 0x40000006: 0x7c, 0x280c: 0x81, 0x4826: 0x4c,
            0x814: 0xac, 0x4002: 0x23, 0x6c06: 0x70, 0x200a: 0x16,
            0x4024: 0xab, 0x202c: 0x19, 0x6816: 0x64, 0x2804: 0x1c,
            0x481e: 0x48, 0x6404: 0x8a, 0x80c: 0x8, 0x440c: 0x37,
            0xfffffff9: 0xb5, 0x2002: 0x13, 0x401c: 0x30, 0x2024: 0x98,
            0x680e: 0x60, 0x4816: 0x44, 0x804: 0x4, 0x4404: 0x33,
            0x600c: 0x55, 0x4014: 0x2c, 0x6c18: 0xa8, 0x2: 0x8e,
            0x201c: 0x94, 0x2c06: 0xa4, 0x6806: 0x5c, 0x203e: 0xb3,
            0x480e: 0x40, 0x6828: 0xa5, 0x2816: 0x21, 0x4830: 0xb8,
            0x6004: 0x51, 0x400c: 0x28, 0x6c10: 0x75, 0x2014: 0x91,
            0x2036: 0x9c, 0xc06: 0xd, 0x4806: 0x3c, 0x6820: 0x69,
            0x4828: 0xb9, 0x280e: 0x82, 0x4004: 0x24, 0x200c: 0xb6,
            0x6c08: 0x71, 0x4026: 0xa2, 0x202e: 0x9b, 0x6818: 0x65,
            0x2806: 0x1d, 0x6406: 0x8b, 0x4820: 0x49, 0x80e: 0x9,
            0x440e: 0x38, 0x6c00: 0x6d, 0x2004: 0x7d, 0x401e: 0x84,
            0x2026: 0x87, 0x6810: 0x61, 0x4818: 0x45, 0x806: 0x5,
            0x4406: 0x34, 0x600e: 0x56, 0x4016: 0x2d, 0x6c1a: 0xa9,
            0x4: 0x99, 0x4c00: 0x4e, 0x201e: 0x95, 0x6808: 0x5d,
            0x2040: 0x9f, 0x4810: 0x41, 0x682a: 0xa6, 0x2818: 0xa3,
            0x6006: 0x52, 0x400e: 0x29, 0x6c12: 0x76, 0x2016: 0xaf,
            0x2c00: 0x79, 0x6800: 0x59, 0x2038: 0xad, 0xc08: 0xe,
            0x4808: 0x3d, 0x6822: 0x6a, 0x2810: 0x83, 0x482a: 0x4d,
            0x4006: 0x25, 0x6c0a: 0x72, 0x200e: 0x9d, 0x4028: 0xba,
            0x2030: 0x92, 0xc00: 0xa, 0x4800: 0x39, 0x681a: 0x66,
            0x40000002: 0x0, 0x2808: 0x1e, 0x4822: 0x4a, 0x6408: 0x8c,
            0x810: 0x11, 0x6c02: 0x6e, 0x2006: 0x14, 0x4020: 0x85,
            0x2028: 0x88, 0x6812: 0x62, 0x2800: 0x1a, 0x481a: 0x46,
            0x6400: 0x57, 0x808: 0x6, 0x4408: 0x35, 0x4018: 0x2e,
            0x6c1c: 0xaa, 0x6: 0xa0, 0x2020: 0x96, 0x680a: 0x5e,
            0x2042: 0xb0, 0x682c: 0xa7, 0x4812: 0x42, 0x800: 0x2,
            0x4400: 0x31, 0x6008: 0x53, 0x4010: 0x2a, 0x6c14: 0x77,
            0x2018: 0x93, 0x2c02: 0x7a, 0x6802: 0x5a, 0x203a: 0xae,
            0xc0a: 0xf, 0x480a: 0x3e, 0x6824: 0x6b, 0x2812: 0x1f,
            0x2400: 0x7f, 0x6000: 0x4f, 0x4008: 0x26, 0x6c0c: 0x73,
            0x2010: 0x17, 0x2032: 0xb2, 0xc02: 0xb, 0x4802: 0x3a,
            0x681c: 0x67, 0x40000004: 0x1, 0x280a: 0x80, 0x640a: 0x58,
            0x4824: 0x4b, 0x812: 0x8f, 0x4000: 0x22, 0x6c04: 0x6f,
            0x2008: 0x15, 0x4022: 0x86, 0x202a: 0x9a, 0x6814: 0x63,
            0x2802: 0x1b, 0x6402: 0x89, 0x481c: 0x47, 0x80a: 0x7,
            0x440a: 0x36, 0x2000: 0x12, 0x401a: 0x2f, 0x8: 0xb1,
            0x2022: 0x97, 0x680c: 0x5f, 0x4814: 0x43, 0x802: 0x3,
            0x4402: 0x32, 0x600a: 0x54, 0x4012: 0x2b
        }

        if obj.vmx_mode == 0:
            update_checkpoint.remove_attr(obj, 'vmxon_ptr')
            update_checkpoint.remove_attr(obj, 'current_vmcs_ptr')
            update_checkpoint.remove_attr(obj, 'vmcs_content')
        else:
            obj.vmcs_cache = [[obj.current_vmcs_ptr,
                [[index_map[f[0]], f[1]] for f in obj.vmcs_content]]]
            update_checkpoint.remove_attr(obj, 'vmcs_content')

for cc in cpu_classes:
    update_checkpoint.SIM_register_class_update(
        6095, cc, remove_field_postfix_from_vmcs_names)

    update_checkpoint.SIM_register_class_update(
        6116, cc, update_vmx_state)

public_cores_with_vcrs = ["x86-snc-smt4", "x86-glc", "x86-snc", 'x86-glc-adl', 'x86-grt-adl',
                          "x86-wlc", "x86-tnt", "x86-snc-client", "x86ex-cnl"]

for cc in public_cores_with_vcrs:
    update_checkpoint.SIM_register_class_update(
        6248, cc, remove_vcr_attr)

def update_perf_global_msr_names(obj):
    obj.ia32_perf_global_status_set = obj.msr_391h
    update_checkpoint.remove_attr(obj, "msr_391h")
    obj.ia32_perf_global_inuse = obj.msr_392h
    update_checkpoint.remove_attr(obj, "msr_392h")

def remove_cpu_msr_topology_attr(obj):
    update_checkpoint.remove_attr(obj, 'cpu_msr_topology_space')

def remove_fusa_regs(obj):
    update_checkpoint.remove_attr(obj, 'msr_fusarr_base')
    update_checkpoint.remove_attr(obj, 'msr_fusarr_mask')

for cc in ['x86-skylake-server', 'x86-haswell-xeon', 'x86-goldmont',
           'x86-goldmont-plus', 'x86-coffee-lake', 'x86-coffee-lake-server',
           'x86-broadwell-xeon', 'x86-cooper-lake', 'x86-sandybridge',
           'x86QSP2', 'x86-skylake', 'x86-knights-landing', 'x86-haswell',
           'x86-knights-mill', 'x86-airmont', 'x86-experimental-fred',
           'x86-silvermont', 'x86-goldencove-server']:
    update_checkpoint.SIM_register_class_update(
        6127, cc, remove_cpu_msr_topology_attr)

for cc in ['x86-goldencove-server']:
    update_checkpoint.SIM_register_class_update(
        6127, cc, remove_fusa_regs)

for cc in  ['x86-goldmont', 'x86-goldmont-plus']:
    update_checkpoint.SIM_register_class_update(
        6101, cc, update_perf_global_msr_names)

def remove_perf_metrics_msr(obj):
    update_checkpoint.remove_attr(obj, "perf_metrics")

update_checkpoint.SIM_register_class_update(
        6101, 'x86-cooper-lake', remove_perf_metrics_msr)

def update_vmcs_layout_arbyte(obj):
    arbyte = [0x4814, 0x4816, 0x4818, 0x481a, 0x481c, 0x481e, 0x4820, 0x4822]

    for i in range(len(obj.vmcs_layout)):
        index = obj.vmcs_layout[i][0]
        if index in arbyte:
            obj.vmcs_layout[i][2] = 2

for cc in  ['x86-skylake-server', 'x86-coffee-lake', 'x86-coffee-lake-server']:
    update_checkpoint.SIM_register_class_update(
        6117, cc, update_vmcs_layout_arbyte)

def remove_msr_ia32_l2_qos_ext_bw_thrtl(obj):
    for i in range(16):
        update_checkpoint.remove_attr(obj,
                                      f"msr_ia32_l2_qos_ext_bw_thrtl_{str(i)}")

for cc in ['x86-goldmont',
           'x86-goldmont-plus',
           'x86-haswell-xeon',
           'x86-broadwell-xeon']:
    update_checkpoint.SIM_register_class_update(
        6125, cc, remove_msr_ia32_l2_qos_ext_bw_thrtl)

def update_msr_ia32_smrr2_names(obj):
    if hasattr(obj, 'msr_1f6h'):
        obj.ia32_smrr2_physbase = obj.msr_1f6h
        update_checkpoint.remove_attr(obj, 'msr_1f6h')
    if hasattr(obj, 'msr_1f7h'):
        obj.ia32_smrr2_physmask = obj.msr_1f7h
        update_checkpoint.remove_attr(obj, 'msr_1f7h')

for cc in cpu_classes:
    update_checkpoint.SIM_register_class_update(
        6179, cc, update_msr_ia32_smrr2_names)

def remove_msr_smrr(obj):
    if hasattr(obj, 'ia32_mtrrcap'):
        obj.ia32_mtrrcap &= ~((1 << 13) | (1 << 11))
    for smr_attr in (
        'ia32_smrr_physbase',
        'ia32_smrr_physmask',
        'ia32_smrr2_physbase',
        'ia32_smrr2_physmask',
    ):
        if hasattr(obj, smr_attr):
            update_checkpoint.remove_attr(obj, smr_attr)

for cc in ['x86-goldmont',
           'x86-goldmont-plus',
           'x86-sandybridge']:
    update_checkpoint.SIM_register_class_update(
        6180, cc, remove_msr_smrr)

def remove_msr_smrr2(obj):
    for smr_attr in (
        'ia32_smrr2_physbase',
        'ia32_smrr2_physmask',
        'msr_4e0h',
    ):
        if hasattr(obj, smr_attr):
            update_checkpoint.remove_attr(obj, smr_attr)

for cc in ['x86-haswell',
           'x86-haswell-xeon']:
    update_checkpoint.SIM_register_class_update(
        6180, cc, remove_msr_smrr2)

def remove_msr_integrity_capabilities(obj):
    update_checkpoint.remove_attr(obj, "msr_2d9h")

for cc in ['x86-glc-adl', 'x86-grt-adl']:
    update_checkpoint.SIM_register_class_update(
        6243, cc, remove_msr_integrity_capabilities)

def remove_sgx_crypto_sha256(obj):
    if hasattr(obj, "sgx_crypto_sha256"):
        update_checkpoint.remove_attr(obj, "sgx_crypto_sha256")

for cc in cpu_classes:
    update_checkpoint.SIM_register_class_update(
        6269, cc, remove_sgx_crypto_sha256)

def remove_sgx_attr(obj):
    for attr in (
        "sgx_abort_page_address",
        "sgx_attributes_mask",
        "sgx_cpusvn",
        "sgx_fk0_unwrapped",
        "sgx_fk1",
        "sgx_ownerepoch",
        "sgx_pr_reset_key",
        "sgx_prmrr_address_map",
        "sgx_pubkeyhash",
        "sgx_report_keyid",
        "sgx_state",
        "sgx_threads",
    ):
        if hasattr(obj, attr):
            update_checkpoint.remove_attr(obj, attr)

for cc in ['x86-glc-adl']:
    update_checkpoint.SIM_register_class_update(
        6269, cc, remove_sgx_attr)

def remove_sgx_fk0_and_gkek_attr(obj):
    for attr in (
        "sgx_fk0",
        "sgx_gkek",
    ):
        if hasattr(obj, attr):
            update_checkpoint.remove_attr(obj, attr)

for cc in ['x86-glc-adl']:
    update_checkpoint.SIM_register_class_update(
        6302, cc, remove_sgx_fk0_and_gkek_attr)

def remove_sgx_flags_attr(obj):
    if hasattr(obj, "sgx_flags"):
        update_checkpoint.remove_attr(obj, "sgx_flags")

for cc in ['x86-glc-adl']:
    update_checkpoint.SIM_register_class_update(
        6307, cc, remove_sgx_flags_attr)

def upgrade_rtit_state_attr(obj):
    named_rtit_attrs = [
        {
            'names': ['rtit_tsc_en', 'rtit_ptw_en', 'rtit_fup_on_ptw',
                      'rtit_mtc_en', 'rtit_mtc_freq', 'rtit_psb_threshold'],
            'format': '<????BQ'},
        {'names': ['rtit_tnt_disable'], 'format': '?'}]

    if not hasattr(obj, 'rtit_state'):
        return

    data = tuple(struct.pack('<' + '?'*15 + 'QLLQQQQQ?Q', *obj.rtit_state))

    for batch in named_rtit_attrs:
        for attr in batch['names']:
            if not hasattr(obj, attr):
                obj.rtit_state = data
                return
        values = [getattr(obj, attr) for attr in batch['names']]
        data += tuple(struct.pack(batch['format'], *values))
        for attr in batch['names']:
            update_checkpoint.remove_attr(obj, attr)

    obj.rtit_state = data

for cc in cpu_classes:
    update_checkpoint.SIM_register_class_update(
        6342, cc, upgrade_rtit_state_attr)

def remove_msr_ia32_sgxlepubkeyhash(obj):
    for attr_name in [f"msr_{x:x}h" for x in range(0x8c, 0x92)]:
        if hasattr(obj, attr_name):
            update_checkpoint.remove_attr(obj, attr_name)

for cc in cpu_classes:
    update_checkpoint.SIM_register_class_update(
        6307, cc, remove_msr_ia32_sgxlepubkeyhash
    )
