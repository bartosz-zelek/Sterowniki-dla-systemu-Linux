# © 2024 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

import pyobj
import stest

class ForwardTarget(pyobj.ConfObject):
    """Test device implementing the transaction interface.
    The accesses are handled differently depending on the address
    of the access."""

    def _initialize(self):
        super()._initialize()
        self.transactions = []

    class transaction(pyobj.Interface):
        def issue(self, t, addr):
            self._up.transactions.append((t.pcie_type, addr))
            return simics.Sim_PE_No_Exception

forward_target = simics.pre_conf_object('forward_target', 'ForwardTarget')
dut = simics.pre_conf_object('dut', 'pcie_map_helper_cpp')
dut.pcie_type = simics.PCIE_Type_IO
dut.forward_target = forward_target
simics.SIM_add_configuration([forward_target, dut], None)

t = transaction_t(size = 4, read = True)

# Invalid pcie_type
with stest.expect_exception_mgr(SimExc_IllegalValue):
    conf.dut.pcie_type = simics.PCIE_Type_Not_Set

# Valid pcie_type
for pcie_type in [simics.PCIE_Type_Cfg, simics.PCIE_Type_Mem,
                  simics.PCIE_Type_IO, simics.PCIE_Type_Msg]:
    conf.dut.pcie_type = pcie_type
    for addr in [0x100, 0xffff0000]:
        conf.dut.iface.transaction.issue(t, addr)
        forwarded_t = conf.forward_target.object_data.transactions[-1]
        stest.expect_equal(forwarded_t, (pcie_type, addr))
