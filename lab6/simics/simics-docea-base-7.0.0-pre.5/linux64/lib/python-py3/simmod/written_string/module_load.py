import cli

class_name = 'written_string'


# info command prints static information
def get_info(obj):
    return [('Identity card', [('Hierarchical name', obj.name),
                               ('What am I?', obj.class_desc),
                               ('Power/Thermal name', obj.power_thermal_name)])
            ]


# status command prints dynamic information
def get_status(obj):
    telemetry = [('Disabled', 'Not connected to any telemetry provider yet')]
    if obj.telemetry_provider is not None:
        telemetry = [('"Value" telemetry (class, telemetry)',
                      obj.upstream_value_telemetry),
                     ('"Time" telemetry (class, telemetry)',
                      obj.upstream_time_telemetry)]
    return [('Fetched telemetry (upstream)', telemetry)]


cli.new_info_command(class_name, get_info)
cli.new_status_command(class_name, get_status)
