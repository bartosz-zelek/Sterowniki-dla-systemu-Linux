# © 2022 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

import dev_util
import simics
import stest
import cli

# Test: mimic the creation from a command-line script

cli.run_command( "load-module led-system-panel-comp" )

cli.run_command( "new-led-system-panel-comp \"panel\" use_clock = TRUE" )

# If something goes wrong, the test fails.
# If nothing goes wrong, the test succeeds. 

# No point in trying to investigate the Simics state to see if 
# it succeeded completely.  
