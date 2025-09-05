# INTEL CONFIDENTIAL

# © 2012 Intel Corporation
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

class_name = 'tm-snare'

#
# default cache status
# NOTE: most of the code below is irrelevant to the transaction cache
# semantics (like MESI states) and is here for legacy reasons.
#
def tm_snare_default_status_cmd(sn, cycle):
    assoc = sn.config_assoc
    line_number = sn.config_line_number
    line_size = sn.config_line_size
    set_nb = line_number//assoc

    print()
    print("RTM/HLE cache status:", sn.name)
    print("-------------")
    print("(M/E/S/I) for Modified/Exclusive/Shared/Invalid.")
    print("The value for each cache line is the base address, not the tag.")

    for i in range(set_nb):
        for j in range(assoc):
            cl = sn.lines[i + j * set_nb]
            if (cl[0] == 3):
                pr("M ")
            elif (cl[0] == 2):
                pr("E ")
            elif (cl[0] == 1):
                pr("S ")
            else:
                pr("I ")
            pr("0x%016x" % (cl[1] * line_size))
        print()
        if cycle:
            for j in range(assoc):
                lu = sn.lines_last_used[i + j*set_nb]
                pr("    %016d  " % lu)
            print()
    print()

#
# info
#
def tm_snare_default_info_cmd(sn):

    print()
    print("Cache Info:", sn.name)
    print("-----------")
    print("Number of cache lines :", sn.config_line_number)
    print("Cache line size       :", sn.config_line_size, "bytes")
    print("Total cache size      :", (sn.config_line_number * sn.config_line_size), "bytes")
    print("Associativity         :", sn.config_assoc)
    pr(   "Index                 : ")
    if sn.config_virtual_index:
        print("virtual")
    else:
        print("physical")
    pr(   "Tag                   : ")
    if sn.config_virtual_tag:
        print("virtual")
    else:
        print("physical")
    print()
    
#
# statistics
#
def tm_snare_default_stats_cmd(sn, ratio):
    # flush the STC counters before reporting stats
    sn.thread.iface.stc.flush_STC_inv(1)

    def prstat(s, val):
        print("%32s:%14s" % (s, number_str(val, 10)))
    def prstat_ratio(s, miss, total):
        print("%32s:%14.2f%%" % (s, 100.0 - (100.0 * miss) / total))

    print()
    print("Cache statistics:", sn.name)
    print("-----------------")

    prstat("Total number of transactions", sn.stat_transaction)
    print()
    prstat("Uncacheable device data reads (DMA)", sn.stat_dev_data_read)
    prstat("Uncacheable device data writes (DMA)", sn.stat_dev_data_write)
    print()
    prstat("Uncacheable data reads", sn.stat_uc_data_read)
    prstat("Uncacheable data writes", sn.stat_uc_data_write)
    prstat("Uncacheable instruction fetches", sn.stat_uc_inst_fetch)
    print()
    prstat("Data read transactions", sn.stat_data_read)
    prstat("Data read misses", sn.stat_data_read_miss)
    if (sn.stat_data_read > 0 and ratio):
        prstat_ratio("Data read hit ratio",
                     sn.stat_data_read_miss, sn.stat_data_read)
    print()
    prstat("Instruction fetch transactions", sn.stat_inst_fetch)
    prstat("Instruction fetch misses", sn.stat_inst_fetch_miss)
    if (sn.stat_inst_fetch > 0 and ratio):
        prstat_ratio("Instruction fetch hit ratio",
                     sn.stat_inst_fetch_miss, sn.stat_inst_fetch)
    print()
    prstat("Data write transactions", sn.stat_data_write)
    prstat("Data write misses", sn.stat_data_write_miss)
    if (sn.stat_data_write > 0 and ratio):
        prstat_ratio("Data write hit ratio",
                     sn.stat_data_write_miss, sn.stat_data_write)
    print()
    prstat("Copy back transactions", sn.stat_copy_back)
    print()

    try:
        if sn.stat_lost_stall_cycles:
            prstat("Lost Stall Cycles", sn.stat_lost_stall_cycles)
    except:
        pass
    
    try:
        if sn.snoopers:
            prstat("[MESI] Exclusive to Shared",
                   sn.stat_mesi_exclusive_to_shared)
            prstat("[MESI] Modified to Shared",
                   sn.stat_mesi_modified_to_shared)
            prstat("[MESI] Invalidates", sn.stat_mesi_invalidate)
    except:
        pass
    
