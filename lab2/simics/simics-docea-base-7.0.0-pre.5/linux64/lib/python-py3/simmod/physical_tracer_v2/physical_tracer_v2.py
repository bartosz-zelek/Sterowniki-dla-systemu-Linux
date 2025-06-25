# -*- coding: utf-8 -*-

import csv
import os

import pyobj
# Tie code to specific API, simplifying upgrade to new major version
import simics_6_api as simics


class physical_tracer_v2(pyobj.ConfObject):
    """A module that is able to dump in a CSV column-based file the changes
    over time of a set of parameters specified by the user. The size of the
    CSV file is capped to 1GB."""

    _class_desc = """A module that is able to dump in a CSV column-based file
    the changes over time of a set of parameters specified by the user.
    The size of the CSV file is capped to 1GB."""

    # Prevents registration in case the file is imported from another Simics
    # module
    _do_not_init = object()

    def _initialize(self):
        super()._initialize()
        self.notifier = None
        self.notifier_handle = None
        self.time_telemetry_class_id = -1
        self.time_telemetry_id = -1
        self.observer = None
        self.traced_parameters = []
        self.csv_file = None

    def _finalize(self):
        simics.SIM_log_warning(
            self.obj, 0, "The physical-tracer-v2 module is DEPRECATED."
            "Consider using the probe framework instead.")

    def _info(self):
        return [('Identity card', [('Hierarchical name', self.obj.name),
                                   ('What am I?', self.obj.class_desc)])]

    def _status(self):
        return [(None, [('Traced parameters', len(self.traced_parameters)),
                        ('CSV file', self.csv_file)])]

    def _get_backup_csv_file_name(self):
        """Infer the path to the backup csv file from the csv file
        path, in case the csv file already exists"""
        # backup in the same directory (hence no need to check directory
        # exists)
        return self.csv_file + "~" if self.csv_file else None

    def _create_backup_file(self):
        """Create a backup of the csv file in case it already exists.
        This backup file is created in the same directory, a ~ is
        appended at the end of the file name. If this backup file
        already exists, then it is overridden."""
        if os.path.exists(self.csv_file):
            try:
                os.rename(self.csv_file, self._get_backup_csv_file_name())
            except OSError:
                simics.SIM_log_error(self.obj, 0, "cannot create backup file")

    def _register_callbacks(self):
        """Register the callback on events notified by the observer. After this
        call, values are dumped to the csv file."""
        if self.observer is not None:
            self.notifier = self.observer
            self.notifier_handle = simics.SIM_add_notifier(
                self.notifier,
                simics.SIM_notifier_type('ISIM-POWER-THERMAL-RESULTS'),
                self.obj, self._available_results_callback, None)
            telemetry = self.observer.iface.telemetry
            self.time_telemetry_class_id = telemetry.get_telemetry_class_id(
                'ISIM-POWER-THERMAL')
            self.time_telemetry_id = telemetry.get_telemetry_id(
                self.time_telemetry_class_id, 'NOTIFIED-RESULTS-TIME-PS')

    def _unregister_callbacks(self):
        """Unregister the callback on events notified by the observer. After
        this call, no more values are dumped to the csv file."""
        if self.notifier is not None and self.notifier_handle is not None:
            simics.SIM_delete_notifier(self.notifier, self.notifier_handle)
            self.notifier = None
            self.notifier_handle = None
        self.time_telemetry_class_id = -1
        self.time_telemetry_id = -1

    def _check_observer_has_metadata(self, observer):
        if observer is None:
            return
        try:
            mf = simics.SIM_get_attribute(observer, 'ppmf_metadata')
            if mf is None or mf == '':
                raise ValueError('invalid ppmf_metadata')
        except Exception:
            # Fallback to old attribute name
            try:
                mf = simics.SIM_get_attribute(observer, 'metadata_file')
                if mf is None or mf == '':
                    raise ValueError('invalid metadata_file')
            except Exception:
                # neither ppmf_metadata nor metadata_file is correctly set
                simics.SIM_log_error(
                    self.obj, 0,
                    'the connected observer does not have a valid metadata '
                    + 'file so the trace will contain only zero values')

    def _initialize_csv_file(self):
        """Initialize the CSV file, that is move the file to a backup by
        suffixing its name with '~' if the CSV file already exists, and write
        the header in a new empty file. This function doesn't do
        anything if either the observer, or the traced parameters or the
        CSV file path is not configured."""
        if self.observer is not None and len(
                self.traced_parameters) > 0 and self.csv_file is not None:
            self._create_backup_file()
            with open(self.csv_file, "w+", newline='') as csv_file:
                writer = csv.writer(csv_file)
                writer.writerow(self._generate_csv_header())

    def _available_results_callback(self, subscriber, notifier, context):
        """Fetch the timestamp of the available results and dump."""
        if self.observer is None:
            simics.SIM_log_error(self.observer, 0, "missing observer")
            return
        time_ps = self.observer.iface.telemetry.get_value(
            self.time_telemetry_class_id, self.time_telemetry_id)
        self._fetch_and_dump_to_csv(time_ps)

    def _fetch_and_dump_to_csv(self, time_ps):
        """For all entries in traced_parameters, call the right function of the
        observer and write in the csv all the values for the given time at
        once."""
        vs = [time_ps]
        for tp in self.traced_parameters:
            if len(tp) == 2:
                v = self._get_implicit_value(time_ps, tp)
            else:
                v = self._get_explicit_value(time_ps, tp)

            vs.append(str(v))

        self._append_row_to_csv(vs)

    def _get_value(self, time_ps, logical_name, value_reader_func):
        """Get the value from docea_model_obs for the given logical name at
        the given time by calling the function identified by value_reader_func
        on docea_model_obs."""
        if value_reader_func is None:
            return None

        f = getattr(self.observer.iface.docea_model_obs, value_reader_func)
        if f is None:
            return None

        v = f(time_ps, logical_name).value
        simics.SIM_log_info(
            4, self.obj, 0, '{}.{}: {}'.format(logical_name, value_reader_func,
                                               str(v)))
        return v

    def _get_value_reader_func(self, nature):
        """Get the name of the function to be called on docea_model_obs given
        the nature of the observable. """
        if nature in ['frequency', 'voltage', 'cdyn', 'current']:
            return 'power_float_variable'

        if nature == 'probe temperature':
            return 'thermal_probe_temperature'

        if nature == 'source temperature':
            return 'thermal_source_temperature'

        return None

    def _get_implicit_value(self, time_ps, tp):
        """Get observable values in case observable is specified as a
        list of size 2, that is, in case the callback is inferred from
        the nature of the observable."""

        return self._get_value(time_ps, tp[0],
                               self._get_value_reader_func(tp[1]))

    def _get_explicit_value(self, time_ps, tp):
        """Get observable values in case observable is specified as a
        list of size 3, that is, in case the callback is explicitly
        specified by the user."""
        return self._get_value(time_ps, tp[0], tp[1])

    def _append_row_to_csv(self, values):
        """Append the given values at the end of the CSV file. The
        parameter values is composed of the time and a value for each
        element in the traced parameters. These values must be sorted
        identically to the traced_parameters."""
        is_first_trace = not self._has_at_least_one_trace(self.csv_file)
        with open(self.csv_file, "a", newline='') as csv_file:
            # Write at least one trace per file
            if is_first_trace or self._is_valid_total_data_size(csv_file):
                writer = csv.writer(csv_file)
                writer.writerow(values)

    def _has_at_least_one_trace(self, csv_file_path):
        """Return true if the csv file has at least one trace (two rows:
        header and first trace) and false otherwise"""
        if not os.path.exists(csv_file_path):
            return False
        with open(self.csv_file, "r") as file:
            line_index = 0
            for row in csv.reader(file, delimiter=','):
                line_index += 1
                if line_index >= 2:
                    break
            return line_index > 1

    def _is_valid_total_data_size(self, csv_file):
        """Return true if the CSV file total size is within the user-defined
        limits. Otherwise return false."""
        # Get file size in gigabytes:
        total_data_size = csv_file.tell() * 1e-09
        # Check limits
        if total_data_size > self.max_total_size_gb.val:
            simics.SIM_log_error(
                self.obj, 0, "CSV files exceeded max allowed size of "
                + str(self.max_total_size_gb.val) + "GB")
            return False
        return True

    def _generate_csv_header(self):
        """Generate the header of the CSV that will contain the values
        changes of the traced parameters."""
        header = ["time"]
        for tp in self.traced_parameters:
            if len(tp) == 2:
                header.append("{}".format(tp[0]))
            else:
                header.append("{} ({})".format(tp[0], tp[2]))

        return header

    def _set_observer(self, val):
        if val is not None and simics.SIM_get_interface(
                val, 'docea_model_obs') is None:
            simics.SIM_log_error(
                self.obj, 0,
                "observer must implement the docea_model_obs interface")
            return simics.Sim_Set_Illegal_Value
        if val is not None and simics.SIM_get_interface(val,
                                                        'telemetry') is None:
            simics.SIM_log_error(
                self.obj, 0, "observer must implement the telemetry interface")
            return simics.Sim_Set_Illegal_Value

        self._check_observer_has_metadata(val)
        self._unregister_callbacks()
        self.observer = val
        self._register_callbacks()

    class connector(pyobj.Attribute):
        """The instance of an object that notifies events from the physical
        model and that allows to observe parameters in the physical model.
        This tracer collects the data exposed by the connector."""

        attrattr = simics.Sim_Attr_Optional
        attrtype = 'o|n'

        def getter(self):
            return self._up.observer

        def setter(self, val):
            return self._up._set_observer(val)

    class observer(pyobj.Attribute):
        """(DEPRECATED) The instance of an object that notifies events from the
        physical model and that allows to observe parameters in the physical
        model. This tracer collects the data exposed by the observer. This
        attribute is replaced the `connector` attribute."""

        attrattr = simics.Sim_Attr_Optional
        attrtype = 'o|n'

        def getter(self):
            return self._up.observer

        def setter(self, val):
            return self._up._set_observer(val)

    class traced_parameters(pyobj.Attribute):
        """List of parameters identified by their logical names,
        for which the value changes in time are dumped in the CSV file.
        Each parameter is specified using a one-dimensional array of
        size:
        - either 2: a logical name followed by a 'nature'. Ths nature
        can be 'frequency', 'voltage', 'cdyn', 'current', 'probe
        temperature' or 'source temperature'
        - or 3: a logical name followed by the name of the callback
        function to be called on the docea model observer to retrieve
        its value, followed by a description string that is output in
        the CSV file header along to the logical name.
        """

        attrattr = simics.Sim_Attr_Optional
        attrtype = '[[ss]|[sss]*]'

        def getter(self):
            return self._up.traced_parameters

        def setter(self, val):

            if len(self._up.traced_parameters) > 0:
                simics.SIM_log_error(self._up.obj, 0,
                                     "traced_parameter can be set only once")
                return simics.Sim_Set_Illegal_Value

            # Skip non significant change
            if len(val) == 0 and len(self._up.traced_parameters) == 0:
                return simics.Sim_Set_Ok

            self._up.traced_parameters = val
            self._up._initialize_csv_file()

    class csv_file(pyobj.Attribute):
        """Path to the file in which the trace will be dumped."""

        attrattr = simics.Sim_Attr_Optional
        attrtype = 's|n'

        def getter(self):
            return self._up.csv_file

        def setter(self, val):
            self._up.csv_file = val
            self._up._initialize_csv_file()

    class max_total_size_gb(pyobj.Attribute):
        """Trace file's maximum total size. When reaching this size, the
        tracer logs an error and stops writing values. Specified in
        gigabytes. The default value is 1 GB."""

        attrattr = simics.Sim_Attr_Optional
        attrtype = 'f|i'

        def _initialize(self):
            self._max_total_file_size = 50
            self._default_size = 1
            self.val = self._default_size

        def getter(self):
            return self.val

        def setter(self, val):
            if not self._is_valid_val(val):
                return simics.Sim_Set_Illegal_Value
            self.val = val

        def _is_valid_val(self, val):
            if val <= 0:
                msg = "Attribute max_total_size_gb must be greater than 0"
                simics.SIM_log_error(self._up.obj, 0, msg)
                return False
            elif val > self._max_total_file_size:
                msg = "Attribute max_total_size_gb must be less than " + str(
                    self._max_total_file_size) + " gigabytes"
                simics.SIM_log_error(self._up.obj, 0, msg)
                return False
            return True
