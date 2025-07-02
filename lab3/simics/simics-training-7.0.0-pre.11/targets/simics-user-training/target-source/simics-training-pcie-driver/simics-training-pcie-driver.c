/* This Software is part of Simics. The rights to copy, distribute,
   modify, or otherwise make use of this Software may be licensed only
   pursuant to the terms of an applicable license agreement.

   Copyright (C) 2010-2025 Intel Corporation */

/******************************************************************************
 *
 * Simics Training PCIe Device Driver - for New User training, debug training,
 * and eventually 
 *
 * Char-class driver in terms of interface to the system
 * Dynamic allocation of major device type   
 *
 * Using MSI-X interrupts to interrupt the processor
 *
 * 2019-Jan: Updated to work with the 4.20 kernel.
 *
 *****************************************************************************/

#include <linux/pci.h>
#include <linux/msi.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/keyboard.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/notifier.h>
#include <linux/delay.h>
#include <linux/mm.h>

// Define this symbol to do lots of "this function got called"
// Very useful for early debug of the driver flow.  It covers
// cases where we print on recurring calls, not on init or 
// rare calls like open and mmap. 
#define  DRIVER_VERBOSE


//----------------------------------------------------------
// Device name
//   DEV_NAME    appears under /dev/ in the file system
//   CLASS_NAME  
//----------------------------------------------------------
static const char *DEV_NAME = KBUILD_MODNAME;
#define CLASS_NAME   "sim_train"

//----------------------------------------------------------
// Device structure, statically allocated
// - i.e., we can only have one of those devices
//----------------------------------------------------------
#define MSIX_VECTORS  2

static struct  {
  struct pci_dev *pdev;
  struct class*  charClass; ///< The device-driver class struct pointer
  struct device* charDevice; ///< The device-driver device struct pointer

  // Hardware interface
  void __iomem  *bar0_base_addr;   // BAR0: control registers
  // bar 1 and 2 are dedicated to MSI-X interrupts
  void __iomem  *bar3_base_addr;   // BAR3: embedded memory

  // Char device identity towards Linux OS
  int  major;     // major device number
  int  openCount; // count # of open sessions
  
  // MSI-X interrupts
  struct msix_entry msix_entries[MSIX_VECTORS];

  // Runtime values
  uint32_t fb_base_offset;  // offset into the device memory for the framebuffer

} simics_training_pcie_dev;

//----------------------------------------------------------
//  PCI device ID table
//----------------------------------------------------------
static const struct pci_device_id id_table[] = {
  // Training device uses Device ID 0x0D5F in the Intel range
  //   This ID is assigned for Simics training devices by Intel,
  //   and will not interfere with any real-world PCI hardware
  { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x0D5F) },
  { 0, },
};


//----------------------------------------------------------
// BAR0 memory map
//----------------------------------------------------------
#define BAR0_ENABLE                           0x0000
#define BAR0_RESET                            0x0004

#define BAR0_UPDATE_DISPLAY_REQUEST           0x0010
#define BAR0_UPDATE_DISPLAY_STATUS            0x0014
#define BAR0_UPDATE_DISPLAY_INTERRUPT_CONTROL 0x0018
#define BAR0_FRAMEBUFFER_BASE_ADDRESS         0x001c

#define BAR0_ADV_COLOR_ALL                    0x0020
#define BAR0_ADV_DPROG_BASE                   0x0024
#define BAR0_ADV_DPROG_STATUS                 0x0028
#define BAR0_ADV_DPROG_CONTROL                0x002c

#define BAR0_BUTTON_INTERRUPT_CONTROL         0x0200
#define BAR0_BUTTON_STATUS_BASE               0x0204
#define BAR0_NUMBER_OF_BUTTONS                    16

#define BAR0_TOGGLE_STATUS_BASE               0x0244
#define BAR0_NUMBER_OF_BUTTONS                    16

#define BAR0_DISPLAY_I2C_BASE                 0x0800
#define BAR0_DISPLAY_WIDTH                    0x0804
#define BAR0_DISPLAY_HEIGHT                   0x0808
#define BAR0_BUTTON_I2C_ADDRESSES             0x0810
#define BAR0_TOGGLE_I2C_ADDRESSES             0x0850


