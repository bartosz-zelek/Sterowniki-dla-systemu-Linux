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

import dev_util as du
import stest
from comp import *

class fake_signal_target:
    cls = simics.confclass('fake-signal-target')
    cls.attr.state('b', default=False)

    @cls.iface.signal.signal_raise
    def signal_raise(self):
        stest.expect_false(self.state, "signal raised when already high")
        self.state = True

    @cls.iface.signal.signal_lower
    def signal_lower(self):
        stest.expect_true(self.state, "signal lowered when already low")
        self.state = False

class sample_gasket_comp(StandardConnectorComponent):
    """Sample Gasket component class."""
    _class_desc = "sample Gasket component"
    _help_categories = ()

    def setup(self):
        super().setup()
        if not self.instantiated.val:
            self.add_objects()

    def add_objects(self):
        simulation = self.add_pre_obj('simulation', 'sample_tlm2_gasket_device')
        gasket32 = self.add_pre_obj('gasket32',
                                    'sample_tlm2_gasket_device_gasket_simics2tlm_IoMemory')
        gasket32.target = 'top.socket32'
        gasket32.simulation = simulation
        gasket64 = self.add_pre_obj('gasket64',
                                    'sample_tlm2_gasket_device_gasket_simics2tlm_IoMemory')
        gasket64.target = 'top.socket64'
        gasket64.simulation = simulation
        multi_gasket = self.add_pre_obj('multi_gasket',
                                        'sample_tlm2_gasket_device_gasket_simics2tlm_IoMemory')
        multi_gasket.target = 'top.multi_socket'
        multi_gasket.simulation = simulation
        multi_gasket64 = self.add_pre_obj('multi_gasket64',
                                         'sample_tlm2_gasket_device_gasket_simics2tlm_IoMemory')
        multi_gasket64.target = 'top.multi_socket64'
        multi_gasket64.simulation = simulation

        signal_in = self.add_pre_obj('signal_in',
                                     'sample_tlm2_gasket_device_gasket_simics2systemc_Signal')
        signal_in.signal = 'top.sc_in'
        signal_in.simulation = simulation

        signal_object = self.add_pre_obj('signal_object', 'fake-signal-target')
        signal_out = self.add_pre_obj('signal_out',
                                      'sample_tlm2_gasket_device_gasket_systemc2simics_Signal')
        signal_out.signal = 'top.sc_out'
        signal_out.object = signal_object
        signal_out.simulation = simulation

        signal_in_2 = self.add_pre_obj('signal_in_2',
                                       'sample_tlm2_gasket_device_gasket_simics2systemc_Signal')
        signal_in_2.signal = 'top.sc_in_2'
        signal_in_2.initial_level = True
        signal_in_2.simulation = simulation

        signal_object_2 = self.add_pre_obj('signal_object_2',
                                           'fake-signal-target')
        signal_out_2 = self.add_pre_obj('signal_out_2',
                                        'sample_tlm2_gasket_device_gasket_systemc2simics_Signal')
        signal_out_2.signal = 'top.sc_out_2'
        signal_out_2.object = signal_object_2
        signal_out_2.simulation = simulation

        VECTOR_SIZE = 100
        vector_in = self.add_pre_obj('vector_in[%d]' % VECTOR_SIZE,
                                     'sample_tlm2_gasket_device_gasket_simics2systemc_Signal')
        for idx in range(VECTOR_SIZE):
            vector_in[idx].signal = 'top.sc_in_vec_%d' % idx
            vector_in[idx].simulation = simulation

        signal_object_vect = [self.add_pre_obj('signal_object_vec_%d' % idx,
                                               'fake-signal-target')
                              for idx in range(VECTOR_SIZE)]
        vector_out = self.add_pre_obj('vector_out[%d]' % VECTOR_SIZE,
                                      'sample_tlm2_gasket_device_gasket_systemc2simics_Signal')
        for idx in range(VECTOR_SIZE):
            vector_out[idx].signal = 'top.sc_out_vec_%d' % idx
            vector_out[idx].object = signal_object_vect[idx]
            vector_out[idx].simulation = simulation

        dma_image = self.add_pre_obj('dma_image', 'image')
        dma_image.size = 32
        dma_ram = self.add_pre_obj('dma_ram', 'ram')
        dma_ram.image = dma_image
        dma_space = self.add_pre_obj('dma_space', 'memory-space')
        dma_space.map = [[0x0, dma_ram, 0, 0, dma_image.size]]

        gasketMemorySpace = self.add_pre_obj('gasketMemorySpace',
                                             'sample_tlm2_gasket_device_gasket_tlm2simics_MemorySpace')
        gasketMemorySpace.initiator = 'top.initiator_socket'
        gasketMemorySpace.simulation = simulation
        gasketMemorySpace.object = dma_space

        multi_gasketMemorySpace = self.add_pre_obj('multi_gasketMemorySpace',
                                                   'sample_tlm2_gasket_device_gasket_tlm2simics_MemorySpace')
        multi_gasketMemorySpace.initiator = 'top.multi_initiator_socket'
        multi_gasketMemorySpace.simulation = simulation
        multi_gasketMemorySpace.object = dma_space

        simulation.gasket_list = [signal_in, signal_out,
                                  signal_in_2, signal_out_2,
                                  gasket32, gasket64,
                                  multi_gasket, multi_gasket64,
                                  gasketMemorySpace, multi_gasketMemorySpace] \
                                  + vector_in + vector_out

def create_sample_gasket_comp():
    simics.SIM_run_command("create-sample-gasket-comp")
    simics.SIM_run_command("instantiate-components")
