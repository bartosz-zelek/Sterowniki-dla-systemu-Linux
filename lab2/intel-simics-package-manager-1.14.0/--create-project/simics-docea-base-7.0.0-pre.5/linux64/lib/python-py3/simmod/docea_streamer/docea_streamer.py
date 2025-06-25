import simics_6_api as simics
from simmod.probe_monitor import sampler
from simmod.probe_monitor import streamer as base_streamer


class docea_streamer(base_streamer.probe_streamer):
    __slots__ = ()

    cls = simics.confclass(
        "docea_streamer",
        parent=base_streamer.probe_streamer.cls,
        pseudo=True,
        short_doc="probe streamer dedicated to Docea data",
        doc="Streamer extending on the standard probe-streamer"
        " and dedicated to streaming out Docea data")

    def create_sampling(self):
        if self.mode == sampler.NOTIFIER_MODE:
            return self.MonoThreadNotificationSampling(self,
                                                       self.notifier_type,
                                                       self.notifier_obj)
        else:
            return super().create_sampling()

    @cls.finalize
    def finalize_instance(self):
        simics.SIM_log_warning(
            self.obj, 0, "The docea-streamer module is DEPRECATED."
            " Consider using Simics' probe streamer instead.")
        super().finalize_instance()

    class MonoThreadNotificationSampling(
            base_streamer.probe_streamer.NotificationSampling):
        def notified(self, obj, src, data):
            super().sample_event()
