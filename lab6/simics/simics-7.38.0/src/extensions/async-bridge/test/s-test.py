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


# Tests for the async-bridge class. The async-bridge class is described
# in detail in README.md file located in the async-bridge's module.

import cli
import conf
import pyobj
import shutil
import stest
import simics
from simics import (
    SIM_break_simulation,
    SIM_continue,
    SIM_create_object,
    SIM_cycle_count,
    SIM_delete_objects,
    SIM_event_post_cycle,
    SIM_object_iterator,
    SIM_get_object,
    SIM_log_info,
    SIM_register_event,
    SIM_set_attribute_default,
    Sim_EC_Notsaved,
    Sim_PE_No_Exception,
    transaction_t,
)
import time
import unittest


class interface_device(pyobj.ConfObject):
    '''Test class for testing async-bridge. The class implements an interface
       device as it is described in async-bridge's README file. The class has
       clock and async-bridge as subobjects. The class implements an external
       interface via 'external' port object. Accesses to the interface run
       the clock subobject (with the help of async-bridge).'''

    class device_cpu(pyobj.PortObject):
        '''clock subobject. "Emulates" a CPU that the device drives
           in order to handle requests'''
        namespace = ''
        classname = 'clock'

    class async_bridge(pyobj.PortObject):
        namespace = 'device_cpu'
        classname = 'async-bridge'

    class cell(pyobj.PortObject):
        namespace = ''
        classname = 'cell'

    def _initialize(self):
        super()._initialize()

        SIM_set_attribute_default(self.obj.device_cpu, "freq_mhz", 1)
        SIM_set_attribute_default(self.obj.device_cpu, "cell", self.obj.cell)
        SIM_set_attribute_default(
            self.obj.device_cpu.async_bridge, "cell", self.obj.cell)

        self.enter_counter = 0
        self.leave_counter = 0

    class synchronous_mode_leave_event(pyobj.Event):
        def callback(self, data):
            self._up.synchronous_mode_leave()

    class external(pyobj.PortObject):
        """External interface. Any accesses to it run device_cpu
           for device_cpu_busy_cycles with the help of async-bridge."""
        class transaction(pyobj.Interface):
            def issue(self, t, addr):
                self._up._up.advance_device_cpu(
                    cycles = self._up._up.device_cpu_busy_cycles.val)
                return Sim_PE_No_Exception

    class device_cpu_busy_cycles(pyobj.Attribute):
        """Number of cycles that we run device_cpu for
           when interface device is accessed."""
        attrattr = simics.Sim_Attr_Pseudo
        attrtype = 'i'
        def _initialize(self):
            self.val = 20
        def getter(self):
            return self.val

    def synchronous_mode_enter(self):
        SIM_log_info(2, self.obj, 0, 'synchronous_mode.enter')
        self.enter_counter += 1
        return self.obj.device_cpu.async_bridge.iface.synchronous_mode.enter()

    def synchronous_mode_leave(self):
        SIM_log_info(2, self.obj, 0, 'synchronous_mode.leave')
        self.leave_counter += 1
        self.obj.device_cpu.async_bridge.iface.synchronous_mode.leave()

    def post_synchronous_mode_leave_event(self, cycles):
        self.synchronous_mode_leave_event.post(
            self.obj.device_cpu, None, cycles = cycles)

    def advance_device_cpu(self, cycles):
        self.post_synchronous_mode_leave_event(cycles)
        sync_count = self.synchronous_mode_enter()
        assert sync_count == 0, ("Check (indirectly) that enter exited only"
                                 " after we advanced time")