// mmap:ing the device:
//
// We have to use /sys/bus/pci/... in order to mmap
// the embedded memory of the device.  One char device cannot
// expose more than one file.  I guess we could add one more
// char device to the driver for the same device...
//
//


//----------------------------------------------------------
//
// CHAR DEVICE INTERFACE of the device
// - Allow open/close/read/write/mmap calls
// - Really useful for mmap to expose the control registers
//
// - Interface:
//      write() interprets a string of coordinates and color
//              values and puts them into the framebuffer
//              memory
//      read() blocks until a button is pressed
//        i.e., needs to set up interrupts to work
//
//----------------------------------------------------------
// Char device file operation functions
//----------------------------------------------------------
static int     chari_open(struct inode *, struct file *);
static int     chari_release(struct inode *, struct file *);
static ssize_t chari_read(struct file *, char *, size_t, loff_t *);
static ssize_t chari_write(struct file *, const char *, size_t, loff_t *);
static int     chari_mmap(struct file *, struct vm_area_struct *);
static long    chari_ioctl(struct file *f, unsigned int cmd, unsigned long arg);

// The header in /linux/fs.h lists the callback functions available
//
// Char devices at least need open, release, read, write to be defined
static struct file_operations fops =
{
   .open             = chari_open,
   .read             = chari_read,
   .write            = chari_write,
   .release          = chari_release,
   .mmap             = chari_mmap,
   .unlocked_ioctl   = chari_ioctl
};

//----------------------------------------------------------
// Char device file operations
//----------------------------------------------------------

// The device open function that is called each time the device is opened
// This will only increment the counter of open calls.  There is nothing
// special that needs to be done, since opening the device does not affect
// its state or operation - for this particular device.
//
// For a char device with several minor numbers, you would use the open
// call to set the file operations pointer in the filep argument to the
// appropriate set of read and write calls.
//
//  - inodep A pointer to an inode object (defined in linux/fs.h)
//  - filep A pointer to a file object (defined in linux/fs.h)
//
static int chari_open(struct inode *inodep, struct file *filep){
	pr_info(KBUILD_MODNAME
	        ": chari_open called!\n");
	simics_training_pcie_dev.openCount++;
	return 0;
}

//----------------------------------------------------------
// Button press
//----------------------------------------------------------

static volatile int flag = 0;                //flag to block the thread
static DECLARE_WAIT_QUEUE_HEAD(queue);//locking queue to stop the calling thread execution. 

//----------------------------------------------------------
// Interrupt handler
//----------------------------------------------------------
// typedef irqreturn_t (*irq_handler_t)(int, void *);
//
// Return values:
//   IRQ_NONE            interrupt was not from this device or was not handled
//   IRQ_HANDLED         interrupt was handled by this device
//

static irqreturn_t irq_handler_button(int irq, void *pdev) {
    // An interrupt has come from the button vector
    // Wake up any waiting read operation
    flag = 1;
    wake_up_interruptible(&queue);

    // Return successful handling of the interrupt
    return IRQ_HANDLED;
}

//----------------------------------------------------------
// char driver read function
//----------------------------------------------------------
// This function is called whenever device is being read from user space i.e. data is
// being requested from the char device.  It will block until there is data to report,
// which is in our case means that the user has pressed a button on the device.
//
// Return value is the number of the button, from 0 to F -- hex value
//
// Parameters:
// - filep  - A pointer to a file object
// - buffer - The pointer to the buffer to which this function writes the data
// - len    - The length of the buffer - we need at least 2 chars!
// - offset - The offset to read from, makes no sense for this device
//            We print it anyway to show what the call gets, but it is nothing
//            we can make much use of
//
static ssize_t chari_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {

	int       btn;
    uint32_t  btnstatus;

    // Check input values
    if(len<2) {
        pr_err(KBUILD_MODNAME
               ": char read - too small buffer provided: len=%lu\n", len);
        return -EIO;
    }

    // Flag to coordinate with IRQ handler
    flag = 0;

    // Debug announce
#ifdef DRIVER_VERBOSE
	pr_info(KBUILD_MODNAME
	        ": chari_read called! To read at most %d chars at offset %p. Blocking until button pressed.",
			(uint32_t) len,
			offset);
#endif
    
    //wait for lock
    wait_event_interruptible(queue, flag != 0);

    // Detect the button that was pressed
    // and clear the status on that button
    for(btn=0; btn < BAR0_NUMBER_OF_BUTTONS; btn++) {
        // Read status register
        btnstatus = ioread32(simics_training_pcie_dev.bar0_base_addr+(BAR0_BUTTON_STATUS_BASE + 4*btn));

        if(btnstatus != 0) {
            // Clear the button status
            iowrite32(1, simics_training_pcie_dev.bar0_base_addr+(BAR0_BUTTON_STATUS_BASE + 4*btn));
            break;  // btn variable holds the number of the button that was pressed
        }
    }

    // Print as reply
    if(btn<=9) {
        buffer[0] = ('0' + btn);
    } else {
        // 10 and above, encode as hex
        buffer[0] = ('A' + btn - 10);
    }
    buffer[1] = '\0';

#ifdef DRIVER_VERBOSE
	pr_info(KBUILD_MODNAME
	        ": chari_read returning, saw button %d, returning '%s'.", btn, buffer);
#endif

    // Allow IRQ to unlock us again
    flag = 0;

    // return number of chars that we read
    return 2;
}



