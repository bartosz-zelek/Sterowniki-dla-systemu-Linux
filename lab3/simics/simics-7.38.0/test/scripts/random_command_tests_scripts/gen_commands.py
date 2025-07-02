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


import random
import itertools
import shutil
import re
import os
import string
import types
import conf
import cli
import simics
from cli import simenv

disabled_commands = frozenset((
    'step-out',                  # avoid all commands that run the simulation
    'lit-capture-trace',         # since they may not terminate
    'step-over-instruction',     #
    'step-over',                 #
    'run',                       #
    'run-cycles',                # shouldn't really run without args
    'run-until-activated',       #
    'run-until-deactivated',     #
    'run-until',                 #
    'bp-run-until',              #
    'bp-run-until-notifier',     #
    'bp-run-until-msr',          #
    'bp-run-until-hap',          #
    'bp-run-until-source-location', #
    'bp-run-until-source-line',     #
    'bp-run-until-memory',          #
    'bp-run-until-console-string',  #
    'bp-run-until-control-register',#
    'bp-run-until-log',          #
    'bp-run-until-time',         #
    'bp-run-until-step',         #
    'bp-run-until-cycle',        #
    'bp-run-until-gfx',          #
    'bp-run-until-exception',    #
    'bp-run-until-magic',        #
    'bp-run-until-cycle-event',  #
    'bp-run-until-step-event',   #
    'bp-run-until-bank',         #
    'bp-run-until-network',      #
    'step-cycle',                #
    'step-instruction',          #
    'force-step-instruction',    #
    'step-into',                 #

    'quit',                      # terminates simics
    'restart-simics',            # terminates simics

    '[',                         # requires matching ]

    'wireshark',                 # launches external program
    'ethereal',                  # launches external program
    'tcpdump',

    'connect-real-network',      # avoid ports (may conflict with other tests)
    'connect-real-network-port-in',
    'connect-real-network-port-out',

    'interrupt-script-branch',   # May cause test to fail in ways that are
                                 # difficult to catch if we have active script
                                 # branches.

    'replace-conf-objects',      # May delete objects. Not fully supported
                                 # by Simics (SIMICS-10641).

    ':',                         # Uninteresting; expander requires args

    'disconnect',                # May remove the clock from devices that
                                 # will crash without one (SIMICS-16133).

    '=', '+=', '-=',             # CLI may execute the LHS as a command,
                                 # leading to hangs (SIMICS-9777).
    'start-web-gui',             # avoid port

    'write-configuration',       # may write huge files taking a long time
    'save-persistent-state',
    'record-session',
    ))

no_random_args_commands = frozenset((
    '@',                  # avoid valid python tracebacks

    '!',                  # tries to execute shell commands
    'pipe',               # tries to execute shell commands
    'shell',              # tries to execute shell commands
    'wait-for-shell',     # tries to execute shell commands

    'add-directory',
    'run-script',   # requires a sensible file-name

    'display',            # may issue commands such as run
    'exec',               # can execute any other command

    'expect',             # may terminate simics

    'pow',                # may take too long
    'range',              # may take too long or use too much memory
    'x',                  # may take too long

    'run-seconds',        # doesn't terminate
    'skip-to',            # doesn't terminate
    'stop',               # may try to interrupt script (-a flag)

    'save',               # save commands may produce large files and take
    'save-file',          # a long time.
    'save-intel-hex',
    'save-intel-obj',
    'save-intel-32-obj',
    'save-mbr-partition',
    'save-bmp',
    'save-vmem',

    'set-bookmark',
    'set-memory-limit', # too small values causes swapout errors
    'set-image-memory-limit', # too small values causes swapout errors
    ))

def all_combinations(l):
    if not l:
        return [[]]
    tails = all_combinations(l[1:])
    return [ [x]+tail for x in l[0] for tail in tails if x[1] is not None ]

