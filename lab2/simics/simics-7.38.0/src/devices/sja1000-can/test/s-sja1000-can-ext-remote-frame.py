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


import dev_util
import conf
import stest
import sja1000_can_common
import cli

tb = sja1000_can_common.TestBench(2)

#-------------------config sender------------------------
#into reset mode
cli.run_command("mems0.write address = 0 size = 1 value = 0x01 -l")
#into Pelican mode
cli.run_command("mems0.write address = 31 size = 1 value = 0x80 -l")
#into operation mode
cli.run_command("mems0.write address = 0 size = 1 value = 0x00 -l")
#compound a TX frame
cli.run_command("mems0.write address = 16 size = 1 value = 0xc0 -l")  #frm info
cli.run_command("mems0.write address = 17 size = 1 value = 0x11 -l")  #id1
cli.run_command("mems0.write address = 18 size = 1 value = 0x22 -l")  #id2
cli.run_command("mems0.write address = 19 size = 1 value = 0x33 -l")  #id3
cli.run_command("mems0.write address = 20 size = 1 value = 0x44 -l")  #id4

#-------------------config receiver------------------------
#into reset mode
cli.run_command("mems1.write address = 0 size = 1 value = 0x01 -l")
#into Pelican mode
cli.run_command("mems1.write address = 31 size = 1 value = 0x80 -l")
#into reset mode for Peli
cli.run_command("mems1.write address = 0 size = 1 value = 0x01 -l")
#acceptance mask: accept all the recv frames
cli.run_command("mems1.write address = 20 size = 1 value = 0xff -l")
cli.run_command("mems1.write address = 21 size = 1 value = 0xff -l")
cli.run_command("mems1.write address = 22 size = 1 value = 0xff -l")
#into operation mode
cli.run_command("mems1.write address = 0 size = 1 value = 0x00 -l")

#launch transmission
cli.run_command("mems0.write address = 1 size = 1 value = 0x01 -l")
SIM_continue(1)

#verify the frame received
fi = cli.run_command("mems1.read address = 16 size = 1 -l")
stest.expect_equal(fi, 0xc0)
id1 = cli.run_command("mems1.read address = 17 size = 1 -l")
stest.expect_equal(id1, 0x11)
id2 = cli.run_command("mems1.read address = 18 size = 1 -l")
stest.expect_equal(id2, 0x22)
id3 = cli.run_command("mems1.read address = 19 size = 1 -l")
stest.expect_equal(id3, 0x33)
id4 = cli.run_command("mems1.read address = 20 size = 1 -l")
stest.expect_equal(id4, 0x40)
