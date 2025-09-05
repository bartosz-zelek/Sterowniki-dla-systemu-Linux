# © 2016 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


import cli
import conf
import simics
import stest
import os

# SIMICS-20853
conf.sim.deprecation_level = 0

class ConsoleWrapper:
    def __init__(self, console, prompt):
        self.__console = console
        self.__prompt = prompt

    def wait_for_string(self, string):
        self.__console.iface.break_strings_v2.add_single(string, None, None)
        simics.SIM_continue(0)

    def type_string(self, string):
        self.__console.iface.con_input.input_str(string)

    def command(self, cmd):
        self.type_string(cmd + '\n')
        self.wait_for_string(self.__prompt)

    def wait_for_prompt(self):
        self.wait_for_string(self.__prompt)


def setup_test_system():
    cli.run_command("load-module sample-linux-tracker")
    qsp_script = simics.SIM_lookup_file(
        '%simics%/targets/qsp-x86/qsp-linux-common.simics')
    stest.expect_different(qsp_script, None,
                           "Unable to locate qsp start script")
    simics.SIM_run_command_file(qsp_script, False)
    cli.run_command("board.software.delete-tracker")

    cli.run_command("%s.insert-tracker tracker = sample_linux_tracker_comp"
                    % get_sw().name)

    params_file = stest.scratch_file("sample-linux.params")
    if os.path.exists(params_file):
        cli.run_command("%s.load-parameters %s" % (get_sw().name, params_file))
    else:
        cli.run_command("%s.tracker.detect-parameters -load %s"
                        % (get_sw().name, params_file))

    console = ConsoleWrapper(get_console(), root_prompt())
    return (conf.board.software, console)

def root_prompt():
    return "~# "

def get_console():
    return conf.board.serconsole.con

def boot_system(console):
    stest.untrap_log("spec-viol")
    console.wait_for_prompt()
    return console

def get_cpu():
    return conf.board.mb.cpu0.core[0][0]

def get_sw():
    return conf.board.software

def child(sw, parent_id, child_name):
    nt_query = sw.iface.osa_node_tree_query
    children = nt_query.get_children(parent_id)
    for child_id in children:
        if nt_query.get_node(child_id)['name'] == child_name:
            return child_id
    return None

def root_node(sw):
    root = sw.iface.osa_component.get_root_node()
    stest.expect_true(root.valid)
    return root.id

def kernel(sw):
    return child(sw, root_node(sw), "Kernel")

def userspace(sw):
    return child(sw, root_node(sw), "Userspace")

def threads_node(sw):
    kernel_id = kernel(sw)
    return child(sw, kernel_id, "Threads")

def other_node(sw):
    kernel_id = kernel(sw)
    return child(sw, kernel_id, "Other")

def is_active(sw, cpu, node_id):
    nt_query = sw.iface.osa_node_tree_query
    return node_id in nt_query.get_current_nodes(root_node(sw), cpu)

class Transaction:
    """Handles transactions (begin/end calls) for both trackers and mappers"""
    def __init__(self, ifc, obj, cpu):
        self.cpu = cpu
        self.obj = obj
        self.ifc = ifc

    def __enter__(self):
        self.tx_id = self.ifc.begin(self.obj, self.cpu)

    def __exit__(self, exc_type, exc_value, traceback):
        self.ifc.end(self.tx_id)


class ShadowTracker:
    def __init__(self, sw, tracker):
        self.__sw = sw
        self.__tracker = tracker
        self.__nt_state_admin = sw.iface.osa_tracker_state_admin
        self.__active = None

    def tx(self, cpu):
        return Transaction(self.__nt_state_admin, self.__tracker, cpu)

    def add_entity(self, initiator, entity_id, props, active, mode):
        with self.tx(initiator):
            self.__nt_state_admin.add(entity_id, props)
            if active is not None:
                self.make_active(initiator, entity_id, active, mode)

    def remove_entity(self, initiator, entity_id):
        with self.tx(initiator):
            self.__nt_state_admin.remove(entity_id)

    def update_entity(self, initiator, entity_id, props):
        with self.tx(initiator):
            self.__nt_state_admin.update(entity_id, props)

    def make_active(self, initiator, entity_id, cpu, mode):
        with self.tx(initiator):
            if self.__active:
                self.update_entity(initiator, self.__active, {'cpu': None})
            self.update_entity(initiator, entity_id, {'cpu': [cpu, mode]})
            self.__active = entity_id

# Must be called after the Simics agent has been started on the target
def upload_binary(src_path, src_name, dst_path):
    matic = simics.SIM_run_command('start-agent-manager name = matic')
    handle = simics.SIM_run_command('%s.connect-to-agent' % matic)
    simics.SIM_run_command('%s.upload -executable %s/%s to = %s' % (
        handle, src_path, src_name, dst_path))
    simics.SIM_run_command('%s.run-until-job' % handle)
    simics.SIM_run_command('%s.delete-handle' % handle)

class MagicData:
    def __init__(self, sw, name):
        self.sw = sw
        self.name = name
        self.hits = 0

def magic_insn(magic_data, cpu, pid):
    if pid == 0xc:
        return  # Ignore magic instructions from the Simics agent
    sw = magic_data.sw
    nt_query = sw.iface.osa_node_tree_query
    active_nodes = nt_query.get_current_nodes(root_node(sw), cpu)

    active_node = nt_query.get_node(active_nodes[-1])
    stest.expect_true("pid" in active_node)
    stest.expect_equal(active_node['pid'], pid)
    stest.expect_equal(active_node['name'], magic_data.name)
    magic_data.hits += 1

def install_magic_callbacks(data):
    simics.SIM_hap_add_callback('Core_Magic_Instruction', magic_insn, data)

def test_active(sw, console):
    magic_data = MagicData(sw, "active")
    install_magic_callbacks(magic_data)
    console.command("active")
    stest.expect_equal(magic_data.hits, 1)
