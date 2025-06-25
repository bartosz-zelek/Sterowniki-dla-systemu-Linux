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


from comp import *
import pyobj
import stest
import dev_util

class Ieee_802_3_mac_v3(dev_util.Iface):
    iface = "ieee_802_3_mac_v3"
    def __init__(self):
        self.status = []
class fake_mac(pyobj.ConfObject):
    class ieee_802_3_mac_v3(Interface):
        def _initialize(self):
            self.status = []

        def link_status_changed(self, phy, status):
            self.status.append((phy, status))


class fake_mac_comp(StandardComponent):
    class component_connector(Interface):
        def _finalize(self):
            self._up.add_connector(
                'phy', 'phy', True, True, False,
                simics.Sim_Connector_Direction_Down)

        def get_check_data(self, cnt):
            return self.get_connect_data(cnt)
        def get_connect_data(self, cnt):
            return [self._up.get_slot("mac")]
        def check(self, cnt, attr):
            return True
        def connect(self, cnt, attr):
            pass
        def disconnect(self, cnt):
            pass

    def setup(self):
        self.add_pre_obj('mac', 'fake_mac')

def test_link_status_change():
    '''Test that the MAC's link status is updated after
    connecting/disconnecting the phy to a link'''
    mac_comp = SIM_create_object('fake_mac_comp', 'mac')
    SIM_create_object('clock', 'clock', freq_mhz=1000)
    SIM_load_module('phy-comp')
    SIM_run_command('$phy = (create-phy-comp phy)')
    SIM_load_module('eth-links')
    SIM_run_command('$link = (create-ethernet-switch link)')
    SIM_run_command('connect $phy.mac mac.phy')
    SIM_run_command('instantiate-components')
    phy = getattr(conf, simenv.phy)
    mac = mac_comp.mac.object_data
    stest.expect_equal(mac.ieee_802_3_mac_v3.status,
                       [(0, IEEE_link_unconnected)])
    mac.ieee_802_3_mac_v3.status = []
    SIM_run_command('connect $phy.eth $link.device0')
    stest.expect_equal(mac.ieee_802_3_mac_v3.status,
                       [(phy.mii_address, IEEE_link_up)])
    mac.ieee_802_3_mac_v3.status = []
    SIM_run_command('disconnect $phy.eth $link.device0')
    stest.expect_equal(mac.ieee_802_3_mac_v3.status,
                       [(phy.mii_address, IEEE_link_unconnected)])

test_link_status_change()