class Tests(unittest.TestCase):

    def setUp(self):
        self.tobj = SIM_create_object('interface_device', 'tobj')
        self.main_cpu = SIM_create_object('clock', 'main_cpu',
                                          freq_mhz = 1, cell = self.tobj.cell)

        self.remove_from_scheduling(self.tobj.device_cpu.async_bridge)
        self.remove_from_scheduling(self.main_cpu)

        def ev_clbk(clk, self):
            SIM_log_info(2, clk, 0,
                         "Accessing external interface from event callback")
            self.tobj.port.external.iface.transaction.issue(transaction_t(), 0)

        self.ext_iface_access_ev = SIM_register_event(
            "External Interface Access",
            None, Sim_EC_Notsaved, ev_clbk,
            None, None, None, None)

    def tearDown(self):
        SIM_delete_objects([self.tobj, self.main_cpu])

    # add_to_scheduling, remove_from_scheduling are used to add/remove
    # an object from Simics scheduler. We use the functions to control
    # what object Simics runs we use SIM_continue:
    def add_to_scheduling(self, clock):
        clock.attr.do_not_schedule = False
    def remove_from_scheduling(self, clock):
        clock.attr.do_not_schedule = True


    def test_running_simulation_forward(self):

        self.add_to_scheduling(self.tobj.device_cpu.async_bridge)

        cycles1 = 10
        SIM_continue(cycles1)
        self.assertEqual(SIM_cycle_count(self.tobj.device_cpu), cycles1)
        self.assertEqual(
            SIM_cycle_count(self.tobj.device_cpu.async_bridge), cycles1)

        cycles2 = 10_000_000
        SIM_continue(cycles2)
        self.assertEqual(SIM_cycle_count(self.tobj.device_cpu), cycles1+cycles2)
        self.assertEqual(
            SIM_cycle_count(self.tobj.device_cpu.async_bridge), cycles1+cycles2)

        self.tobj.port.external.iface.transaction.issue(transaction_t(), 0)
        self.assertEqual(self.tobj.object_data.enter_counter, 1)
        self.assertEqual(self.tobj.object_data.leave_counter, 1)
        self.assertEqual(SIM_cycle_count(self.tobj.device_cpu),
                         cycles1 + cycles2 + self.tobj.device_cpu_busy_cycles)
        self.assertEqual(
            SIM_cycle_count(self.tobj.device_cpu.async_bridge), cycles1+cycles2)

        cycle_until = cycles1 + cycles2 + 10_000
        conf.bp.cycle.cli_cmds.run_until(object = self.tobj.device_cpu,
                                         cycle = cycle_until, _absolute = True)
        self.assertEqual(SIM_cycle_count(self.tobj.device_cpu), cycle_until)
        self.assertEqual(
            SIM_cycle_count(self.tobj.device_cpu.async_bridge), cycle_until)

    def test_access_from_global_context(self):
        '''Test accessing external interface from Global Context (the test code
           runs in Global Context so we don't need to do anything in order
           to enter it).'''

        self.tobj.port.external.iface.transaction.issue(transaction_t(), 0)

        self.assertEqual(self.tobj.object_data.enter_counter, 1)
        self.assertEqual(self.tobj.object_data.leave_counter, 1)
        self.assertEqual(SIM_cycle_count(self.tobj.device_cpu),
                         self.tobj.device_cpu_busy_cycles)
        self.assertEqual(SIM_cycle_count(self.tobj.device_cpu.async_bridge), 0)

    def test_access_from_global_context_with_breakpoints(self):
        '''Test behavior when accessing external interface from Global Context
           triggers breakpoints. Current behavior is that in such case
           breakpoints don't stop simulation immediately: device CPU executes
           until it leaves synchronous mode.'''

        self.add_to_scheduling(self.tobj.device_cpu.async_bridge)
        SIM_continue(1)

        bp_id = conf.bp.cycle.cli_cmds.break_cmd(
            object = self.tobj.device_cpu, cycle = 2)
        assert bp_id in conf.bp.cli_cmds.list()

        self.tobj.port.external.iface.transaction.issue(transaction_t(), 0)

        # Check that the breakpoint has been triggered by verifying it was
        # deleted. Please note that we simulation didn't stop
        # at the breakpoint - the cycle count of tobj.device_cpu is larger.
        self.assertNotIn(bp_id, conf.bp.cli_cmds.list())

        self.assertEqual(self.tobj.object_data.enter_counter, 1)
        self.assertEqual(self.tobj.object_data.leave_counter, 1)
        self.assertEqual(SIM_cycle_count(self.tobj.device_cpu),
                         self.tobj.device_cpu_busy_cycles + 1)
        self.assertEqual(SIM_cycle_count(self.tobj.device_cpu.async_bridge), 1)

    def test_access_from_execution_context(self):
        '''Test accessing external interface when simulation is running
           (in other words, calling external interface in Execution context):
           Simics scheduler -> main_cpu -> async_bridge.'''

        self.add_to_scheduling(self.main_cpu)

        SIM_event_post_cycle(
            self.main_cpu, self.ext_iface_access_ev, self.main_cpu, 2, self)
        SIM_continue(3)

        self.assertEqual(self.tobj.object_data.enter_counter, 1)
        self.assertEqual(self.tobj.object_data.leave_counter, 1)
        self.assertEqual(SIM_cycle_count(self.main_cpu), 3)
        self.assertEqual(SIM_cycle_count(self.tobj.device_cpu),
                         self.tobj.device_cpu_busy_cycles)
        self.assertEqual(SIM_cycle_count(self.tobj.device_cpu.async_bridge), 0)

    def test_execution_with_breakpoints(self):
        '''Test the following scenario: Simics scheduler -> main_cpu ->
           async-bridge's external interface -> executor -> breakpoint.'''

        self.add_to_scheduling(self.main_cpu)

        bp_id = conf.bp.cycle.cli_cmds.break_cmd(
            object = self.tobj.device_cpu, cycle = 5)
        assert bp_id in conf.bp.cli_cmds.list()

        SIM_event_post_cycle(
            self.main_cpu, self.ext_iface_access_ev, self.main_cpu, 2, self)

        SIM_continue(3)

        # Check that the breakpoint stopped simulation
        # exactly at the point when it happened.
        self.assertEqual(self.tobj.object_data.enter_counter, 1)
        self.assertEqual(self.tobj.object_data.leave_counter, 0)
        self.assertEqual(SIM_cycle_count(self.main_cpu), 2)
        self.assertEqual(SIM_cycle_count(self.tobj.device_cpu), 5)
        self.assertEqual(SIM_cycle_count(self.tobj.device_cpu.async_bridge), 0)

        # We must execute more, till "synchronous" mode is left: otherwise,
        # the deletion of test objects - it is done in self.tearDown - fails.
        conf.bp.cycle.cli_cmds.break_cmd(
            object = self.main_cpu, cycle = 0)
        SIM_continue(0)

    def test_sync_mode_enter_during_async_bridge_run(self):
        '''Test for the following scenario: Simics scheduler -> async-bridge ->
           executor -> call to async-bridge's external interface (while we still
           run in executor).'''

        self.add_to_scheduling(self.tobj.device_cpu.async_bridge)

        sim_continue_cycles = 5
        device_cpu_busy_cycles = 7000
        test_event_cycle = 2

        def test_event_clbk(clk, _):
            self.tobj.object_data.post_synchronous_mode_leave_event(
                cycles = device_cpu_busy_cycles)
            # NB: synchronous_mode.enter returns immediately when we are running
            sync_count = self.tobj.object_data.synchronous_mode_enter()
            assert sync_count == 1, ("Counter is 1,"
                                     " but we return immediately")

        test_ev = SIM_register_event(
            "Test Event",
            None, Sim_EC_Notsaved, test_event_clbk,
            None, None, None, None)

        SIM_event_post_cycle(
            self.tobj.device_cpu, test_ev, self.tobj.device_cpu,
            test_event_cycle, self)

        SIM_continue(sim_continue_cycles)
        self.assertEqual(self.tobj.object_data.enter_counter, 1)
        self.assertEqual(self.tobj.object_data.leave_counter, 1)
        self.assertEqual(SIM_cycle_count(self.tobj.device_cpu),
                         test_event_cycle + device_cpu_busy_cycles)
        self.assertEqual(SIM_cycle_count(self.tobj.device_cpu.async_bridge),
                         sim_continue_cycles)

    def test_sync_mode_enter_during_async_bridge_run_with_breakpoint(self):
        '''Test for the following scenario: Simics scheduler -> async-bridge ->
           executor -> call to async-bridge's external interface (while we still
           run in executor) -> breakpoint (while we still run in executor and
           have just entered sync mode).'''

        self.add_to_scheduling(self.tobj.device_cpu.async_bridge)

        sim_continue_cycles = 5
        device_cpu_busy_cycles = 7000
        test_event_cycle = 2

        def test_event_clbk(clk, _):
            self.tobj.object_data.post_synchronous_mode_leave_event(
                cycles = device_cpu_busy_cycles)
            # NB: synchronous_mode.enter returns immediately when we are running
            sync_count = self.tobj.object_data.synchronous_mode_enter()
            assert sync_count == 1, ("Counter is 1,"
                                     " but we return immediately")

            SIM_break_simulation("Test breakpoint")

        test_ev = SIM_register_event(
            "Test Event",
            None, Sim_EC_Notsaved, test_event_clbk,
            None, None, None, None)

        SIM_event_post_cycle(
            self.tobj.device_cpu, test_ev, self.tobj.device_cpu,
            test_event_cycle, self)

        SIM_continue(sim_continue_cycles)

        # Check that SIM_break_simulation request stopped simulation
        # exactly when it has happened.
        self.assertEqual(self.tobj.object_data.enter_counter, 1)
        self.assertEqual(self.tobj.object_data.leave_counter, 0)
        self.assertEqual(SIM_cycle_count(self.tobj.device_cpu.async_bridge),
                         test_event_cycle)
        self.assertEqual(SIM_cycle_count(self.tobj.device_cpu),
                         test_event_cycle)

        # We must execute more, till "synchronous" mode is left: otherwise,
        # the deletion of test objects - it is done in self.tearDown - fails.
        conf.bp.cycle.cli_cmds.run_until(
            object = self.tobj.device_cpu.async_bridge,
            cycle = test_event_cycle, _absolute = True)
        self.assertEqual(self.tobj.object_data.enter_counter, 1)
        self.assertEqual(self.tobj.object_data.leave_counter, 1)
        self.assertEqual(SIM_cycle_count(self.tobj.device_cpu),
                         test_event_cycle + device_cpu_busy_cycles)
        self.assertEqual(SIM_cycle_count(self.tobj.device_cpu.async_bridge),
                         test_event_cycle)