// This function is called whenever the device is being written to from user space.
//
// Parameters:
//  - filep  A pointer to a file object
//  - buffer The buffer to that contains the string to write to the device.  We should really
//           mistrust this data, as it comes from an untrusted user-space process.
//  - len    The length of the array of data that is being passed in the const char buffer
//  - offset The offset if required
//
// Basically,  take the data and poke it into the second memory space, starting at the
// programmed framebuffer offset.
//
// Consider input a series of bytes - RRGGBBAA RRGGBBAA ... two hex digits per byte
//

// helper - take two characters and interpret as hex digit
//          if the value is totally broken, return -1
//          if correct, please parse value to uint8_t for use
static int parse_hex_digit(const char c) {
	if(c>='0' && c<='9') return c-'0';
	if(c>='A' && c<='F') return c-'A' + 10;
	if(c>='a' && c<='f') return c-'a' + 10;
	return -1;
}

static int parse_hex_pair(const char *str) {
	int c0, c1;
	c0 = parse_hex_digit(str[0]);
	c1 = parse_hex_digit(str[1]);
	if( (c0<0) || (c1<0) ) return -1;
	return c0*16 + c1;
}

static ssize_t chari_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
	int                i=0;
	volatile uint8_t  *destptr;
	int                byteval;

#ifdef DRIVER_VERBOSE
     pr_info(KBUILD_MODNAME
	        ": chari_write called, with %d characters of input!\n", (int)len);
#endif

    // Make sure the driver and the hardware agree on where to
    // put the characters.  Code that does mmap() can and will change
    // the location of the buffer in the memory.
    iowrite32(  simics_training_pcie_dev.fb_base_offset ,
    		    simics_training_pcie_dev.bar0_base_addr + (BAR0_FRAMEBUFFER_BASE_ADDRESS) );

    // Loop over all the input, in pairs of chars and rather stupidly poke them
    // one byte at a time... which will be inefficient but this is just for
    // testing purposes
    destptr = (simics_training_pcie_dev.bar3_base_addr + simics_training_pcie_dev.fb_base_offset);
    while((len-i) >= 2) {
    	byteval = parse_hex_pair(buffer + i);
    	if(byteval==-1) {
    		// error!
            pr_err(KBUILD_MODNAME
                    ": char write - parsing error in input: %c%c\n", buffer[i], buffer[i+1]);
                    return -EIO;
    	}
    	*destptr = (uint8_t) byteval;
    	destptr++;            // next byte to write
    	i+=2;
    };

    // Tell the device to update contents
    iowrite32(1, simics_training_pcie_dev.bar0_base_addr + BAR0_UPDATE_DISPLAY_REQUEST);

    // Tell the system we did consume the input
    return len;
}

// The ioctl function is used to do controls that are device-specific
// and do not fit in the common file operations.  In our case, we use
// it to allow a user program to enable and disable device interrupts
static long chari_ioctl(struct file *f, unsigned int cmd, unsigned long arg) {
    pr_info(KBUILD_MODNAME
            ": chari_ioctl called (cmd=0x%x arg=0x%lx)!\n", cmd, arg);
    // do nothing
    return 0;
}


