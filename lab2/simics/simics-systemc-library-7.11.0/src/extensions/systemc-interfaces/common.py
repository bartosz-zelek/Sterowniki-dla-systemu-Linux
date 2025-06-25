# © 2015 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


from simics import SIM_object_iterator_for_interface

def fmt_time(ns):
    us = ns / 1000.0
    ms = us / 1000.0
    s = ms / 1000.0
    if abs(ns) < 1000:
        return "%d ns" % (ns)
    elif abs(us) < 1000:
        return "%.3f us" % (us)
    elif abs(ms) < 1000:
        return "%.3f ms" % (ms)
    elif abs(s) < 60:
        return "%.3f s" % (s)
    days = int(s / (3600 * 24))
    s -= days * 3600 * 24
    hours = int(s / 3600)
    s -= hours * 3600
    min = int(s / 60)
    s -= min * 60
    res = ""
    if days:
        res += "%d d " % days
    if hours or days:
        res += "%2d h " % hours
    return res + "%2d m %2d s" % (min, s)

# return a sorted list of processes so that commands that display process
# information. Always list the processes in the same order.
def get_processes(context):
    return sorted([obj
                   for obj in SIM_object_iterator_for_interface(["sc_process"])
                   if obj.name.startswith(context.name + '.')])
