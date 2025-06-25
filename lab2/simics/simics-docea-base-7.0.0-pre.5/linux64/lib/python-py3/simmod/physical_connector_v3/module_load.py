import cli

class_name = 'physical_connector_v3'


# info command prints static information
def get_info(obj):
    return [('Identity card', [('Hierarchical name', obj.name),
                               ('What am I?', obj.class_desc)])]


# status command prints dynamic information
def get_status(obj):
    return [(None, [('Platform physical model', obj.ppmf),
                    ('Platform physical model metadata', obj.ppmf_metadata),
                    ('Physical simulation started?', obj.started)])]


cli.new_info_command(class_name, get_info)
cli.new_status_command(class_name, get_status)