// The device release function that is called whenever the device is closed/released by
// the userspace program.  Does not need to do anything for this device, since opening it
// does not do anything either.
//
// - inodep A pointer to an inode object (defined in linux/fs.h)
// - filep A pointer to a file object (defined in linux/fs.h)
//
static int chari_release(struct inode *inodep, struct file *filep){
    pr_info(KBUILD_MODNAME
            ": chari_release called!\n");
    // No else needs to be done, no need for clean-up
    return 0;
}


//=========================================================================
//--------------------------- MMAP ----------------------------------------
//=========================================================================
#ifndef VM_RESERVED
#define VM_RESERVED (VM_DONTEXPAND | VM_DONTDUMP)
#endif

void vm_open(struct vm_area_struct *vma) {
    pr_info(KBUILD_MODNAME
            ": (mmap) VMA Open. virtual: 0x%lx, physical: 0x%llx\n",
            vma->vm_start, 
            simics_training_pcie_dev.pdev->resource[0].start);
}

void vm_close(struct vm_area_struct *vma) {
    pr_info(KBUILD_MODNAME ": (mmap) VMA closed.\n");
}

static struct vm_operations_struct vm_ops = {
    .open = vm_open,
    .close = vm_close,
};

static int chari_mmap(struct file *filp, struct vm_area_struct *vma){
    pr_info(KBUILD_MODNAME
            ": chari_mmap called!\n");
    remap_pfn_range(vma, vma->vm_start,
                    simics_training_pcie_dev.pdev->resource[0].start>>PAGE_SHIFT,
                    vma->vm_end - vma->vm_start,
                    vma->vm_page_prot);
    vma->vm_ops = &vm_ops;
    vm_open(vma);
    return 0;		
}


//=========================================================================
//---------------------------- end MMAP -----------------------------------
//=========================================================================


//----------------------------------------------------------
// PCI Init and Exit
//----------------------------------------------------------

// 
// Probe function.
//
// Called after the module is initialized, when there is a device in the hardware
// matching the IDs that we declared.  Or if the module is already loaded into 
// the kernel, and a device with a matching ID shows up.  This function is
// called as a result of the module init function calling pci_register_driver.
//
// If it not called, we have a problem with the kernel detecting our device.
//
// For the steps inside this driver, see
//  <linux>/Documentation/PCI/pci.txt
//
// Note that we should NOT map the BARs for the MSI-X table and PBA -- they are
// managed by the Linux kernel.  To enable or disable specific MSI-X interrupts,
// call the kernel enable_irq/disable_irq functions with the vector numbers from 
// the MSI-X registration
// 
#define DEFAULT_FRAMEBUFFER_OFFSET        0x1000