# Generate a random integer in [low,high], using the random generator g.
def random_integer(g, low, high):
    def gen_edge_case(lo, hi):
        edge_cases = {lo, -1, 0, 1, hi}
        return g.choice(sorted(x for x in edge_cases if lo <= x <= hi))

    # List of 2**n in [lo, hi], lo ≥ 0.
    def powers_of_2(lo, hi):
        if 0 <= lo < hi:
            hi_power = hi.bit_length() - 1
            lo_power = 0 if lo <= 0 else lo.bit_length()
            return [1 << x for x in range(lo_power, hi_power + 1)]
        else:
            return []

    # List of 2**n and 2**n-1 in [lo, hi], lo ≥ 0.
    def quasi_powers_of_2(lo, hi):
        p = powers_of_2(lo, hi)
        return p + [x - 1 for x in p if x > lo and x > 2]

    def gen_quasi_power_of_2(lo, hi):
        pos_powers = quasi_powers_of_2(max(lo, 0), hi)
        neg_powers = [-x for x in quasi_powers_of_2(0, -lo)]
        powers = pos_powers + neg_powers
        if powers:
            return g.choice(powers)
        else:
            # If there are no (quasi) powers of two in the interval,
            # fall back to the general number generation.
            return gen_general_number(lo, hi)

    # Generate a random number in [lo,hi], lo ≥ 0,
    # exponentially biased towards small numbers.
    def exp_dist(lo, hi):
        max_bits = hi.bit_length()
        min_bits = max(lo.bit_length(), 1)
        k = g.randint(min_bits, max_bits)
        # Generate a k-bit number.
        return g.randint(max(1 << (k - 1), lo),
                         min((1 << k) - 1, hi))

    # Pick a number in [lo,hi] exponentially biased towards low magnitudes.
    def gen_general_number(lo, hi):
        if lo < -1:
            # Consider negative numbers.
            if g.randint(0, 1):
                return -exp_dist(1, -lo)
        return exp_dist(max(lo, 1), hi)

    methods = [gen_edge_case, gen_quasi_power_of_2, gen_general_number]
    m = g.choice(methods)
    n = m(low, high)
    assert low <= n <= high
    return n


# Generate a random floating-point value using the random generator g.
def random_float(g):
    def gen_zero():
        return 0.0

    def gen_common():
        return g.choice([-1.0, 0.5, 1.0, 2.0, 5.0])

    # Pick a reasonable floating-point number. Nothing too extreme.
    def gen_other():
        # Use a range of magnitudes that take most uses into account.
        exp = g.randint(-30, 30)
        val = g.randint(1, 5) / 5.0
        sign = g.choice([-1, 1])
        return sign * val * 2**exp

    methods = [gen_zero, gen_common, gen_other]
    m = g.choice(methods)
    return m()


