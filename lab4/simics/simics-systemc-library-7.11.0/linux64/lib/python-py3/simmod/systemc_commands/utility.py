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


# Utility functions for creating custom info and status commands

# only expose the get_status right now, the user can directly invoke the
# methods of sc_simcontext interface directly from their own status commands
# should the provided one not be good enough
__all__ = ('get_status',)

import cli

def time_stamp(obj):
    """Returns the current SystemC time in PS, as a string"""
    return cli.number_str(obj.iface.sc_simcontext.time_stamp()) + " ps"

def delta_count(obj):
    """Returns the delta count, as a string"""
    return cli.number_str(obj.iface.sc_simcontext.delta_count())

def time_to_pending_activity(obj):
    """Returns the relative time to next SystemC event, as a string. Or "<no
pending activity>" if there is no pending activity"""
    t = obj.iface.sc_simcontext.time_to_pending_activity()
    if obj.iface.sc_simcontext.time_stamp() + t == (1 << 64) - 1:
        return "<no pending activity>"
    else:
        return cli.number_str(obj.iface.sc_simcontext.time_to_pending_activity()) + " ps"

def get_status(obj):
    """Default status command with just the basic time/event information"""
    return [("SystemC",
             [ ("Time", time_stamp(obj)),
               ("Delta count", delta_count(obj)),
               ("Time to pending activity", time_to_pending_activity(obj))])]