static int
simics_pcie_probe(struct pci_dev *pdev,
          	  	  const struct pci_device_id *id)
{
    int           err;
    void __iomem *bar0, *bar3;
    int           i;

    // Link speed and width
    enum pci_bus_speed speed = PCI_SPEED_UNKNOWN;
    enum pcie_link_width width = PCIE_LNK_WIDTH_UNKNOWN;
    struct pci_dev *dummy_limiting_dev;
    
    // For debug and understanding the flow, printk that we are in the
    // pcie probe function
    pr_info(KBUILD_MODNAME
            ": PCIe probe function called.\n");

    // Enable the device, must be done first before anything else
    // is done with the device.
    //
    // Since our device does not use IO BARs, we use the _mem version
    //
    err = pci_enable_device_mem(pdev);
    if(err) {
        pr_err(KBUILD_MODNAME
                ": Failed to enable PCIe device: %d\n", err);
        return err;
    }
    pr_info(KBUILD_MODNAME
            ": PCIe device enable successful.\n");

    // Enable DMA from this device, since it is necessary
    // to be able to send MSI-X interrupts.  THis would also be
    // used if the device did do normal DMA.
    //
    // It is needed for MSI-X since MSI-X interrupts are actually
    // memory operations send by the device hardware.
    pci_set_master(pdev);
    pr_info(KBUILD_MODNAME
            ": PCI bus mastering enabled.\n");

    // Get access to the PCI regions, which ensures we are
    // the only driver for this device (if I understand the kernel
    // documentation correctly)
    //
    //  To clean up, use pci_release_regions(pdev)
    //
    err = pci_request_regions(pdev, DEV_NAME);
    if (err) {
        pr_err(KBUILD_MODNAME
                ": Failed to request regions: %d\n", err);
        return err;
    }
    pr_info(KBUILD_MODNAME
            ": PCIe device requested and obtained regions.\n");

    // Save the device pointer into our global data structure
    simics_training_pcie_dev.pdev = pdev;

    // Diagnose the nature of our connection to the rest of the
    // system - bus width and speed
    //
    // enum width: actual numeric value of width, easy
    // enum speed: complex encoding
    //
    err = pcie_bandwidth_available(pdev, &dummy_limiting_dev, &speed, &width);
    if ( err != 0 ||
            speed == PCI_SPEED_UNKNOWN ||
            width == PCIE_LNK_WIDTH_UNKNOWN) {
        pr_warn(KBUILD_MODNAME
                ": Failed to request determine link speed and width! (%d) \n", err);
    } else {
        char *pciegen = "unknown";
        switch (speed){
        case PCIE_SPEED_8_0GT:
            pciegen = "3.0";
            break;
        case PCIE_SPEED_5_0GT:
            pciegen = "2.0";
            break;
        case PCIE_SPEED_2_5GT:
            pciegen = "1.0";
            break;
        default:
            // Making the assumption Linux enum will be built out sequentially
            if(speed > PCIE_SPEED_8_0GT) {
                pciegen = "beyond 3";
            } else {
                // I would assume we would never get this answer asking for PCIe
                // connection speeds, but better to program defensively
                pciegen = "PCI";
            }
      }
      pr_info(KBUILD_MODNAME
              ": PCIe link properties: gen=%s, width=%d, speed=%d.\n",
              pciegen, width, speed);
    }

    // Remap BAR0, where our registers live, so that we can start
    // accessing the device.  If we used more BARs, we would map
    // them here too.  Note that we should NOT map the BARs used
    // by the MSI-X interrupt tables, as they are managed by the
    // kernel itself automatically.
    //
    //   To clean up, use iounmap()
    //
    bar0 = pci_ioremap_bar(pdev, 0);
    if (!bar0) {
        err = -ENOMEM;
        pr_err(KBUILD_MODNAME
                ": Failed to remap BAR0, reporting ENOMEM\n");
        return err;
    }
    simics_training_pcie_dev.bar0_base_addr = bar0;
    pr_info(KBUILD_MODNAME
            ": Access to BAR0 ready, mapped at 0x%p.\n", bar0);

    // Remap BAR3, where the subsystem memory is mapped
    //
    //   To clean up, use iounmap()
    //
    bar3 = pci_ioremap_bar(pdev, 3);
    if (!bar3) {
        err = -ENOMEM;
        pr_err(KBUILD_MODNAME
                ": Failed to remap BAR3, reporting ENOMEM\n");
        return err;
    }
    simics_training_pcie_dev.bar3_base_addr = bar3;
    pr_info(KBUILD_MODNAME
            ": Access to BAR3 ready, mapped at 0x%p.\n", bar3);


    // Enable interrupts to be sent
    // Set up entry numbers for our vectors, just straight numeric mapping
    // - So we have two entries that our driver numbers 0 and 1
    // - It appears that the entries must be numbered from 0 to make Linux
    //   happy -- numbering from 1 at offset 0 made the MSI-X request fail!
    //
    // The linux will allocate vectors for us and fill in the vector table
    // in the hardware msix_table register bank.
    //
    for(i=0;i<MSIX_VECTORS;i++) {
        // kernel will fill in the 16-bit 'vector' entry as a
        // result of calling pci_enable_msix
        simics_training_pcie_dev.msix_entries[i].entry = (i);
    }
    // Set up MSI-X interrupts - will write values to .vector
    // of all the msix_entries.
    // (this function will also enable interrupts)
    err = pci_enable_msix_exact(pdev,
				simics_training_pcie_dev.msix_entries,
				MSIX_VECTORS);
    if (err != 0) {
        pr_err(KBUILD_MODNAME
                ": pci_enable_msix failed for %d vectors\n", MSIX_VECTORS);
        return err;
    }
    pr_info(KBUILD_MODNAME
            ": MSI-X interrupts enabled (pci_enable_msix, for %d interrupts).\n", MSIX_VECTORS);

    // Request the IRQs for the vectors we were given by the kernel
    // the first (number 0) IRQ is for the button interrupt handler
    err = request_irq(
            simics_training_pcie_dev.msix_entries[0].vector,
            irq_handler_button,
            0,
            DEV_NAME,     // as seen in /proc/interrupts
            NULL);        // this is just an ID sent to the interrupt handler
    if (err != 0) {
        pr_err(KBUILD_MODNAME
                ": request_irq failed for interrupt 0, vector=%d\n",
                simics_training_pcie_dev.msix_entries[0].vector);
        return err;
    }
    pr_info(KBUILD_MODNAME
            ": request_irq successful for interrupt 0, vector=%d\n",
            simics_training_pcie_dev.msix_entries[0].vector);
    // Actually enable interrupts in the device
    iowrite32(1, simics_training_pcie_dev.bar0_base_addr + BAR0_BUTTON_INTERRUPT_CONTROL);

    // TODO - build IRQ handler for second IRQ, for redraw complete

    // Configure the device configuration registers with values
    // that are known to be good for the hardware configuration
    iowrite32( 92 , simics_training_pcie_dev.bar0_base_addr + (BAR0_BUTTON_I2C_ADDRESSES + 0) );
    iowrite32( 93 , simics_training_pcie_dev.bar0_base_addr + (BAR0_BUTTON_I2C_ADDRESSES + 4) );
    iowrite32( 90 , simics_training_pcie_dev.bar0_base_addr + (BAR0_TOGGLE_I2C_ADDRESSES + 0) );
    iowrite32( 91 , simics_training_pcie_dev.bar0_base_addr + (BAR0_TOGGLE_I2C_ADDRESSES + 4) );
    iowrite32( 16 , simics_training_pcie_dev.bar0_base_addr + (BAR0_DISPLAY_I2C_BASE) );
    iowrite32(  8 , simics_training_pcie_dev.bar0_base_addr + (BAR0_DISPLAY_WIDTH) );
    iowrite32(  8 , simics_training_pcie_dev.bar0_base_addr + (BAR0_DISPLAY_HEIGHT) );

    simics_training_pcie_dev.fb_base_offset = DEFAULT_FRAMEBUFFER_OFFSET;
    iowrite32(  simics_training_pcie_dev.fb_base_offset ,
    		    simics_training_pcie_dev.bar0_base_addr + (BAR0_FRAMEBUFFER_BASE_ADDRESS) );


    // Enable the device
    iowrite32(  1 , simics_training_pcie_dev.bar0_base_addr + (BAR0_ENABLE) );

    pr_info(KBUILD_MODNAME
            ": PCIe probe function returned successfully!\n");

    return 0;
}