#
# reset statistics
#
def tm_snare_reset_stats_cmd(sn):
    # flush the STC counters before resetting stats
    for c in sn.cpus:
        SIM_STC_flush_cache(c)

    sn.stat_transaction = 0
    
    sn.stat_dev_data_read = 0
    sn.stat_dev_data_write = 0
    
    sn.stat_uc_data_read = 0
    sn.stat_uc_data_write = 0
    sn.stat_uc_inst_fetch = 0
    
    sn.stat_data_read = 0
    sn.stat_data_read_miss = 0
    sn.stat_inst_fetch = 0
    sn.stat_inst_fetch_miss = 0
    sn.stat_data_write = 0
    sn.stat_data_write_miss = 0

    sn.stat_copy_back = 0

    try:
        sn.stat_mesi_exclusive_to_shared = 0
        sn.stat_mesi_modified_to_shared = 0
        sn.stat_mesi_invalidate = 0
    except:
        pass

#
# reset the cache lines
#
def tm_snare_default_reset_cache_lines_cmd(sn):
    # flush the STC before emptying the cache
    SIM_STC_flush_cache(sn.thread)
    for i in range(sn.config_line_number):
        # reset the line's values
        sn.lines[i] = [0,0,0,0]

#
# add a profiler to the cache
#
def type_expander(string):
    return get_completions(string,
                           ["data-read-miss-per-instruction",
                            "data-read-miss-per-virtual-address",
                            "data-read-miss-per-physical-address",
                            "data-write-miss-per-instruction",
                            "data-write-miss-per-virtual-address",
                            "data-write-miss-per-physical-address",
                            "instruction-fetch-miss-per-virtual-address",
                            "instruction-fetch-miss-per-physical-address"])

type_to_itype = {
    "data-read-miss-per-instruction"              : (0, 1, 0),
    "data-read-miss-per-virtual-address"          : (1, 0, 0),
    "data-read-miss-per-physical-address"         : (2, 0, 1),
    "data-write-miss-per-instruction"             : (3, 1, 0),
    "data-write-miss-per-virtual-address"         : (4, 0, 0),
    "data-write-miss-per-physical-address"        : (5, 0, 1),
    "instruction-fetch-miss-per-virtual-address"  : (6, 1, 0),
    "instruction-fetch-miss-per-physical-address" : (7, 1, 1)}

def tm_snare_define_cache_commands(device_name,
                             tm_snare_status_cmd,
                             tm_snare_info_cmd,
                             tm_snare_stats_cmd,
                             tm_snare_reset_cache_lines_cmd):
    # status
    if not tm_snare_status_cmd:
        tm_snare_status_cmd = tm_snare_default_status_cmd
    new_command("status", tm_snare_status_cmd,
                [arg(flag_t, "-cycle")],
                cls = device_name,
                short ="print the cache lines status",
                doc = """Print the cache lines status. <tt>-cycle</tt> prints
                         the last cycle each cache line was accessed.
                """)
    # info
    if not tm_snare_info_cmd:
        tm_snare_info_cmd = tm_snare_default_info_cmd
    new_command("info", tm_snare_info_cmd,
                [],
                cls = device_name,
                short="print the cache information",
                doc  = """
                Print configuration information for the cache.
                """)
    # statistics
    if not tm_snare_stats_cmd:
        tm_snare_stats_cmd = tm_snare_default_stats_cmd
    new_command("statistics", tm_snare_stats_cmd,
                [],
                cls = device_name,
                short="print the cache statistics",
                doc  = """
                Print the current value of the cache statistics (this will flush the STC counters to report correct values).
                """)
    # reset-statistics
    new_command("reset-statistics", tm_snare_reset_stats_cmd,
                [],
                cls = device_name,
                short="reset the cache statistics",
                doc  = """
                Reset the cache statistics.
                """)
    # reset-cache-lines
    if not tm_snare_reset_cache_lines_cmd:
        tm_snare_reset_cache_lines_cmd = tm_snare_default_reset_cache_lines_cmd
    new_command("reset-cache-lines", tm_snare_reset_cache_lines_cmd,
                [],
                cls = device_name,
                short="reset all the cache lines",
                doc  = """
                Reset all the cache lines to their default values.
                """)

def snare_info_cmd(sn):
    tm_snare_default_info_cmd(sn)
    c_list = sn.snoopers
    if c_list:
        pr(   "Snoopers              : ")
        for c in c_list:
            pr(c.name + " ")
        pr("\n")

    pr(   "Overlaid memory space: ")
    pr(sn.physical_memory.name + " ")
    pr("\n")

    pr(   "Transactions store device: ")
    pr(sn.tx_store.name + " ")
    pr("\n")

def snare_stats_cmd(sn):
    tm_snare_default_stats_cmd(sn, 1)

tm_snare_define_cache_commands(class_name, None, snare_info_cmd, snare_stats_cmd, None)
