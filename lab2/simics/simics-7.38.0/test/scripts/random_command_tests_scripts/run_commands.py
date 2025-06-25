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


import cli, cli_impl
import os, sys, time, traceback
import conf
import simics

# TODO: Empty this list. (bug 14070)
# If you add something to this list, make sure to also create a bug
# for it and reference it!
skip_commands = (
    'instantiate-components',       # fails when broken create-/new- commands (bug 16286)
    'move-object',                  # changes components too much (bug 16318)
    "switch-to-telnet-console",     # bug 16325
    "switch-to-host-serial-console",# bug 16325
    'new-processor-core2-solo',     # SIMICS-9028

    'create-sdram-memory-module',# SIMICS-9039
    'create-ddr-memory-module',  #
    'create-ddr2-memory-module', #

    'delete',                    # need more cleanup of references to deleted
                                 # objects before enabling <component>.delete

    'lit-recorder-start',        # intelbug 114
    'lcat-recorder-start',       # Similar  behavior as lit-recorder-start, so
                                 # intelbug 114 again
    'add-diff-file',             # SIMICS-12257 - add-diff-file throws segfault
    'add-partial-diff-file',     # SIMICS-12257 - add-diff-file throws segfault
    )

def command_match(cmd, line):
    # remove arguments and namespaces from line before comparing
    return cmd == line.split(" ")[0].rsplit(".")[-1]

def skip_command(cmd):
    return any(command_match(c, cmd) for c in skip_commands)

def pr(s):
    print(s, file=sys.stderr)
    sys.stderr.flush()

def main():
    machine_script = os.getenv("T30_MACHINE_SCRIPT")

    # Allow deprecated commands to be tested and ignore warnings
    conf.sim.deprecation_level = 0
    conf.sim.stop_on_error = False

    simics.SIM_run_command_file(simics.SIM_lookup_file(machine_script), False)

    errors = False

    start = time.time()
    cmd_start = 0

    # NB: don't use Simics variables: they may be removed by the unset command.
    start_line = int(os.getenv("T30_START_LINE"))
    command_file = os.getenv("T30_CMD_FILE")

    for i, cmd in enumerate(open(command_file)):
        if (i + 1) < start_line:
            continue
        cmd = cmd.rstrip('\n')
        if skip_command(cmd):
            pr("SKIPPING line %d: %s" % (i + 1, cmd))
            continue
        # print time since last command and since start
        now = time.time()
        pr("RUNNING line %d (%.2f %.2f) %s"
           % (i + 1, now - cmd_start, now - start, cmd))
        cmd_start = now
        try:
            # use run_command_retval() to prevent interactive return messages
            # such as "overlapping breakpoints" that take too long to generate
            cli_impl.run_command_retval(cmd)
        except (cli.CliError, cli_impl.CliException):
            # run_command_retval() does not translate CliException -> CliError
            # as run_command() would have done for us
            pass
        except Exception as ex:
            pr("\nERROR COMMAND: %s" % cmd)
            pr("EXCEPTION %s %s" % (type(ex), ex))
            traceback.print_exc()
            pr("")
            errors = True
            break

    if errors:
        pr("\n")
        raise Exception(
            "Test Failed. Search this log file for \"ERROR COMMAND\""
            "to locate the offending command and the error message.")

if __name__ == "__main__":
    main()
