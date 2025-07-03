import cli

class_name = 'float_forwarder'


# info command prints static information
def get_info(obj):
    return [('Identity card', [('Hierarchical name', obj.name),
                               ('What am I?', obj.class_desc)])]


# status command prints dynamic information
def get_status(obj):
    up_tele = [('Disabled', 'Not connected to any telemetry provider yet')]
    if obj.telemetry_provider is not None:
        up_tele = [('"Value" telemetry (class, telemetry)',
                    obj.upstream_value_telemetry),
                   ('"Time" telemetry (class, telemetry)',
                    obj.upstream_time_telemetry)]
    down_tele = [('Telemetry class', obj.downstream_telemetry_class),
                 ('"Value" telemetry', obj.downstream_value_telemetry),
                 ('"Time" telemetry', obj.downstream_time_telemetry)]
    return [('Fetched telemetry (upstream)', up_tele),
            ('Provided telemetry (downstream)', down_tele)]


cli.new_info_command(class_name, get_info)
cli.new_status_command(class_name, get_status)
