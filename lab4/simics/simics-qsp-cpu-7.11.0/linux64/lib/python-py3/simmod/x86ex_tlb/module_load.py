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


from cli import *
from simics import *

#
# ------------------------ info -----------------------
#

def calc_entries(tlb):
    return len(tlb) * len(tlb[0])

def calc_assoc(tlb):
    return len(tlb[0])

def get_info(obj):
    return [ (None,
              [ ("CPU", obj.cpu) ]) ]

new_info_command("x86ex-tlb", get_info)


def status_cmd(obj):
    entries_used = len(obj.tlb)
    if entries_used:
        if entries_used == 1 and len(obj.tlb[0])==0:
            print("TLB is empty")
            return
        print("%d PCIDs used" % (entries_used-1))
        tlb = obj.tlb
        for pcid_num in range(len(tlb)):
            pcid = tlb[pcid_num]
            for entry_num in range(len(pcid)):
                entry = pcid[entry_num]
                if entry_num == 0:
                    if pcid_num == 0:
                        print("GLOBAL:")
                        print("------- LA ------- ------- PA ------- ------ PTE ------- PAT  MTRR SIZE")
                    else:
                        print("PCID 0x%03x:"%(entry))
                        print("------- LA ------- ------- PA ------- ------ PTE ------- PAT  MTRR SIZE")
                        continue
                print("0x%016x 0x%016x 0x%016x %-4s %-4s %-4s" % (
                entry[0],
                entry[1],
                entry[2],
                entry[3],
                entry[4],
                {4: "4k", 2*1024: "2M", 4*1024: "4M", 1024*1024: "1G"}[entry[5]]))

    print()
    (adds, hits, misses, flushes) = obj.counters
    lookups = hits + misses
    if lookups != 0:
        print("Hit rate: %2.2f (%d hits per %d lookups)" % (
                                  float(hits)/lookups, hits, lookups))
        print("Add rate: %2.2f (%d adds per %d lookups)" % (
                        float(adds)/lookups, adds, lookups))
    if flushes != 0:
        print("%.2f lookups per flush (%d flushes per %d lookups)" % (
                        float(lookups)/flushes, flushes, lookups))

new_command("status", status_cmd,
            [],
            alias = "",
            short = "print status of the device",
            cls = "x86ex-tlb",
            doc = """
Print detailed information about the current status of the TLB object.<br/>
""")
