import simics_6_api as simics

simics.SIM_load_module("probe-monitor")

import conf  # noqa: F401, E402

from . import docea_streamer  # noqa: F401, E402

from . import commands  # noqa: F401, E402 isort:skip
