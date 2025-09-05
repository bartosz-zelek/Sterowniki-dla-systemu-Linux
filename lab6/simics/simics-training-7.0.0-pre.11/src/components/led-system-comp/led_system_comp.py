# © 2018 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


##----------------------------------------------------------------------------------
## Component for Simics New User and Model Builder Training LED-based subsystem
##
## The component is built to connect over PCIe to as host computer
## It contains a complete subsystem, including an I2C bus with many devices on it
## It also provides a system panel to visualize the front end
##
## VERSION 2 - with customizations for model-builder training
## 
## VERSION 3 - replacing Simics Eclipse system panel with a graphics 
##             console 

from comp import(
    PciBusUpConnector,
    SimpleConfigAttribute,
    StandardConnectorComponent,
)

from comp import *

leds_high = 8
leds_wide = 8

#
class led_system_comp(StandardConnectorComponent):
    """PCIe card with a set of LEDs and buttons on its panel. Used for Simics training materials."""
    _class_desc = "training pcie card"
    _help_categories = ()

    def setup(self):
        StandardConnectorComponent.setup(self)
        if not self.instantiated.val:
            self.add_objects()
        self.add_connector('pci', PciBusUpConnector(0, 'master_bb'))

    #########################################################################################
    ## Configuration
    ##   All of these show up as parameters to the new-<component> and create-<component>
    ##   commands.  Automatically generated for you by the component system.
    ##---------------------------------------------------------------------------------------
    ## For example, tab-completing on new-led-system-comp:
    ##
    ## simics> new-led-system-comp 
    ##  clock_freq =  display_i2c_base =  master_i2c_address =  name =  pixel_update_time =  ram_size =  toggle_check_interval =
    ##
    #########################################################################################
    class ram_size(SimpleConfigAttribute(1024,"i")):
        """Size of memory in the controller, in kilobytes."""
        
    class display_i2c_base(SimpleConfigAttribute(16,"i")):
        """Hardware I2C address of the first LED in the display array. 
           Due to how the Simics system panel works, we cannot parameterize the physical
           size of the display - to change that, you have to rebuild the component."""

    class master_i2c_address(SimpleConfigAttribute(100,"i")):
        """Hardware I2C address of the master device on the I2C bus."""

    class clock_freq(SimpleConfigAttribute(100*1000*1000,"i")):
        """Clock frequency of the subsystem processor (Hertz)."""

    class pixel_update_time(SimpleConfigAttribute(20,"i")):
        """Delay between LED pixel draws, mirroring software overhead (cycles)."""

    class toggle_check_interval(SimpleConfigAttribute(1*1000*1000,"i")):
        """Interval at which to do toggle checks (cycles)."""

    
    #########################################################################################
    ##  Special argument to create variant classes for various labs
    ##  - String argument
    ##  - Used in add_objects() below to determine what to build.  
    #########################################################################################

    class lab_variant(SimpleConfigAttribute("","s")):
        """For model builder labs - which hardware lab variant to instantiate.  Using magic string keys to indicate how to configure the hardware. """

        
    #########################################################################################
    ## Add objects
    #########################################################################################
    
    def add_objects(self):
        #----------------------------------------------------------------------------------
        #   Set up the system panel in a graphics console 
        #----------------------------------------------------------------------------------
        self.add_component('panel', 'led_system_panel_comp',[])

        #----------------------------------------------------------------------------------
        #   Local RAM inside of device
        #----------------------------------------------------------------------------------
        # RAM
        ram_size_in_bytes = self.ram_size.val * 1024
        card_mem = self.add_pre_obj('local_memory', 'memory-space')

        # On-board RAM - put the image underneath the ram itself
        ram = self.add_pre_obj('ram', 'ram')
        ram_image = self.add_pre_obj('ram.image', 'image')
        ram_image.attr.size = ram_size_in_bytes
        ram.attr.image = ram_image   

        #----------------------------------------------------------------------------------
        #   I2C subsystem
        #----------------------------------------------------------------------------------
        
        # I2C endpoint ID, increment at each use of I2C to make sure IDs are unique
        endp_id  = 0xabcd0000  # Assigning IDs to i2c link endpoints, id 0 is not allowed

        # Address of first LED on the I2C bus
        display_i2c_base = self.display_i2c_base.val 
        
        # I2C link to connect with the LEDs
        # This is the way in Simics 5 for i2c_link_v2 
        i2c_bus = self.add_pre_obj('i2c_bus', 'i2c-link-impl')
        i2c_bus.goal_latency = 1e-8  ## 10 nano seconds
        
        ##
        ## The Black-box model of the master device
        ## - Including variants for lab
        master_bb_class = 'led_system_controller_bb'  ## default
        
        # TODO - apply to model builder training, maybe
        #if(self.lab_variant.val=="lab_t"):
        #    master_bb_class = 'led_system_controller_bb_lab_t'  ## LAB variant T            
        # end TODO 

        master_bb = self.add_pre_obj(
            'master_bb',
            master_bb_class,
            local_memory=card_mem,
            i2c_address=self.master_i2c_address.val)
        # i2c endpoint for master - put underneath the master object
        master_endpoint = self.add_pre_obj(
            'master_bb.i2c_ep',
            'i2c-link-endpoint',
            device=master_bb.port.i2c_in,
            link=i2c_bus,
            id=endp_id)
        endp_id += 1
        master_bb.attr.i2c_link = master_endpoint 
        master_bb.attr.fw_clock_frequency    = self.clock_freq.val
        master_bb.attr.pixel_update_time     = self.pixel_update_time.val
        master_bb.attr.toggle_check_interval = self.toggle_check_interval.val
                
        ##
        ## Create the toggle and button  I2C devices
        ##

        # Button A
        button_a = self.add_pre_obj(
            'button_a',
            'button_i2c',
            address=92,
            notify_address=self.master_i2c_address.val)
        button_a_endpoint = self.add_pre_obj(
            'button_a.i2c_ep',
            'i2c-link-endpoint',
            device=button_a.port.i2c_in,
            link=i2c_bus,
            id=endp_id)
        button_a.attr.i2c_link = button_a_endpoint
        endp_id += 1
          
        # Button B
        button_b = self.add_pre_obj(
            'button_b',
            'button_i2c',
            address=93,
            notify_address=self.master_i2c_address.val)
        button_b_endpoint = self.add_pre_obj(
            'button_b.i2c_ep',
            'i2c-link-endpoint',
            device=button_b.port.i2c_in,
            link=i2c_bus,
            id=endp_id)
        button_b.attr.i2c_link = button_b_endpoint 
        endp_id += 1
        
        # Toggle T
        toggle_t = self.add_pre_obj(
            'toggle_t',
            'toggle_i2c',
            address=90)
        toggle_t_endpoint=self.add_pre_obj(
            'toggle_t.i2c_ep',
            'i2c-link-endpoint',
            device=toggle_t.port.i2c_in,
            link=i2c_bus,
            id=endp_id)
        toggle_t.attr.i2c_link = toggle_t_endpoint 
        endp_id += 1

        # Toggle U
        toggle_u = self.add_pre_obj(
            'toggle_u',
            'toggle_i2c',
            address=91)
        toggle_u_endpoint = self.add_pre_obj(
            'toggle_u.i2c_ep',
            'i2c-link-endpoint',
            device=toggle_u.port.i2c_in,
            link=i2c_bus,
            id=endp_id)
        toggle_u.attr.i2c_link = toggle_u_endpoint 
        endp_id += 1

        # Set up the connections from the objects to the panel
        # Too fine-grained to work well as a component connector
        # in the current component system - see also the LED loop above
        (self.get_slot("panel.controls.button_a")).output = button_a.port.button_in
        (self.get_slot("panel.controls.button_b")).output = button_b.port.button_in
        (self.get_slot("panel.controls.toggle_t")).output = toggle_t.port.toggle_in
        (self.get_slot("panel.controls.toggle_u")).output = toggle_u.port.toggle_in

        ##        
        ## Create MxN RGB LED I2C devices
        ##
        # Loop to create all the LED devices, and connect to the panel LEDs 
        # - I2C address: row by row, from top to bottom
        # - Since I2c only has 7-bit addresses, display size like 16x16 is not possible
        # - I2C addresses 0b0000_xxx and 0b1111_xxx are reserved... so we need to work around that
        #
        # Note that we could model this as a 2D array of LED objects - which would make the model
        # map 1:1 to the display.  But that is not really appropriate, since from a network perspective
        # they are all just members of a linear I2C address sequence.  Thus, putting them in a 
        # single array makes more sense.  In principle, the LED arrangement on the panel is 
        # independent from how they are created here. They are just a set of LEDs with I2C objects,
        # and the mapping to the display is up to the software driving the LED controller.  Not 
        # a part of the Simics object hierarchy for the I2C bus. 
        # 
        dheight = leds_high
        dwidth = leds_wide
        ledcount = leds_wide * leds_high
        # Create an array of rgb led controller on i2c
        rgb_leds = self.add_pre_obj( "rgb_led[%d]" % ledcount, 'rgb_led_i2c')
        # Initialize all the led controllers
        for row in range(dheight):
            for column in range(dwidth):
                ledno = column + row * dwidth
                i2caddr = display_i2c_base + ledno
                rgb_leds[ledno].panel_led_out = self.get_slot(f"panel.led[{row}][{column}]")
                rgb_leds[ledno].address = i2caddr
                # i2c link endpoint handling - this is about handling Simics I2C links
                rgb_led_endpoint = self.add_pre_obj( f"rgb_led[{ledno}].i2c_ep", 'i2c-link-endpoint')
                # Set up attributes to make the endpoint and led controller find each other
                rgb_led_endpoint.device  = rgb_leds[ledno].port.i2c_in  # Using port object
                rgb_led_endpoint.link    = i2c_bus
                rgb_led_endpoint.id      = endp_id  
                rgb_leds[ledno].i2c_link = rgb_led_endpoint 
                # increment ID counters
                endp_id += 1


        #----------------------------------------------------------------------------------
        #   Set up the memory map
        #   - RAM mapped to low addresses
        #   - Master control registers mapped high 
        #----------------------------------------------------------------------------------
        card_mem.map = [[0x0000, ram, 0, 0, ram_size_in_bytes],
                        [0x10000000, master_bb.bank.regs, 0, 0, 0x1000],]