//----------------------------------------------------------
// remove function:
// -- Called when the PCIe device is getting released by the
//    kernel, like after a rmmod call
//----------------------------------------------------------
static void
simics_pcie_remove(struct pci_dev *pdev)
{
	pr_info(KBUILD_MODNAME
	        ": PCIe exit function called.\n");

	// Unmap IO
	iounmap(simics_training_pcie_dev.bar0_base_addr);
	iounmap(simics_training_pcie_dev.bar3_base_addr);

	// Release regions
	pci_release_regions(pdev);

	// Disable the device
	pci_disable_device(pdev);
}


//----------------------------------------------------------
// PCI Driver structure
// -- Passed to the kernel at module init time to give it
//    the probe and remove functions
//----------------------------------------------------------
static struct pci_driver simics_pcie_driver_pci = {
  .name     = KBUILD_MODNAME,
  .id_table = id_table,
  .probe    = simics_pcie_probe,
  .remove   = simics_pcie_remove
};


//----------------------------------------------------------
// Module init & exit
// -- This is the general kernel init and exit code, common
//    to all types of devices.  It sets up the module as a
//    PCIe and char device, so that the kernel can call the
//    other device functions we have above.
//----------------------------------------------------------
static int __init simics_pcie_module_init(void) {
	int err;
    int majorNumber = 0;

	// Just to see if we loaded
	pr_info(KBUILD_MODNAME
	        ": Device driver initialization started\n");

    // Char device driver: register major number
    majorNumber = register_chrdev(0, DEV_NAME, &fops);
    if (majorNumber<0){
       pr_err(KBUILD_MODNAME
    		   	": failed to register a major number for char driver\n");
       return majorNumber;
    }
    pr_info(KBUILD_MODNAME
	        ": Registered correctly with major number for char driver = %d\n", majorNumber);
    simics_training_pcie_dev.major     = majorNumber;
    simics_training_pcie_dev.openCount = 0;

    //
    // Register as a device class so that we can be found
    //
    simics_training_pcie_dev.charClass = class_create(CLASS_NAME);
    if (IS_ERR(simics_training_pcie_dev.charClass)){
    	// Clean up
       unregister_chrdev(simics_training_pcie_dev.major, DEV_NAME);
       pr_err(KBUILD_MODNAME
    		   	": Failed to register device class\n");
       return PTR_ERR(simics_training_pcie_dev.charClass);          // Correct way to return an error on a pointer
    }
    pr_info(KBUILD_MODNAME
	        ": Device class '%s' registered correctly\n", CLASS_NAME);

    //
    // Register the device driver as a char device
    //
    simics_training_pcie_dev.charDevice = device_create(simics_training_pcie_dev.charClass,
                                               NULL,
                                               MKDEV(simics_training_pcie_dev.major, 0),
                                               NULL,
                                               DEV_NAME);
    if (IS_ERR(simics_training_pcie_dev.charDevice)){               // Clean up if there is an error
       class_unregister(simics_training_pcie_dev.charClass);
       class_destroy(simics_training_pcie_dev.charClass);
       unregister_chrdev(simics_training_pcie_dev.major, DEV_NAME);
       pr_err(KBUILD_MODNAME
    		   	": Failed to create the char device\n");
       return PTR_ERR(simics_training_pcie_dev.charDevice);
    }
    pr_info(KBUILD_MODNAME
	        ": Char device driver registration successful\n");

    //
    // Register as PCI device
    //
    pr_info(KBUILD_MODNAME
	        ": PCI device driver registration started\n");
    err = pci_register_driver(&simics_pcie_driver_pci);
    if (err) {
    	device_destroy(simics_training_pcie_dev.charClass, MKDEV(simics_training_pcie_dev.major, 0));     // remove the device
    	class_unregister(simics_training_pcie_dev.charClass);
        class_destroy(simics_training_pcie_dev.charClass);
        unregister_chrdev(simics_training_pcie_dev.major, DEV_NAME);
        pr_err(KBUILD_MODNAME
        		": PCI device driver registration failed for %s\n",
        		DEV_NAME);
        return err;
    }
    pr_info(KBUILD_MODNAME
	        ": PCI device driver registration successful\n");


    //
    // Driver init completed and successful. Do a kernel printout 
    // that can be used to test for this success.  
    //
    // Change this message to see that you have managed to update
    // the driver code.  
    //
    pr_info(KBUILD_MODNAME
	        ": Device driver initialization successful\n");
    // Return success
    return 0;
}

static void __exit simics_pcie_module_exit(void){
	pr_info(KBUILD_MODNAME 
	        ": Goodbye and good night!\n");

	// Clean up all the things we allocated
	device_destroy(simics_training_pcie_dev.charClass, MKDEV(simics_training_pcie_dev.major, 0));     // remove the device
//	pci_unregister_driver(&simics_pcie_driver_pci);
	class_unregister(simics_training_pcie_dev.charClass);
        class_destroy(simics_training_pcie_dev.charClass);
        unregister_chrdev(simics_training_pcie_dev.major, DEV_NAME);
}
 
module_init(simics_pcie_module_init);
module_exit(simics_pcie_module_exit);

//----------------------------------------------------------
// Module metadata
//----------------------------------------------------------
MODULE_DESCRIPTION("Simics Training Device Driver");
MODULE_AUTHOR("Jakob Engblom <jakob.engblom@intel.com>");
MODULE_LICENSE("GPL v2");
MODULE_DEVICE_TABLE(pci, id_table);



