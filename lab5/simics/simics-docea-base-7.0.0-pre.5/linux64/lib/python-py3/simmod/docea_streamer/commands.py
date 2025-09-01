import simics_6_api as simics
from cli import new_info_command, new_status_command
from simmod.probe_monitor import commands as base_commands

simics.SIM_load_module("probe-monitor")

docea_streamer_cmd_setr = base_commands.CmdSet()
docea_streamer_cmd_setr.add_command("start", base_commands.start_cmd,
                                    base_commands.start_kwargs)
docea_streamer_cmd_setr.add_command("stop", base_commands.stop_cmd,
                                    base_commands.stop_kwargs)
docea_streamer_cmd_setr.add_command("delete", base_commands.delete_cmd,
                                    base_commands.delete_kwargs)
docea_streamer_cmd_setr.add_command("add-probe", base_commands.add_probe_cmd,
                                    base_commands.add_probe_kwargs)
docea_streamer_cmd_setr.add_command("remove-probe",
                                    base_commands.remove_probe_cmd,
                                    base_commands.remove_probe_kwargs)
docea_streamer_cmd_setr.add_table_command("summary", base_commands.summary_cmd,
                                          base_commands.summary_kwargs)
docea_streamer_cmd_setr.add_command("sampling-settings",
                                    base_commands.sampling_settings_cmd,
                                    base_commands.sampling_settings_kwargs)

docea_streamer_cmd_setr.apply_to("docea_streamer", "Docea streamer")

new_info_command("docea_streamer", base_commands.get_info)
new_status_command("docea_streamer", base_commands.get_status)