class Randomizer:
    def __init__(self):
        # The reason for this roundabout seed selection is to make it
        # easier to reproduce runs. We could print the entire Python random
        # state but it's rather big, so we settle for a mid-sized number.
        # Let's first check if the T30_SEED env variable has been set
        # If someone wants to reproduce using a specific seed,
        # Then simply export T30_SEED=<seed value> before running the test
        t30_env_seed = os.getenv("T30_SEED")
        seed = random.getrandbits(128)
        if t30_env_seed and t30_env_seed != "":
            try:
                seed = int(t30_env_seed)
                print("IMPORTANT! Seed was set by T30_SEED in the environment")
            except ValueError:
                print("WARNING! Attempted to use '{0}'".format(t30_env_seed))
                print("as the seed, but it is not an int, using random seed.")
        print("Using random seed", seed)
        self.g = random.Random(seed)

    def get_int(self):
        return random_integer(self.g, -10000, 10000)

    def get_int64(self):
        return random_integer(self.g, cli.int64_t.min, cli.int64_t.max)

    def get_uint64(self):
        return random_integer(self.g, 0, 0xffffffffffffffff)

    def get_uint(self):
        return random_integer(self.g, 0, 20000)

    def get_float(self):
        return random_float(self.g)

    def get_addr(self):
        return int(self.g.random() * (~(1)))

    def get_string(self, min_length = 1):
        chars = string.ascii_letters + string.digits
        randstr = "".join(chars[self.g.randrange(len(chars))]
                          for i in range(self.g.randrange(min_length - 1, 20)))
        randletter = string.ascii_letters[self.g.randrange(
                len(string.ascii_letters))]
        return randletter + randstr

    def get_filename(self, must_exist):
        dirpath = os.path.join(os.getenv("SANDBOX", ""), "scratch", "dummyfiles")
        path = os.path.join(dirpath, self.get_string(5))
        if os.path.isdir(path):
            # A previous run happened to use this file to make a directory.
            # Remove it first, just in case.
            shutil.rmtree(path, ignore_errors=True)
        if must_exist:
            os.makedirs(dirpath, exist_ok=True)
            open(path, 'w')
        return path

    def flip_a_coin(self):
        return self.g.random() > 0.5

    # Return True with probability p.
    def chance(self, p):
        return self.g.random() <= p

    # Instance implements all given interfaces, or is of a given class.
    # If multiple interfaces are required, these are specified as "foo&bar"
    def instance_of_kinds(self, obj, kind):
        for k in kind.split("&"):
            if not cli.matches_class_or_iface(obj, k):
                return False
        return True

    def get_object_name(self, kinds):
        objects = set()

        # Objects can be checked by a special function
        if isinstance(kinds, types.FunctionType):
            objects.update({x for x in simics.SIM_object_iterator(None)
                            if kinds(x)})
        else:
            for kind in kinds:
                objects.update(
                    {x for x in simics.SIM_object_iterator(None)
                     if kind == None or self.instance_of_kinds(x, kind)})
        if len(objects) == 0:
            return ""
        return self.g.choice(sorted(objects)).name

    def get_int_in_range(self, min, max):
        return random_integer(self.g, min, max)

    def get_bool(self):
        if self.flip_a_coin():
            return "TRUE"
        return "FALSE"

    def choice(self, elements):
        return self.g.choice(sorted(elements, key=str))

    def get_poly_value(self, typelist):
        t = typelist[self.g.randrange(len(typelist))]
        if t == cli.int_t:
            return self.get_int()
        elif t == cli.uint_t:
            return self.get_uint()
        elif t == cli.int64_t:
            return self.get_int64()
        elif t == cli.uint64_t:
            return self.get_uint64()
        elif t == cli.str_t:
            return self.get_string()
        elif t == cli.addr_t:
            return self.get_addr()
        elif t == cli.float_t:
            return self.get_float()
        elif isinstance(t, cli.obj_t):
            return self.get_object_name(t.kinds)
        elif t == cli.nil_t:
            return None
        elif t == cli.list_t:
            # TODO: generate something better than empty list
            return []
        elif isinstance(t, cli.bool_t):
            return self.get_bool()
        else:
            raise Exception("Unsupported poly value type: %s" % str(t))

    def shuffle(self, l):
        self.g.shuffle(l)

randomizer = Randomizer()

# Holds information about a single command invocation.
class Invocation:
    def __init__(self, object, command, args):
        self.object = object
        if command.has_object():
            self.initial_object_class = self.get_object_classname()
        self.command = command
        self.args = args
        self.command_line = None

        # for now, only 'info' and 'status' with 0 arguments must succeed
        self.may_fail = not (self.command.has_object()
                             and self.command.get_method() in ['info', 'status']
                             and not self.args)

    def get_arg(self, i):
        return str(self.args[i][1])

    def is_valid(self):
        if self.command.has_object() and not self.object_exists():
            print(self, "not valid: object does not exist.")
            return False
        if not self.command.is_valid():
            try:
                print(self, "not valid: command is not valid.")
            except:
                print(self.command.get_name(), "not valid: command is not valid.")
            return False
        return True

    def object_exists(self):
        try:
            simics.SIM_get_object(self.get_object_name())
        except simics.SimExc_General:
            return False
        return True

    def get_num_args(self):
        return len(self.args)

    def get_object_name(self):
        return self.object.name

    def get_object_classname(self):
        return simics.SIM_get_object(self.get_object_name()).classname

    def has_object(self):
        return bool(self.object)

    def has_args(self):
        return bool(self.args)

    def get_command_line(self):
        command_line = ""

        # Infix commands have two args
        if self.command.is_infix():
            if not self.has_args():
                command_line = self.command.get_name()

            elif len(self.args) == 2:
                command_line = "%s %s %s" % (self.get_arg(0),
                                             self.command.get_name(),
                                             self.get_arg(1))

            else:
                print(("infix command invocation '%s' has %d args, expected 2"
                       % (self.command.get_name(), self.get_num_args())))
        else:
            if self.has_object():
                command_line = "%s.%s" % (self.get_object_name(),
                                           self.command.get_method())
            else:
                command_line = "%s" % (self.command.get_name())

            for key, val in self.args:
                if key and val:
                    command_line += " %s = %s" % (key, val)
                elif val:
                    command_line += " %s" % val

        self.command_line = command_line

        return command_line

    def __str__(self):
        return self.get_command_line()

