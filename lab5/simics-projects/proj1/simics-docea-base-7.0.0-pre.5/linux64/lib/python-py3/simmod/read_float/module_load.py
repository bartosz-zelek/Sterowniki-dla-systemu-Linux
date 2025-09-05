import cli

class_name = 'read_float'


# info command prints static information
def get_info(obj):
    return [('Identity card', [('Hierarchical name', obj.name),
                               ('What am I?', obj.class_desc),
                               ('Power/Thermal name', obj.power_thermal_name)])
            ]


# status command prints dynamic information
def get_status(obj):
    telemetry = [('Disabled',
                  'Not connected to any time provider (picosecond clock) yet')]
    if obj.time_provider is not None:
        telemetry = [('Telemetry class', obj.downstream_telemetry_class),
                     ('"Value" telemetry', obj.downstream_value_telemetry),
                     ('"Time" telemetry', obj.downstream_time_telemetry)]
    return [('Provided telemetry (downstream)', telemetry)]


cli.new_info_command(class_name, get_info)
cli.new_status_command(class_name, get_status)