class TestsWithScriptBranches(unittest.TestCase):
    '''Different tests where we access the interface that uses
       synchronous_mode interface from script branches.'''

    def setUp(self):
        self.tobj = SIM_create_object('interface_device', 'tobj')

    def tearDown(self):
        SIM_delete_objects([self.tobj])


    def access_from_script_branch_test(self, wait_object):

        wait_cycle = 5
        def script_branch():
            conf.bp.cycle.cli_cmds.wait_for(object = wait_object,
                                            cycle = wait_cycle,
                                            _absolute = True)
            self.assertEqual(SIM_cycle_count(wait_object), wait_cycle)

            # We are in global context here. Access the interface that drives
            # device_cpu forward with the synchronous_mode interface.
            self.tobj.port.external.iface.transaction.issue(transaction_t(), 0)
            cli.global_cmds.stop()

        cli.sb_create(script_branch)

        SIM_continue(0)

        self.assertEqual(self.tobj.object_data.enter_counter, 1)
        self.assertEqual(self.tobj.object_data.leave_counter, 1)
        self.assertEqual(SIM_cycle_count(self.tobj.device_cpu),
                         wait_cycle + self.tobj.device_cpu_busy_cycles)
        self.assertEqual(SIM_cycle_count(self.tobj.device_cpu.async_bridge), 5)

    def test_access_from_script_branch_1(self):
        self.access_from_script_branch_test(self.tobj.device_cpu)

    def test_access_from_script_branch_2(self):
        self.access_from_script_branch_test(self.tobj.device_cpu.async_bridge)

    def test_script_branch_and_nested_synchronous_mode_entry(self):
        '''Test when synchronous_mode.enter is accessed from script branch'''

        wait_cycle = 5
        device_cpu_busy_cycles = 100
        def script_branch():
            conf.bp.cycle.cli_cmds.wait_for(object = self.tobj.device_cpu,
                                            cycle = wait_cycle,
                                            _absolute = True)
            self.assertEqual(SIM_cycle_count(self.tobj.device_cpu), wait_cycle)

            self.tobj.object_data.post_synchronous_mode_leave_event(
                device_cpu_busy_cycles)
            sync_count = self.tobj.object_data.synchronous_mode_enter()
            self.assertEqual(
                sync_count, 2,
                "Nested call to synchronous_mode.enter returns immediately")

            cli.global_cmds.stop()  # This stop doesn't do anything

        cli.sb_create(script_branch)

        self.tobj.port.external.iface.transaction.issue(transaction_t(), 0)

        self.assertEqual(self.tobj.object_data.enter_counter, 2)
        self.assertEqual(self.tobj.object_data.leave_counter, 2)
        self.assertEqual(SIM_cycle_count(self.tobj.device_cpu),
                         wait_cycle + device_cpu_busy_cycles)
        self.assertEqual(SIM_cycle_count(self.tobj.device_cpu.async_bridge), 0)


