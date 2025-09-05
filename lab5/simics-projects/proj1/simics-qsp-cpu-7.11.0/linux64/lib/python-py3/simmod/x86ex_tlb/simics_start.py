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


from simics import *
from update_checkpoint import *

def upgrade_to_single_mode_tlb(obj):
    def get_entries(tlb_attr, page_size_k):
        if tlb_attr == None:
            return []
        ret = []

        for x in tlb_attr:
            for (la, pa, mode, rw, g, pat, mtrr) in x:
                if (la & ((page_size_k * 1024) - 1)) != 0:
                    print("Checkpoint has page not aligned on %d KiB. Ignoring attribute." % page_size_k)
                    return []

        for x in tlb_attr:
            for (la, pa, mode, rw, g, pat, mtrr) in x:
                access = (Sim_Access_Read | Sim_Access_Execute)
                if rw == 0: # rw == 1 -> Read
                    access |= Sim_Access_Write
                supervisor_access = access
                if mode: # mode == 1 -> Supervisor
                    user_access = 0
                else:
                    user_access = access
                entry = [la, pa, supervisor_access, user_access, g, pat,
                         mtrr, page_size_k]
                ret.append(entry)
        return ret

    if not hasattr(obj, "large_tlb_select"):
        # Updater has already been applied
        return

    remove_attr(obj, "type")
    large_mb = obj.large_tlb_select
    remove_attr(obj, "large_tlb_select")
    tlb = []
    if hasattr(obj, "itlb_large"):
        tlb += get_entries(obj.itlb_large, large_mb * 1024)
    if hasattr(obj, "itlb_4k"):
        tlb += get_entries(obj.itlb_4k, 4)
    if hasattr(obj, "itlb_1g"):
        tlb += get_entries(obj.itlb_1g, 1024*1024)
    if hasattr(obj, "dtlb_large"):
        tlb += get_entries(obj.dtlb_large, large_mb * 1024)
    if hasattr(obj, "dtlb_4k"):
        tlb += get_entries(obj.dtlb_4k, 4)
    if hasattr(obj, "dtlb_1g"):
        tlb += get_entries(obj.dtlb_1g, 1024*1024)
    obj.tlb = tlb
    remove_attr(obj, "itlb_large")
    remove_attr(obj, "dtlb_large")
    remove_attr(obj, "itlb_4k")
    remove_attr(obj, "dtlb_4k")
    remove_attr(obj, "itlb_1g")
    remove_attr(obj, "dtlb_1g")

SIM_register_class_update(4580, "x86ex-tlb", upgrade_to_single_mode_tlb)
