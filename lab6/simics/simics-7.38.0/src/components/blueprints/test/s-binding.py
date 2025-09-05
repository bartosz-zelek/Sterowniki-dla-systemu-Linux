# © 2025 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

from blueprints import (instantiate, Builder, Namespace, Binding,
                        ConfObject, blueprint, BlueprintError)
import simics
import conf
import cli
import stest

class Server:
    cls = simics.confclass("server")
    cls.attr.client("o|n")

class Client:
    cls = simics.confclass("client")
    cls.attr.server("o|n")

class Connection(Binding):
    server = ConfObject()
    client = ConfObject()

# Blueprint must be registered with Simics to make CLI commands work on it
@blueprint
def server(builder: Builder, name: Namespace):
    con = builder.expose_state(name, Connection)
    con.server = builder.obj(name, "server", client=con.client)

@blueprint
def client(builder: Builder, name: Namespace):
    con = builder.read_state(name, Connection)
    con.client = builder.obj(name, "client", server=con.server)

def top(builder: Builder, name: Namespace):
    builder.expand(name, "server", server)
    builder.establish_binding(Connection, name.server, name.client)
    builder.expand(name, "client", client)

name = "top"
instantiate(name, top)
stest.expect_equal(cli.global_cmds.list_objects(namespace=name),
                   [conf.top.client, conf.top.server])
stest.expect_equal(conf.top.server.client, conf.top.client)
stest.expect_equal(conf.top.client.server, conf.top.server)

def error_top(builder: Builder, name: Namespace):
    builder.expand(name, "server", server)
    builder.establish_binding(Connection, name.server, name.client)
    builder.establish_binding(Connection, name.server, name.client2)
    builder.expand(name, "client", client)
    builder.expand(name, "client2", client)

stest.expect_exception(instantiate, (name, error_top), BlueprintError)

# Server exposes Connection on its top node
stest.expect_equal(cli.global_cmds.list_blueprint_state(blueprint="server"),
                   [["<top>", "Connection"]])
# and expects noting
stest.expect_equal(cli.global_cmds.list_blueprint_state(blueprint="server",
                                                        _expected=True),
                   [])

# Client exposes nothing
stest.expect_equal(cli.global_cmds.list_blueprint_state(blueprint="client"), [])
# and expects a Connection to be available on its top node
stest.expect_equal(cli.global_cmds.list_blueprint_state(blueprint="client",
                                                        _expected=True),
                   [["<top>", "Connection"]])