# Convert a value to a properly quoted CLI string.
def cli_quote(val):
    if isinstance(val, str):
        # This set of characters that must be quoted was lifted from
        # the tab-completion insertion code, so it should correspond to
        # the intent of the expander.
        if re.search(r'[ !"#$%&\'()*+\,/;<=>?@\[\\\]^`{|}]', val):
            return '"%s"' % val.replace("\\", "\\\\")
        else:
            return val
    else:
        # Should work for all other types.
        return str(val)

# Wrapper around CLI command objects
class Command:
    def __init__(self, cmd):
        # CLI command object
        self.cmd = cmd                  # command object

    def is_valid(self):
        # if command is disabled or the namespace object invalid,
        # don't execute any commands
        return not (self.get_name() in disabled_commands
                    or self.get_method() in disabled_commands)

    # Return the command's name
    def get_name(self):
        return self.cmd.name

    # Returns True/False if the command is a namespace-command (i.e. has an
    # associated object.
    def has_object(self):
        return bool(self.get_namespace())

    # Returns the commands namespace (object)
    def get_namespace(self):
        return self.cmd.cls or self.cmd.iface

    def is_infix(self):
        return self.cmd.infix

    # Return's the method name
    def get_method(self):
        return self.cmd.method

    # Return a list of all objects to which this command applies
    def get_confobjs(self):
        if not self.has_object():
            return []
        else:
            return sorted(obj for obj in simics.SIM_object_iterator(None)
                          if cli.matches_class_or_iface(
                                  obj, self.get_namespace()))

    def get_args(self):
        # -> is a special operator with lots of magic
        # in particular it takes 4 arguments, but we only deal with two
        if self.get_name() == '->':
            return self.cmd.args[:2]
        return self.cmd.args

    # Return an argument value (on the usual 2-tuple form),
    # or None if no value could be generated.
    def generate_arg_value(self, name, handler):
        if handler == cli.int_t:
            return (name, str(randomizer.get_int()))

        elif handler == cli.uint_t:
            return (name, str(randomizer.get_uint()))

        elif handler == cli.str_t:
            return (name, randomizer.get_string())

        elif handler == cli.flag_t:
            return (None, name if randomizer.flip_a_coin() else "")

        elif handler == cli.addr_t:
            return (name, str(randomizer.get_addr()))

        elif handler == cli.float_t:
            return (name, str(randomizer.get_float()))

        elif isinstance(handler, cli.filename_t):
            return (name, randomizer.get_filename(handler.must_exist))

        elif isinstance(handler, cli.obj_t):
            return (name, randomizer.get_object_name(handler.kinds))

        elif isinstance(handler, cli.range_t):
            return (name,
                    str(randomizer.get_int_in_range(handler.min, handler.max)))

        elif isinstance(handler, cli.bool_t):
            return (name, randomizer.get_bool())

        elif isinstance(handler, cli.string_set_t):
            return (name,
                    randomizer.choice(list(handler.strings.keys())))

        elif isinstance(handler, cli.poly_t):
            val = randomizer.get_poly_value(handler.types)
            if val:
                return (name, str(val))
            else:
                return None

        elif handler == cli.list_t:
            # TODO: generate something better than empty list
            return (name, [])
        elif handler == cli.nil_t:
            return (name, None)
        elif handler == cli.ip_port_t:
            return (name, str(randomizer.get_int_in_range(0, 65535)))
        else:
            raise Exception("Argument %s not supported for command %s"
                            % (str(handler), self.get_name()))

    def generate_invocations_for_object(self, obj):
        # try calling the command with no args
        if not self.is_infix():
            inv = Invocation(obj, self, [])
            yield inv

        if (self.get_name() in no_random_args_commands
            or self.get_method() in no_random_args_commands):
            return

        arg_values = []

        for arg in self.get_args():
            # Take a value from the expander, if there is one, most of the time.
            expander_value = None
            if randomizer.chance(0.8):
                candidates = arg.call_expander("", obj, cmd=self.cmd)
                if candidates:
                    choice = randomizer.choice(candidates)
                    # File name completers return a tuple (string, isdir),
                    # which suits us fine because we don't want to run
                    # commands with completed file names anyway for safety
                    # reasons, in case the command would alter the file in
                    # some way.
                    if not isinstance(choice, tuple):
                        expander_value = (None, cli_quote(choice))

            if expander_value is not None:
                arg_value = [expander_value]
            else:
                if isinstance(arg.handler, tuple):
                    arg_alts = zip(arg.name, arg.handler)
                else:
                    arg_alts = [(arg.name, arg.handler)]

                arg_value = []
                for (name, handler) in arg_alts:
                    v = self.generate_arg_value(name, handler)
                    if v is not None:
                        arg_value.append(v)

            arg_values.append(arg_value)

            # Now create all possible combination of arguments
            for cmd_args in all_combinations(arg_values):
                if len(cmd_args) > 0:
                    inv = Invocation(obj, self, cmd_args)
                    yield inv

    def generate_invocations(self):
        if self.has_object():
            return itertools.chain(*(self.generate_invocations_for_object(obj)
                                     for obj in self.get_confobjs()))
        else:
            return self.generate_invocations_for_object(None)

    def __str__(self):
        if self.has_object():
            return "Command(%s.%s)" % (self.get_namespace(), self.get_method())
        else:
            return "Command(%s)" % self.get_name()

    def gen_commands(self):
        return [inv.get_command_line()
                for inv in self.generate_invocations() if inv.is_valid()]

