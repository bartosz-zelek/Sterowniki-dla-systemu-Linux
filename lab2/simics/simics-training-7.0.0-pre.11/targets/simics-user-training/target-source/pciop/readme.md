# pciop 
This program implements access to a PCIe device's BARs via /sys.
It is a simple program to test that such accesses work, and to 
help debug and explore PCIe devices.   

It has only been tested on Simics, even though in practice it
ought to work on real hardware too.  

## Usage
On the QSP setups with the training card on pci bus 2, it is invoked something like this:

'''
 ./pciop /sys/bus/pci/devices/0000\:02\:00.0/resource0 r 4 0x04
'''


