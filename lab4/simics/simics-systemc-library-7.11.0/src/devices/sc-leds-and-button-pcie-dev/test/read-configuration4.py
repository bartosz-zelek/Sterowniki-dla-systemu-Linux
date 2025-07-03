# © 2017 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


'''Load checkpoint state 4 and check the state is same as when writing
the checkpoint.'''

from checkpoint import check_state, load_checkpoint, run_next_ckpt_test

load_checkpoint(4)
check_state(4)
