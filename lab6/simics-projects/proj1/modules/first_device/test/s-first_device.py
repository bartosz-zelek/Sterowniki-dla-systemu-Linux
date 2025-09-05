import dev_util
import conf
import stest

# Create an instance of the device to test
dev = pre_conf_object('dev', 'first_device')
SIM_add_configuration([dev], None)
dev = conf.dev

# Register wrappers for the regs bank
r_arg1 = dev_util.Register_LE(dev.bank.regs, 0x00)
r_arg2 = dev_util.Register_LE(dev.bank.regs, 0x04)
r_op = dev_util.Register_LE(dev.bank.regs, 0x08)
r_res = dev_util.Register_LE(dev.bank.regs, 0x0C)

# Initial state: result should be 0 (no writes yet)
stest.expect_equal(r_res.read(), 0)

# Test addition: 5 + 3 = 8
r_op.write(43)  # '+'
r_arg1.write(5)
r_arg2.write(3)
stest.expect_equal(r_res.read(), 8)

# Change operation to subtraction: 5 - 3 = 2
r_op.write(45)  # '-'
stest.expect_equal(r_res.read(), 2)

# Update arg1 under subtraction: 10 - 3 = 7
r_arg1.write(10)
stest.expect_equal(r_res.read(), 7)

# Change operation to multiplication: 10 * 3 = 30
r_op.write(42)  # '*'
stest.expect_equal(r_res.read(), 30)

# Update arg2 under multiplication: 10 * 4 = 40
r_arg2.write(4)
stest.expect_equal(r_res.read(), 40)