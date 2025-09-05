# © 2010 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


from simmod.x86_comp.x86_processor import processor_x86_apic

class processor_nehalem(processor_x86_apic):
    """Base class for Nehalem processors."""
    _do_not_init = object()

    def _initialize(self, cpu_threads = 2, cpu_cores = 4):
        processor_x86_apic._initialize(self, cpu_threads, cpu_cores)
        self.cpuclass = 'x86-nehalem'

    def setup(self, cpu_name="", cpu_stepping=0):
        processor_x86_apic.setup(self)
        if not self.instantiated.val:
            self.add_nehalem_objects(cpu_name, cpu_stepping)

    def add_nehalem_objects(self, cpu_name, cpu_stepping):
        for t in self.get_all_threads():
            t.cpuid_stepping = cpu_stepping
            self.intel_cpu_name(t, cpu_name)

            # Pad the reserved APIC IDs for the core level to
            # represent 8 cores (for 16 logical CPUs in total)
            if self.cpu_cores <= 8:
                t.cpuid_core_level_apic_id_shift_count = 3

class processor_x86QSP1(processor_nehalem):
    '''x86-64 QSP processor with variable number of cores and threads.'''
    _class_desc = u'model of x86-64 QSP'
    _no_new_command = object()
    def _initialize(self):
        processor_nehalem._initialize(self, 1, 1)
        self.cpuclass = 'x86QSP1'

    def setup(self):
        processor_nehalem.setup(self, cpu_name="x86-64 QSP",
                                cpu_stepping=8)

class processor_core_i7(processor_nehalem):
    '''Intel® Core™ i7 processor.'''
    _class_desc = u'model of Intel® Core™ i7 processor'
    _no_new_command = object()
    def _initialize(self, cpu_threads = 2, cpu_cores = 4):
        processor_nehalem._initialize(self, cpu_threads, cpu_cores)
        self.cpuclass = 'x86-nehalem'

    def setup(self):
        processor_nehalem.setup(self, cpu_name="Intel(R) Core(TM) i7 CPU",
                                cpu_stepping=4)

class processor_core_i7_single(processor_core_i7):
    '''Intel® Core™ i7 processor with just one core and one thread.'''
    _class_desc = u'model of Intel® Core™ i7 1 core 1 thread proc'
    _no_new_command = object()
    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        processor_core_i7._initialize(self, cpu_threads, cpu_cores)
        self.cpuclass = 'x86-nehalem'

    def setup(self):
        processor_nehalem.setup(self, cpu_name="Intel(R) Core(TM) i7 CPU",
                                cpu_stepping=8)

class processor_core_i7_duo(processor_core_i7):
    '''Intel® Core™ i7 processor with 1 core a and 2 threads.'''
    _class_desc = u'model of Intel® Core™ i7 1 core 2 threads proc'
    _no_new_command = object()
    def _initialize(self, cpu_threads = 2, cpu_cores = 1):
        processor_core_i7._initialize(self, cpu_threads, cpu_cores)
        self.cpuclass = 'x86-nehalem'

    def setup(self):
        processor_nehalem.setup(self, cpu_name="Intel(R) Core(TM) i7 CPU",
                                cpu_stepping=8)

class processor_core_i7_8c_4t(processor_core_i7):
    '''Intel® Core™ i7 processor with 8 cores and 4 threads.'''
    _class_desc = u'model of Intel® Core™ i7 8 cores 4 threads proc'
    _no_new_command = object()
    def _initialize(self):
        processor_core_i7._initialize(self, 4, 8)
        self.cpuclass = 'x86-nehalem'

    def setup(self):
        processor_nehalem.setup(self, cpu_name="Intel(R) Core(TM) i7 CPU",
                                cpu_stepping=8)

class processor_core_i7_6c_2t(processor_core_i7):
    '''Intel® Core™ i7 processor with 6 cores and 2 threads.'''
    _class_desc = u'model of Intel® Core™ i7 6 cores 2 threads proc'
    _no_new_command = object()
    def _initialize(self, cpu_threads = 2, cpu_cores = 6):
        processor_core_i7._initialize(self, cpu_threads, cpu_cores)
        self.cpuclass = 'x86-nehalem'

    def setup(self):
        processor_nehalem.setup(self, cpu_name="Intel(R) Core(TM) i7 CPU",
                                cpu_stepping=8)

class processor_xeon_5500(processor_nehalem):
    '''Intel® Xeon® 5500 processor.'''
    _class_desc = u'model of Intel® Xeon® 5500 processor'
    _no_new_command = object()
    def _initialize(self, cpu_threads = 2, cpu_cores = 4):
        processor_nehalem._initialize(self, cpu_threads, cpu_cores)
        self.cpuclass = 'x86-nehalem-xeon'

    def setup(self):
        processor_nehalem.setup(self, cpu_name="Intel(R) Xeon(R) CPU",
                                cpu_stepping=5)

class processor_xeon_5530(processor_nehalem):
    '''Intel® Xeon® E5530 processor with 4 cores and 2 threads.'''
    _class_desc = u'model of Intel® Xeon® E5530 processor'
    _no_new_command = object()
    def _initialize(self, cpu_threads = 2, cpu_cores = 4):
        processor_nehalem._initialize(self, cpu_threads, cpu_cores)
        self.cpuclass = 'x86-nehalem-xeon'

    def setup(self):
        processor_nehalem.setup(
            self,
            cpu_name="Intel(R) Xeon(R) processor          E5530",
            cpu_stepping=5)

class processor_x86_intel64(processor_nehalem):
    '''Intel® Extendable 64-bit processor.'''
    _class_desc = u'model of Intel® Extendable 64-bit processor'
    _no_new_command = object()
    def _initialize(self, cpu_threads = 1, cpu_cores = 1):
        processor_nehalem._initialize(self, cpu_threads, cpu_cores)
        self.cpuclass = 'x86-intel64'

    def setup(self):
        processor_nehalem.setup(self, cpu_name="Intel(R) Intel64 CPU")
