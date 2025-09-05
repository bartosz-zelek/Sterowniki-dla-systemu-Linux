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

import simics, pyobj

class sender(pyobj.ConfObject):
    """This device simply sends the character it gets written into its
       pseudo attribute <tt>char_to_send</tt> out of its <tt>to_receiver</tt>
       connect."""

    _class_desc = "simple training device to send chars"

    class pending_data(pyobj.SimpleAttribute(-1, 'i')):
        """Data that was rejected by receiver waiting for re-send."""

    class send_trigger(pyobj.Event):
        def callback(self, data):
            iface = self._up.to_receiver.iface
            if iface:
                bytes_written = iface.write(ord(data))
                if not bytes_written:
                    SIM_log_info(1, self._up.obj, 0, 'No bytes written.' +
                                 ' Waiting for receiver to become ready.')
                    self._up.pending_data.val = ord(data)

    class serial_device(pyobj.Interface):
        def write(self, val):
            # intentionally empty. We do not want to use it in the training
            pass
        def receive_ready(self):
            if (self._up.pending_data.val != -1):
                SIM_log_info(1, self._up.obj, 0, 'Receiver became ready.' +
                             ' Re-sending.')
                # receiver just got ready and we only send a single byte
                # so it MUST work
                self._up.to_receiver.iface.write(self._up.pending_data.val)
                self._up.pending_data.val = -1

    class to_receiver(pyobj.Attribute):
        """Connect for a serial_device interface."""
        attrattr = simics.Sim_Attr_Optional
        attrtype = 'o|n'
        def _initialize(self):
            self.val = None
            self.iface = None
        def getter(self):
            return self.val
        def setter(self, val):
            if val:
                # will throw and abort if not available
                tmp = SIM_get_interface(val, 'serial_device')
            else:
                tmp = None
            self.iface = tmp
            self.val = val

    class char_to_send(pyobj.Attribute):
        """Character that shall be sent."""
        attrattr = simics.Sim_Attr_Pseudo
        attrtype = 's'
        def setter(self, val):
            if (len(val) != 1):
                SIM_log_error(self._up.obj, 0, 'Only chars are accepted.')
                return simics.Sim_Set_Illegal_Value
            SIM_log_info(1, self._up.obj, 0, 'Will send in 100 cycles.')
            self._up.send_trigger.post(SIM_object_clock(self._up.obj),
                                       val,
                                       cycles = 100)
            return simics.Sim_Set_Ok
         
class man_in_the_middle(pyobj.ConfObject):
    """This device simply delays the character for 100 cycles it gets written
       into its port <tt>from_sender</tt> before writing it to its
       <tt>to_receiver</tt> connect. The device has a buffer size of 1."""

    _class_desc = "simple training device to delay chars"

    class blocked(pyobj.SimpleAttribute(False, 'b')):
        """Are we currently waiting to deliver a char?"""

    class send_trigger(pyobj.Event):
        def callback(self, data):
            iface = self._up.to_receiver.iface
            if iface:
                # no check for return value here as this is designed to
                # talk to a terminal with unlimited bandwidth
                iface.write(data)
            self._up.blocked.val = False
            self._up.to_sender.iface.receive_ready()
            

    class from_sender(pyobj.PortObject):
        class serial_device(pyobj.Interface):
            def write(self, val):
                myself = self._up._up
                if (myself.blocked.val):
                    SIM_log_info(1, myself.obj, 0, 'Currently busy. Cannot ' +
                                 'forward. Need to wait until other char is out.')
                    return 0
                myself.blocked.val = True
                myself.send_trigger.post(SIM_object_clock(myself.obj),
                                         val,
                                         cycles = 100)
                SIM_log_info(1, myself.obj, 0, 'Will forward in 100 cycles.')
                return 1
            def receive_ready(self):
                # intentionally empty. We do not want to use it in the training
                pass 

    class serial_device(pyobj.Interface):
        def write(self, val):
            # intentionally empty. We do not want to use it in the training
            pass
        def receive_ready(self):
            pass # TODO

    class to_sender(pyobj.Attribute):
        """Connect for a serial_device interface."""
        attrattr = simics.Sim_Attr_Optional
        attrtype = 'o|n'
        def _initialize(self):
            self.val = None
            self.iface = None
        def getter(self):
            return self.val
        def setter(self, val):
            if val:
                # will throw and abort if not available
                tmp = SIM_get_interface(val, 'serial_device')
            else:
                tmp = None
            self.iface = tmp
            self.val = val


    class to_receiver(pyobj.Attribute):
        """Connect for a serial_device interface."""
        attrattr = simics.Sim_Attr_Optional
        attrtype = 'o|n'
        def _initialize(self):
            self.val = None
            self.iface = None
        def getter(self):
            return self.val
        def setter(self, val):
            if val:
                # will throw and abort if not available
                tmp = SIM_get_interface(val, 'serial_device')
            else:
                tmp = None
            self.iface = tmp
            self.val = val
             
cli.global_cmds.log_setup(_time_stamp = True)
SIM_create_object('clock', 'clk', freq_mhz = 10)

#SIM_create_object('sender','uut', queue = conf.clk)
#SIM_run_command('load-module txt_console_comp')
#SIM_run_command('new-txt-console-comp console')
#SIM_create_object('man_in_the_middle','charlie', queue = conf.clk)

#conf.uut.to_receiver=conf.charlie.port.from_sender
#conf.charlie.to_sender=conf.uut

#conf.charlie.to_receiver = conf.console.con

