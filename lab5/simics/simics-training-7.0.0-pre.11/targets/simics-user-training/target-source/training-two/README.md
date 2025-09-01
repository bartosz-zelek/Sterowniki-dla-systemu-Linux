# training-two
Demo program one for use in Simics training, in particular for debugging
labs.  It activates the panel via mmap-IO to both the internal RAM and 
device register bank.  It features a more complex input format for an 
image to display, in order to get a simple interpreter in there.  

## Building
Should build on most Linuxes with a plain make. 

## Pedagogical take
Interpreters are, as we know, riddled with bugs in general.  The 
question is, is this one too?  Indeed it is, and such a bug can 
be triggered with an input like:

'''
./training-two RGBWCMYK /sys/bus/pci/devices/0000\:02\:00.0
'''


