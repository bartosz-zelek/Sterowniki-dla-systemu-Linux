# © 2021 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


## Component for Simics Mandelbrot Accelerator workshop  
##
## Creates the whole subsystem from a single command
## - control, compute units, and a DML display unit
##  
## Can be configured with/out clock and pcie
## - For stand-alone use, set use_clock to True, and
##   use_pcie to False. This is the default. 
##
## - For use with the QSP, set use_clock to False 
##   and use_pcie to True.  It will work to have a 
##   clock when connected to the QSP, it is just 
##   redundant. 
##

from comp import(
    Attribute,
    PciBusUpConnector,
    SimpleConfigAttribute,
    StandardComponent,
    StandardConnectorComponent,
)
import connectors
import simics

class m_accelerator_comp(StandardConnectorComponent):
    """Component for the mandelbrot accelerator subsystem."""
    _class_desc = "mandelbrot accelerator component"
    _help_categories = ()

    class basename(StandardConnectorComponent.basename):
        ## If no name is given when creating a
        ## new component, this will be used to generate
        ## a name. 
        val = "mandel"

    ## Entry point when creating an instance of the component
    def setup(self):
        super().setup()
        ## Create all the objects 
        if not self.instantiated.val:
            self.add_objects()
        ## Add connector to the graphics console
        ## - Note that this uses a special pseudo attribute in the device
        ##   Since we want to not just change the attribute value, but also
        ##   force a redraw in case we are doing a hot-plug operation 
        self.add_connector(
            'console',
            connectors.GfxDownConnector('display', 'connect_console'))
        ## Add connector to PCIe, if requested
        if self.use_pcie.val == True:
            ## Add PCIe connector
            self.add_connector('pci', PciBusUpConnector(0, 'control'))

    ## Configuration options for the component
    class compute_units(SimpleConfigAttribute(2, "i")):
        """Number of compute units in this accelerator instance."""
        def setter(self,v):
            ## Check here has to be consistent with the compiled-in 
            ## limit in control.dml... not ideal, but that is how it works
            if v>8:  
                return simics.Sim_Set_Illegal_Value
            self.val = v

    # Allow configuration of the compute unit class, for use with the 
    # updated m_compute module that compiles out all variants into different
    # classes for testing. 
    class compute_unit_class(SimpleConfigAttribute("m_compute", "s")):
        """Class of the compute units, for testing (m_compute[_dummy][_threaded])."""
        def setter(self,v):
            if not v in ["m_compute","m_compute_dummy","m_compute_threaded","m_compute_dummy_threaded"]:  
                return simics.Sim_Set_Illegal_Value
            self.val = v

    class use_pcie(SimpleConfigAttribute(False, 'b')):
        """If true, the component will provide a PCIe interface. Defaults to false."""

    class use_clock(SimpleConfigAttribute(True, 'b')):
        """If true, the component will instantiate its own clock. Defaults to true."""

    class ram_size(SimpleConfigAttribute(64 * 1024 * 1024, "i")):
        """Size of memory in the accelerator (bytes).  More than 64MiB will not be mappable by the device."""
        def setter(self,v):
            ## Check here has to be consistent with the compiled-in 
            ## limit in control.dml... not ideal, but that is it works
            if v>64 * 1024 * 1024:  
                return simics.Sim_Set_Illegal_Value
            self.val = v

    class pixel_compute_time(SimpleConfigAttribute(10000, "i")):
        """Time to compute one pixel (ps)."""

    ## For UI presentation purposes - computes reply to UI
    class memory_megs(Attribute):
        '''Total memory in MB.'''
        attrtype = 'i'
        attrattr = simics.Sim_Attr_Pseudo
        def getter(self):
            return self._up.ram_size.val >> 20

    ## Icon, used if the component is considered "top_level"
    class system_icon(StandardComponent.system_icon):
        val = "mandel-chip-icon.png"

    ## Add the objects that live inside the component.
    ## Note that being in a namespace is implicit in a component
    ##   = there is no namespace object created by this code
    def add_objects(self):
        # Local clock, if wanted 
        if self.use_clock.val==True:
            self.add_pre_obj('clock', 'clock', freq_mhz = 10)
            self.top_level.val = True
        
        # Memory maps
        register_memory = self.add_pre_obj('register_memory', 'memory-space')
        register_memory.attr.map = []
        local_memory = self.add_pre_obj('local_memory', 'memory-space')

        # Memory 
        ram = self.add_pre_obj('ram', 'ram')
        ram_image = self.add_pre_obj('ram.image', 'image')
        ram_image.attr.size = self.ram_size.val 
        ram.attr.image = ram_image  
        local_memory.attr.map = [[0x0000, ram, 0, 0, self.ram_size.val]]
       
        # Control unit 
        # - Note that the "queue" attribute is now automatically set 
        #   by the component framework itself. 
        control = self.add_pre_obj(
            'control',
            'm_control',
            connected_compute_unit_count=self.compute_units.val,
            local_memory=local_memory,
            register_memory=register_memory,
            compute_unit_control=[])
        register_memory.attr.map.append([0, control.bank.ctrl, 0, 0, 0x80])
       
        # Compute units
        compute_units = self.add_pre_obj(
            f"compute[{self.compute_units.val}]", 
            self.compute_unit_class.val,
            local_memory=local_memory,
            pixel_compute_time=self.pixel_compute_time.val )

        # Add compute units, map them, point at controller
        register_bank_address_0 = 0x80
        for i in range(self.compute_units.val):
            # Tell the control unit about this compute unit
            # And vice versa
            compute_units[i].attr.operation_done = control.port.done[i]
            control.attr.compute_unit_control.append(compute_units[i].port.control_in)

            register_memory.attr.map.append([register_bank_address_0 + i * 0x40, 
                                             compute_units[i].bank.ctrl, 
                                             0, 0, 0x10])

        # Display unit - map last in memory map, after the "real"
        # units. Since this is a simulation-only trick that would not
        # be used in actual hardware most likely. 
        display = self.add_pre_obj(
            'display',
            'm_display',
            local_memory=local_memory)
        register_memory.attr.map.append([0x300, display.bank.regs, 0, 0, 0x100])
