/* This Software is part of Simics. The rights to copy, distribute,
   modify, or otherwise make use of this Software may be licensed only
   pursuant to the terms of an applicable license agreement.

   Copyright (C) 2010-2021 Intel Corporation */

/******************************************************************************
 *
 * Simics PCIe Demo Device Driver - for demos and educational purposes.
 *
 * Char-class driver in terms of interface to the system
 * Dynamic allocation of major device type   
 *
 * Using MSI-X interrupts to interrupt the processor
 *
 * Drives the "PCIe Demo Device"
 *
 * Written using Yocto 1.8/Linux kernel 3.14
 * Ported to Clear Linux with Kernel 5.0.4
 *
 *****************************************************************************/

//  

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

// IOCTL
#include "simics-pcie-demo-driver-ioctl.h"

// Define this symbol to do lots of "this function got called"
// Very useful for early debug of the driver flow.  It covers
// cases where we print on recurring calls, not on init or 
// rare calls like open and mmap. 
#define DRIVER_VERBOSE


// To create the version to use on disk images for initial use,
// with "later" functionality removed
#undef INITIAL_LIMITED_VERSION

//----------------------------------------------------------
// Device name
//   DEV_NAME    appears under /dev/ in the file system
//   CLASS_NAME  
//----------------------------------------------------------
static const char *DEV_NAME = KBUILD_MODNAME;
#define CLASS_NAME   "spcie"

//----------------------------------------------------------
// Device structure, statically allocated
// - i.e., we can only have one of those devices
//----------------------------------------------------------
#define MSIX_VECTORS  2

static struct {
  struct pci_dev *pdev;
  struct class*  charClass; ///< The device-driver class struct pointer
  struct device* charDevice; ///< The device-driver device struct pointer

  // Hardware interface
  void __iomem  *bar0_base_addr;

  // Char device identity towards Linux OS
  int  major;     // major device number
  int  openCount; // count # of open sessions
  
  // MSI-X interrupts
  struct msix_entry msix_entries[MSIX_VECTORS];

} simics_pcie_dev;

//----------------------------------------------------------
//  PCI device ID table
//----------------------------------------------------------
static const struct pci_device_id id_table[] = {
  // Use one of the demo device IDs assigned to Simics (0x0D61)
  // The device in the training setup is using the other (0x0D5F)
  { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x0D61) },
  { 0, },
};


//----------------------------------------------------------
// BAR0 memory map
//----------------------------------------------------------
#define BAR0_LED0_OFFSET             0x00
#define BAR0_LED1_OFFSET             0x04
#define BAR0_LED2_OFFSET             0x08
#define BAR0_LED3_OFFSET             0x0C
#define BAR0_BUTTON_STATUS_OFFSET    0x14
#define    BAR0_BUTTON_STATUS_BUTTON_1  0x01  // Bit masks for button status
#define    BAR0_BUTTON_STATUS_BUTTON_2  0x02
#define    BAR0_BUTTON_STATUS_BUTTON_3  0x04  // Hypothetical third button would have this mask
#define BAR0_BUTTON_1_COUNT_OFFSET   0x18
#define BAR0_BUTTON_2_COUNT_OFFSET   0x1C

#define BAR0_POWER_LED_OFFSET        0x20    // "power" LED, able to override HW
#define BAR0_ACTIVITY_LED_OFFSET     0x24    // "initialized" LED

#define BAR0_BUTTON_INTERRUPT_CONTROL  0x28
#define    BAR0_BUTTON_INTERRUPT_CONTROL_BUTTON_1  0x01
#define    BAR0_BUTTON_INTERRUPT_CONTROL_BUTTON_2  0x02

#define LED0  0
#define LED1  1
#define LED2  2
#define LED3  3
#define LEDO  4  // power/on/off
#define LEDA  5  // initialized/activity

//----------------------------------------------------------
// hardware interface
//
// led_no is:
//   0-3 for the main panel
//   4 for "on/off" aka "connected" aka "power"
//   5 for "activity" aka "initialized"
//
// brightness is:
//      0 off
//   != 0 on (i.e., 1)
//----------------------------------------------------------
void hw_update_led_state(int led_no, int brightness) {
	static uint32_t led_offsets[6] = {
           BAR0_LED0_OFFSET,
           BAR0_LED1_OFFSET,
           BAR0_LED2_OFFSET,
           BAR0_LED3_OFFSET,
           BAR0_POWER_LED_OFFSET,
           BAR0_ACTIVITY_LED_OFFSET
	};

	if( (led_no<0) || (led_no>5)) {
		pr_err(KBUILD_MODNAME
                       ": Internal driver error, bad led number: %d\n", led_no);
		return;
	}
	if (brightness != 0) {
		brightness = 1;
	}

	// Do the actual write
	iowrite32(brightness, (simics_pcie_dev.bar0_base_addr+led_offsets[led_no]) );
}


