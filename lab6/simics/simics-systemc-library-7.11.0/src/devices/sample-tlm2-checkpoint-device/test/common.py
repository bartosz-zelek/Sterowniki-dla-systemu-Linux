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


import cli
import simics
import stest

logs = []

def create_device(device, name):
    return simics.SIM_create_object(device, name)

def enable_trace(name):
    cli.run_command('%s.checkpoint_device.responder.target.trace-sc' % name)
    cli.run_command('%s.checkpoint_device.requester.initiator.trace-sc' % name)

def add_logger():
    log = []
    def fun(data, src, len):
        for line in src.splitlines():
            log.append(line)
    simics.SIM_add_output_handler(fun, None)
    return log

def match_log_empty(log):
    stest.expect_equal(log, [])

def match_log(match, log):
    stest.expect_equal(log.pop(0)[:len(match)], match)

def match_log_initiator_to_target(name, log):
    stest.expect_equal(log.pop(0), '[%s.checkpoint_device.requester.initiator trace-nb-fw-out] write sz:4 addr:0x0 data:0x03020100 BEGIN_REQ' % name)
    stest.expect_equal(log.pop(0), '[%s.checkpoint_device.responder.target trace-nb-fw-in] write sz:4 addr:0x0 data:0x03020100 BEGIN_REQ' % name)
    stest.expect_equal(log.pop(0), '[%s.checkpoint_device.responder.target trace-nb-fw-out] write sz:4 addr:0x0 data:0x03020100 status:TLM_INCOMPLETE_RESPONSE TLM_ACCEPTED' % name)
    stest.expect_equal(log.pop(0), '[%s.checkpoint_device.requester.initiator trace-nb-fw-in] write sz:4 addr:0x0 data:0x03020100 status:TLM_INCOMPLETE_RESPONSE TLM_ACCEPTED' % name)

def match_log_target_to_initiator(name, log):
    stest.expect_equal(log.pop(0), '[%s.checkpoint_device.responder.target trace-nb-bw-out] write sz:4 addr:0x0 data:0x03020100 BEGIN_RESP' % name)
    stest.expect_equal(log.pop(0), '[%s.checkpoint_device.requester.initiator trace-nb-bw-in] write sz:4 addr:0x0 data:0x03020100 BEGIN_RESP' % name)
    stest.expect_equal(log.pop(0), '[%s.checkpoint_device.requester.initiator trace-nb-bw-out] write sz:4 addr:0x0 data:0x03020100 status:TLM_INCOMPLETE_RESPONSE TLM_UPDATED' % name)
    stest.expect_equal(log.pop(0), '[%s.checkpoint_device.responder.target trace-nb-bw-in] write sz:4 addr:0x0 data:0x03020100 status:TLM_INCOMPLETE_RESPONSE TLM_UPDATED' % name)

def match_delta_count(device, delta_count):
    stest.expect_equal(device.iface.sc_simcontext.delta_count(), delta_count)