def get_local_classes():
    local_classes = set()
    for m in simics.SIM_get_all_modules():
        path = m[1]
        classes = m[7]
        components = m[9]
        if os.path.relpath(path, conf.sim.project).startswith('..'):
            continue
        if classes:
            for cls in classes:
                local_classes.add(cls)
        if components:
            for comp in components:
                local_classes.add(comp)
    return local_classes

def is_local_command(cmd, local_classes):
    if cmd.cls:
        return local_classes == [] or cmd.cls in local_classes
    if not hasattr(cmd.fun, '__code__'):
        return True
    filename = cmd.fun.__code__.co_filename
    if filename in ['sim_commands.py', 'sim_profile_commands.py']:
        return False
    elif not os.path.isfile(filename): # TODO: NOT PERFECT
        return False
    return True

def gen_random_command_sequence(project_only,
                                user_disabled_commands,
                                user_disabled_classes,
                                user_disabled_interfaces,
                                max_num_commands = None):
    # Allow deprecated commands to be tested and ignore warnings
    conf.sim.deprecation_level = 0
    conf.sim.stop_on_error = False

    # Generate the commands without duplication and in random order
    cmds = []
    local_classes = get_local_classes() if project_only else []
    for c in cli.simics_commands():
        if project_only and not is_local_command(c, local_classes):
            continue
        if c.name in disabled_commands:
            continue
        if c.cls in user_disabled_classes:
            continue
        if c.iface in user_disabled_interfaces:
            continue
        if c.name in user_disabled_commands:
            continue
        cmds.extend(Command(c).gen_commands())
    cmds = sorted(set(cmds))
    randomizer.shuffle(cmds)

    if max_num_commands is not None:
        cmds = cmds[:max_num_commands-1]

    # Run each command twice in succession, on the grounds that this
    # may provoke certain bugs to appear.
    ret = []
    for c in cmds:
        ret.extend((c, c))
    return ret

def main():
    simics.SIM_run_command_file(simics.SIM_lookup_file(simenv.machine_script),
                                False)
    with open(simenv.command_file, 'w') as f:
        commands = gen_random_command_sequence(simenv.project_only,
                                               simenv.disabled_commands,
                                               simenv.disabled_classes,
                                               simenv.disabled_interfaces)
        f.write(''.join(c + '\n' for c in commands))

if __name__ == "__main__":
    main()