class CreationDeletionTest(unittest.TestCase):
    def test_deletion_of_async_bridge(self):
        """A bit unclear how to test this best. Currently we (indirectly) check
           that the execution environment associated with async-bridge
           object was also deleted."""

        import psutil
        p = psutil.Process()
        num_threads = p.num_threads()
        threads_old_ids = [t.id for t in p.threads()]

        tobj = SIM_create_object('interface_device', 'tobj')

        self.assertEqual(p.num_threads(), num_threads + 1)
        threads_new_ids = [t.id for t in p.threads()
                           if t.id not in threads_old_ids]
        self.assertEqual(len(threads_new_ids), 1, threads_new_ids)
        execute_env_thread_id = threads_new_ids[0]

        SIM_delete_objects([tobj])

        # Check that the new thread has gone:
        if execute_env_thread_id not in [t.id for t in p.threads()]:
            return  # success: execute_env_thread_id was terminated
        # We have seen that sometimes p.threads() may still report
        # execute_env_thread_id even though the thread is terminated
        # when the async-bridge object is deleted. We wait here - hopefully,
        # p.threads() notices that the thread was terminated - and check again.
        time.sleep(1)
        self.assertTrue(execute_env_thread_id not in [t.id for t in p.threads()])


class SaveRestoreTest(unittest.TestCase):
    '''Simple tests for saving and restoring async-bridge.'''

    def setUp(self):
        self.existing_objects = set(SIM_object_iterator(None))
        self.tobj = SIM_create_object('interface_device', 'tobj')

    def tearDown(self):
        objects_to_delete = list(
            set(SIM_object_iterator(None)) - self.existing_objects)
        SIM_delete_objects(objects_to_delete)


    def save_restore_run_test(self):
        '''The function saves objects, restores them and checks that
        the restored objects can run.'''

        checkpoint = stest.scratch_file("test.ckpt")
        shutil.rmtree(checkpoint, ignore_errors = True)
        cli.global_cmds.write_configuration(file = checkpoint)

        prefix = "restored_"
        cli.global_cmds.read_configuration(file = checkpoint, prefix = prefix)

        restored_tobj = SIM_get_object(prefix + self.tobj.name)

        self.assertEqual(SIM_cycle_count(self.tobj.device_cpu),
                         SIM_cycle_count(restored_tobj.device_cpu))
        self.assertEqual(SIM_cycle_count(self.tobj.device_cpu.async_bridge),
                         SIM_cycle_count(restored_tobj.device_cpu.async_bridge))

        restored_cpu_cycles = SIM_cycle_count(
            restored_tobj.device_cpu)
        restored_async_bridge_cycles = SIM_cycle_count(
            restored_tobj.device_cpu.async_bridge)

        restored_tobj.port.external.iface.transaction.issue(transaction_t(), 0)
        self.assertEqual(restored_tobj.object_data.enter_counter, 1)
        self.assertEqual(restored_tobj.object_data.leave_counter, 1)
        self.assertEqual(SIM_cycle_count(restored_tobj.device_cpu),
                         restored_cpu_cycles
                         + restored_tobj.device_cpu_busy_cycles)
        self.assertEqual(SIM_cycle_count(restored_tobj.device_cpu.async_bridge),
                         restored_async_bridge_cycles)

        conf.bp.cycle.cli_cmds.run_until(
            object = restored_tobj.device_cpu,
            cycle = 10_000, _absolute = True)
        self.assertEqual(SIM_cycle_count(restored_tobj.device_cpu), 10_000)
        self.assertEqual(
            SIM_cycle_count(restored_tobj.device_cpu.async_bridge), 10_000)


    def test1(self):

        # advance simulation a bit and test if save/restore work
        SIM_continue(10)

        self.save_restore_run_test()

    def test2(self):

        # advance only device_cpu and test if save/restore work
        self.tobj.port.external.iface.transaction.issue(transaction_t(), 0)
        self.assertNotEqual(SIM_cycle_count(self.tobj.device_cpu),
                            SIM_cycle_count(self.tobj.device_cpu.async_bridge))

        self.save_restore_run_test()



unittest.main(argv = ['unittest'])