//----------------------------------------------------------
//
// CHAR DEVICE INTERFACE of the device
// - Allow open/close/read/write/mmap calls
// - Really useful for mmap
//
// - Interface:
//      write() strings to turn LEDs on off:
//        "nS" - n for the LED (0,1,2,3,a,o), S for state (0,1)
//      read() blocks until the button is pressed
//        i.e., needs to set up interrupts to work
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
// apropriate set of read and write calls.
//
//  - inodep A pointer to an inode object (defined in linux/fs.h)
//  - filep A pointer to a file object (defined in linux/fs.h)
//
static int chari_open(struct inode *inodep, struct file *filep){
	pr_info(KBUILD_MODNAME
	        ": chari_open called!\n");
	simics_pcie_dev.openCount++;
	return 0;
}

//----------------------------------------------------------
// Button press
//----------------------------------------------------------

static volatile int flag = 0;                //flag to block the thread
static volatile int lastButtonPressed = -1;  //the last button pressed
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

static irqreturn_t irq_handler(int irq, void *pdev) {

#if 0
	// printk in an IRQ handler is really bad, but for initial debug
	// of the flow it is acceptable.  But do not keep it in a production
	// driver as it will cost a lot of time!
    //
    // It is not even guarded with "verbose" as this is a truly bad place
    // to print
	pr_info(KBUILD_MODNAME
	        ": irq_handler called (irq=%d)\n", irq);
#endif

	//
	// Get the device status register value from the device, to see
	// which buttons were pressed to cause this interrupt
	//
	uint32_t statusReg = ioread32(simics_pcie_dev.bar0_base_addr+BAR0_BUTTON_STATUS_OFFSET);

	//
	// store the last pressed button in the shared variable so that the chari_read function
	// can return the appropriate value
	//
	if(statusReg & BAR0_BUTTON_STATUS_BUTTON_1) {
	    lastButtonPressed = 1;
	    // Clear the bit - just this bit, not the entire register
	    // in case another interrupt comes in for another register
	    iowrite32( BAR0_BUTTON_STATUS_BUTTON_1, simics_pcie_dev.bar0_base_addr + BAR0_BUTTON_STATUS_OFFSET);
	    //
	    // If you want a kernel crash for people to debug -- reverse the order of arguments to iowrite32.
	    // that will cause a bad write that the kernel will catch (as it accesses page zero, presumably)
	    // and show that the driver is doing bad stuff.  This is based on actual experience.
	    //
	}
    if(statusReg & BAR0_BUTTON_STATUS_BUTTON_2) {
        lastButtonPressed = 2;
        // Clear the bit - just this bit, not the entire register
        // in case another interrupt comes in for another register
        iowrite32( BAR0_BUTTON_STATUS_BUTTON_2, simics_pcie_dev.bar0_base_addr + BAR0_BUTTON_STATUS_OFFSET);
    }

#if 0
    // As said above, printing inside the IRQ handler is not a good idea
    // enable this only for quickly checking that things work
	pr_info(KBUILD_MODNAME
		        ": Button pressed; Status reg value: %d, reporting button: %d\n", statusReg, lastButtonPressed);
#endif

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
// Parameters:
// - filep  - A pointer to a file object
// - buffer - The pointer to the buffer to which this function writes the data
// - len    - The length of the buffer - we need at least 2 chars!
// - offset - The offset to read from, makes no sense for this device
//            We print it anyway to show what the call gets, but it is nothing
//            we can make much use of
//
static ssize_t chari_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {

    // Check input values
    if(len<2) {
        pr_err(KBUILD_MODNAME
               ": char read - too small buffer provided: len=%lu\n", len);
        return -EIO;
    }

    // clear the hardware register before doing a read, so that
    // we are sure to only see one button read
    iowrite32((uint32_t)0x3, simics_pcie_dev.bar0_base_addr+BAR0_BUTTON_STATUS_OFFSET);
    flag = 0;

    // Debug announce
#ifdef DRIVER_VERBOSE
	pr_info(KBUILD_MODNAME
	        ": chari_read called! To read at most %i chars at offset %i. Blocking until button pressed.", len, offset);
#endif
    
    //wait for lock
    wait_event_interruptible(queue, flag != 0);

    //write the last button pressed to the return buffer
    if(lastButtonPressed==1) {
        buffer[0] = '1';
    }
    if(lastButtonPressed==2) {
        buffer[0] = '2';
    }
    buffer[1] = '\0';

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
static ssize_t chari_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
	int led_no;
	int brightness;
	int charsleft;
	led_no = -1;
	brightness = -1;
	charsleft = (int)len;

#ifdef DRIVER_VERBOSE
     pr_info(KBUILD_MODNAME
	        ": chari_write called, with %d characters of input!\n", (int)len);
#endif

    // Parse loop, parse each pair of input characters
    while(charsleft >= 2) {
        // Which LED to operate on?
        switch(buffer[0]) {
	    case '0': led_no=0; break;
	    case '1': led_no=1; break;
	    case '2': led_no=2; break;
	    case '3': led_no=3; break;
	    case 'a': led_no=5; break;  // activity
	    case 'o': led_no=4; break;  // on/off
	    default:
                pr_err(KBUILD_MODNAME
                       ": char write - bad LED specified: %c\n", buffer[0]);
                       return -EIO;
        }

        // Which state to set?
        switch(buffer[1]) {
            case '0': brightness=0; break;
            case '1': brightness=1; break;
            default:
                pr_err(KBUILD_MODNAME
                       ": char write - bad brightness specified: %c\n", buffer[1]);
                       return -EIO;
        }

        // Update LEDs
        hw_update_led_state(led_no,brightness);

        // Update buffer to point to next pair of chars, count down the length
        charsleft -= 2;
        buffer    += 2;
    }
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
    return -ENOSYS;
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
#ifdef INITIAL_LIMITED_VERSION

static int chari_mmap(struct file *filp, struct vm_area_struct *vma){
    pr_info(KBUILD_MODNAME
            ": chari_mmap called - but we are just a dummy right now!\n");
    return -ENOSYS;
}

#else // full version:

void vm_open(struct vm_area_struct *vma) {
    pr_info(KBUILD_MODNAME
            ": (mmap) VMA Open. virtual: %lx, physical: %p\n", 
            vma->vm_start, 
            simics_pcie_dev.pdev->resource[0].start);
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
                    simics_pcie_dev.pdev->resource[0].start>>PAGE_SHIFT,
                    vma->vm_end - vma->vm_start,
                    vma->vm_page_prot);
    vma->vm_ops = &vm_ops;
    vm_open(vma);
    return 0;		
}
#endif // actual MMAP implementation

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
static int
simics_pcie_probe(struct pci_dev *pdev,
          	  	  const struct pci_device_id *id)
{
    int           err;
    void __iomem *bar0;
    int           i;

    struct pci_dev *dummy_limiting_dev;

    // Link speed and width
    enum pci_bus_speed speed = PCI_SPEED_UNKNOWN;
    enum pcie_link_width width = PCIE_LNK_WIDTH_UNKNOWN;

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
    simics_pcie_dev.pdev = pdev;

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
            // Making the assumption Linux enum will be built out seequentially
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
    simics_pcie_dev.bar0_base_addr = bar0;
    pr_info(KBUILD_MODNAME
            ": Access to BAR0 ready, mapped at virtual address 0x%p.\n", bar0);

    // Demonstrate to the user that we have access,
    // by turning on the "connected" LED on the device.
    hw_update_led_state(LEDA,1);

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
        simics_pcie_dev.msix_entries[i].entry = (i);
    }

    // Set up MSI-X interrupts - will write values to .vector
    // of all the msix_entries.
    // (this function will also enable interrupts)
    err = pci_enable_msix_exact(pdev,
    		simics_pcie_dev.msix_entries,
				MSIX_VECTORS);
    if (err != 0) {
        pr_err(KBUILD_MODNAME
                ": pci_enable_msix failed for %d vectors\n", MSIX_VECTORS);
        return err;
    }
    pr_info(KBUILD_MODNAME
            ": MSI-X interrupts enabled (pci_enable_msix, for %d interrupts).\n", MSIX_VECTORS);

    // Request the IRQs for the vectors we were given by the kernel
    err = request_irq(
            simics_pcie_dev.msix_entries[0].vector,
            irq_handler,
            0,
            DEV_NAME,     // as seen in /proc/interrupts
            NULL);        // this is just an ID sent to the interrupt handler
    if (err != 0) {
        pr_err(KBUILD_MODNAME
                ": request_irq failed for interrupt 0, vector=%d\n",
                simics_pcie_dev.msix_entries[0].vector);
        return err;
    }
    pr_info(KBUILD_MODNAME
            ": request_irq successful for interrupt 0, vector=%d\n",
            simics_pcie_dev.msix_entries[0].vector);

    // Request the IRQs for the vectors we were given by the kernel
    err = request_irq(
            simics_pcie_dev.msix_entries[1].vector,
            irq_handler,  // using the same handler for both inputs
            0,
            DEV_NAME,     // as seen in /proc/interrupts
            NULL);        // this is just an ID sent to the interrupt handler
    if (err != 0) {
        pr_err(KBUILD_MODNAME
                ": request_irq failed for interrupt 1, vector=%d\n",
                simics_pcie_dev.msix_entries[1].vector);
        return err;
    }
    pr_info(KBUILD_MODNAME
            ": request_irq successful for interrupt 1, vector=%d\n",
            simics_pcie_dev.msix_entries[1].vector);

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

	// Turn off initialized LED
	// -- Intentionally leave others as they are
	hw_update_led_state(LEDA,0);

	// Unmap IO
	iounmap(simics_pcie_dev.bar0_base_addr);

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

static int __init simics_pcie_module_init(void){
    int err;
    int majorNumber = 0;

    // Just to see if we loaded
    pr_info(KBUILD_MODNAME
            ": Device driver initialization started\n");

#ifdef INITIAL_LIMITED_VERSION
    pr_info(KBUILD_MODNAME
            ": Basic device driver version without mmap()\n");
#endif

    // Char device driver: register major number
    majorNumber = register_chrdev(0, DEV_NAME, &fops);
    if (majorNumber<0){
       pr_err(KBUILD_MODNAME
    		   	": failed to register a major number for char driver\n");
       return majorNumber;
    }
    pr_info(KBUILD_MODNAME
	        ": Registered correctly with major number for char driver = %d\n", majorNumber);
    simics_pcie_dev.major     = majorNumber;
    simics_pcie_dev.openCount = 0;

    //
    // Register as a device class so that we can be found
    //
    simics_pcie_dev.charClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(simics_pcie_dev.charClass)){
    	// Clean up
       unregister_chrdev(simics_pcie_dev.major, DEV_NAME);
       pr_err(KBUILD_MODNAME
    		   	": Failed to register device class\n");
       return PTR_ERR(simics_pcie_dev.charClass);          // Correct way to return an error on a pointer
    }
    pr_info(KBUILD_MODNAME
	        ": Device class '%s' registered correctly\n", CLASS_NAME);

    //
    // Register the device driver as a char device
    //
    simics_pcie_dev.charDevice = device_create(simics_pcie_dev.charClass,
                                               NULL,
                                               MKDEV(simics_pcie_dev.major, 0),
                                               NULL,
                                               DEV_NAME);
    if (IS_ERR(simics_pcie_dev.charDevice)){               // Clean up if there is an error
       class_unregister(simics_pcie_dev.charClass);
       class_destroy(simics_pcie_dev.charClass);
       unregister_chrdev(simics_pcie_dev.major, DEV_NAME);
       pr_err(KBUILD_MODNAME
    		   	": Failed to create the char device\n");
       return PTR_ERR(simics_pcie_dev.charDevice);
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
    	device_destroy(simics_pcie_dev.charClass, MKDEV(simics_pcie_dev.major, 0));     // remove the device
    	class_unregister(simics_pcie_dev.charClass);
        class_destroy(simics_pcie_dev.charClass);
        unregister_chrdev(simics_pcie_dev.major, DEV_NAME);
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
	class_unregister(simics_pcie_dev.charClass);
    class_destroy(simics_pcie_dev.charClass);
    unregister_chrdev(simics_pcie_dev.major, DEV_NAME);
    pci_unregister_driver(&simics_pcie_driver_pci);
	device_destroy(simics_pcie_dev.charClass, MKDEV(simics_pcie_dev.major, 0));     // remove the device
}


 
module_init(simics_pcie_module_init);
module_exit(simics_pcie_module_exit);

//----------------------------------------------------------
// Module metadata
//----------------------------------------------------------
MODULE_DESCRIPTION("Simics LED educational device PCIe device driver");
MODULE_AUTHOR("Jakob Engblom <jakob.engblom@intel.com>");
MODULE_LICENSE("GPL v2");
MODULE_DEVICE_TABLE(pci, id_table);


//----------------------------------------------------------
// Notes
//
// 1. 3.14 kernel vs old 2.6 PCI device driver function declarations:
//
//   Macros __devinit, __devinitdata, __devinitconst, __devexit, __devexitdata, __devexitconst
//   are no longer in use.  They were present in Daniel Aarno's old Simics.
//   PCIe device driver code, written for kernel 2.6.
//
// 2. How to properly access device memory
//
//   It appears that using iowrite32, ioread32 is the best way to access
//   the memory.  "readl" etc. is deprecated
//


