# © 2017 Intel Corporation

import instrumentation
import cli

# USER-TODO: Add tool specific commands here. The "connections" command is
# just an example and could be used as template.

def connections_cmd(obj):
    print("Tool is connected to the following objects:")
    for o in obj.connections:
        print(o.provider.name)

cli.new_command("connections", connections_cmd,
                [],
                cls = "empty_tool",
                type = ["Instrumentation"],
                short = "print all connections",
                doc = ("""
This tool is a empty tool that can be used to develop other tools."""))
