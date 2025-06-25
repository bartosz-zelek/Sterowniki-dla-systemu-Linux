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

tb = sja1000_can_common.TestBench(1)

#-------------------config sender------------------------
#into reset mode
cli.run_command("mems0.write address = 0 size = 1 value = 0x01 -l")
#into Pelican mode
cli.run_command("mems0.write address = 31 size = 1 value = 0x80 -l")
#acceptance filter
cli.run_command("mems0.write address = 16 size = 1 value = 0xc0 -l") #acr0
cli.run_command("mems0.write address = 17 size = 1 value = 0x05 -l") #acr1
cli.run_command("mems0.write address = 18 size = 1 value = 0x55 -l") #acr2
cli.run_command("mems0.write address = 19 size = 1 value = 0x66 -l") #acr3
cli.run_command("mems0.write address = 20 size = 1 value = 0x33 -l") #amr0
cli.run_command("mems0.write address = 21 size = 1 value = 0x7f -l") #amr1
cli.run_command("mems0.write address = 22 size = 1 value = 0xff -l") #amr2
cli.run_command("mems0.write address = 23 size = 1 value = 0xff -l") #amr3
#set to single filter and go into operation mode
cli.run_command("mems0.write address = 0 size = 1 value = 0x08 -l")
#enable receive interrupt
cli.run_command("mems0.write address = 4 size = 1 value = 0x01 -l")
#compound a TX frame
cli.run_command("mems0.write address = 16 size = 1 value = 0x03 -l")  #frm info
cli.run_command("mems0.write address = 17 size = 1 value = 0xe1 -l")  #id1
cli.run_command("mems0.write address = 18 size = 1 value = 0x72 -l")  #id2
cli.run_command("mems0.write address = 19 size = 1 value = 0x55 -l")  #data1
cli.run_command("mems0.write address = 20 size = 1 value = 0x66 -l")  #data2
cli.run_command("mems0.write address = 21 size = 1 value = 0x77 -l")  #data2

#launch a self reception request
cli.run_command("mems0.write address = 1 size = 1 value = 0x10 -l")
SIM_continue(1)
rmc = cli.run_command("mems0.read address = 29 size = 1 -l") #msg counter
stest.expect_equal(rmc, 1)

#verify the frame received
fi = cli.run_command("mems0.read address = 16 size = 1 -l")
stest.expect_equal(fi, 0x03)
id1 = cli.run_command("mems0.read address = 17 size = 1 -l")
stest.expect_equal(id1, 0xe1)
id2 = cli.run_command("mems0.read address = 18 size = 1 -l")
stest.expect_equal(id2, 0x60)
data1 = cli.run_command("mems0.read address = 19 size = 1 -l")
stest.expect_equal(data1, 0x55)
data2 = cli.run_command("mems0.read address = 20 size = 1 -l")
stest.expect_equal(data2, 0x66)
data3 = cli.run_command("mems0.read address = 21 size = 1 -l")
stest.expect_equal(data3, 0x77)

#check the interrupt flags
stest.expect_equal(conf.irq0.irq_cnt, 1)
ir = cli.run_command("mems0.read address = 3 size = 1 -l")
stest.expect_equal(ir&0x1, 0x01)
