# © 2017 Intel Corporation

import instrumentation

# USER-TODO: Update all parameters
instrumentation.make_tool_commands(
    "empty_tool",
    object_prefix = "empty",
    provider_requirements = "conf_object",
    new_cmd_doc = """Creates a new empty tool object which
    can be connected to providers. The only restriction on the provider is that
    it implements the conf_object interface (all objects do this). In a more
    realistic tool the provider requirements should be more specific.""")
