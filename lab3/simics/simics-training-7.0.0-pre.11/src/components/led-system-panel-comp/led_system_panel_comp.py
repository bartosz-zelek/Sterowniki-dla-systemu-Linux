# © 2022 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

from comp import (StandardConnectorComponent, PciBusUpConnector,
                  ConfigAttribute, SimpleConfigAttribute)

import target_info  ## To use the component system way to find images

## Look up a PNG file
## Using the system also used by target info icons, 
## as that is the best we have currently in Simics for
## module resources like this.  
def getpng(filename):
    full_path = target_info.find_image(filename)
    # Error checking, should never happen in a user installation 
    if not full_path:
        raise ValueError(f"File {filename} could not be found (component totally broken)")
    return full_path

## Component defining the system panel displayed in a graphics console
class led_system_panel_comp(StandardConnectorComponent):
    """LED system panel used in training, displayed in a graphics console."""
    _class_desc = "training LED panel"
    _help_categories = ()

    class basename(StandardConnectorComponent.basename):
        ## If no name is given when creating a
        ## new component, this will be used to generate
        ## a name. 
        val = "training_led_panel"

    def setup(self):
        super().setup()
        if not self.instantiated.val:
            self.add_objects()

    class component(StandardConnectorComponent.component):
        def post_instantiate(self):
            super().post_instantiate()
            # Draw the initial state of the panel
            # Which includes the layering of images
            # Good enough solution for now.  
            # A robust solution would require building a
            # specific compositor class that holds this
            # information, which is too much work for a few
            # simple images stacked.
            # 
            bgimg = self.get_slot_value("background.board")
            bgimg.iface.uint64_state.set(0)  # causes drawing

            ledbgimg = self.get_slot_value("background.led_panel")
            ledbgimg.iface.uint64_state.set(0)  # causes drawing

            for a in range(8):
                for b in range(8):
                    ## Each LED in the row
                    l = self.get_slot_value(f"led[{a}][{b}]")                    
                    l.iface.uint64_state.set((a+b)%8)  # default state is cool

            # Getting attribute value of the controls object 
            c = self.get_slot_value("controls")
            cs = c.controls  # attribute controls in object controls. 
            for o in cs:
                o.iface.p_control_button.initial_state()
            

    def add_objects(self):
        # Local clock, if wanted 
        if(self.use_clock.val==True):
            self.add_pre_obj('clock', 'clock', freq_mhz = 10)
            self.top_level.val = True
        
        # The graphics console to display the panel in  
        rec = self.add_pre_obj('rec', 'recorder')   
        gcon = self.add_pre_obj(
            'con',
            'graphcon',
            window_title="Training System LED Panel Interactive",
            recorder=rec)

        # The display driver object
        display = self.add_pre_obj(
            'display_driver',
            'p_display',
            height=500,
            width=1000,
            console=gcon)
        gcon.attr.device=display

        # Background image
        self.add_pre_obj('background', 'namespace')
        self.add_pre_obj(
            'background.board',
            'p_output_image',
            draw=display.port.draw,
            x=0,
            y=0,
            images=[getpng("bg-board-1000x500.png")])

        # Background for the LEDs
        led_top = 90  
        led_left = 290
        led_offset = 28
        led_width = 33   # One pixel margin between
        led_height = 33

        self.add_pre_obj(
            'background.led_panel',
            'p_output_image',
            draw=display.port.draw,
            x=led_left,
            y=led_top,
            images=[getpng("led-background.png")])

        # All the LEDs
        led_images = [getpng("led-black.png"),   # 0 = black
                      getpng("led-blue.png"),    # 1 = blue
                      getpng("led-red.png"),     # 2 = red
                      getpng("led-magenta.png"), # 3 = magenta
                      getpng("led-green.png"),   # 4 = green
                      getpng("led-cyan.png"),    # 5 = cyan
                      getpng("led-yellow.png"),  # 6 = yellow
                      getpng("led-white.png"),    # 7= white
                    ]
        leds = self.add_pre_obj(
            "led[8][8]",
            'p_output_image',
            draw=display.port.draw,
            images=led_images)
        # Loop and set x and y 
        for a in range(8):
            for b in range(8):
                ## Each LED in the row
                leds[a][b].attr.x = led_left + led_offset + led_width * b
                leds[a][b].attr.y = led_top + led_offset + led_height * a

        # Controls inside a namespace -
        # and the controller for the controls is used
        # to create the namespace object
        controls = self.add_pre_obj('controls', 'p_control')
        controls.attr.display = display.port.draw
        gcon.attr.abs_pointer_enabled = True
        gcon.attr.abs_mouse = controls.port.pointer

        # Button A
        btn_a = self.add_pre_obj('controls.button_a', 'p_button')
        btn_a_img = self.add_pre_obj(
            'controls.button_a.img',
            'p_output_image',
            x=718,
            y=208,
            draw=display.port.draw,
            images=[getpng("button-A.png"),
                    getpng("button-A-highlight.png")])
        btn_a.attr.output_image = btn_a_img

        # Button B
        btn_b = self.add_pre_obj('controls.button_b', 'p_button')
        btn_b_img = self.add_pre_obj(
            'controls.button_b.img',
            'p_output_image',
            x=840,
            y=208,
            draw=display.port.draw,
            images=[getpng("button-B.png"),
                    getpng("button-B-highlight.png")])
        btn_b.attr.output_image = btn_b_img

        # Toggle 1 
        tog_t = self.add_pre_obj(
            'controls.toggle_t',
            'p_toggle',
            toggle_state=False) 
        tog_t_img = self.add_pre_obj(
            'controls.toggle_t.img','p_output_image',
            x=852,
            y=336,
            draw = display.port.draw,
            images=[getpng("toggle-0.png"),
                    getpng("toggle-change.png"),
                    getpng("toggle-1.png")])
        tog_t.attr.output_image = tog_t_img

        tog_u=self.add_pre_obj(
            'controls.toggle_u',
            'p_toggle', 
            toggle_state=False) 
        tog_u_img = self.add_pre_obj(
            'controls.toggle_u.img',
            'p_output_image',
            x=852+28,
            y=336,
            draw=display.port.draw,
            images=[getpng("toggle-0.png"),
                    getpng("toggle-change.png"),
                    getpng("toggle-1.png")])
        tog_u.attr.output_image = tog_u_img

        #Provide the control manager with all the controls that can be clicked
        controls.attr.controls = [btn_a.port.control,
                                  btn_b.port.control,
                                  tog_t.port.control,
                                  tog_u.port.control]
        
    # Configuration
    class use_clock(SimpleConfigAttribute(False, 'b')):
        """If true, the component will instantiate its own clock. 
           That allows it to be used on its own.  Defaults to false."""


