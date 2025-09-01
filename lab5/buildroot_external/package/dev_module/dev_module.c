
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/io.h>

/* Simplified driver: always map Simics device at 0x10050000 (size 0x10)
 * No error checks, minimal logic, prints kept. */

static const unsigned long io_phys = 0x10050000UL;
static const unsigned long io_size = 0x10UL;

static void __iomem *dev_base;
static int major;
static struct class *device_class;
static struct device *device_file;

static int _open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "Device opened\n");
	return 0;
}

static ssize_t _write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	u32 val;
	size_t c = (count > sizeof(val)) ? sizeof(val) : count;
	copy_from_user(&val, buf, c);
	/* write 4 bytes at base only */
	memcpy_toio(dev_base, &val, sizeof(val));
	printk(KERN_INFO "Write operation: wrote %zu bytes to 0x%lx\n", c, io_phys);
	return c;
}

static ssize_t _read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	u32 val;
	size_t c = (count > sizeof(val)) ? sizeof(val) : count;
	/* read 4 bytes at base only */
	memcpy_fromio(&val, dev_base, sizeof(val));
	copy_to_user(buf, &val, c);
	printk(KERN_INFO "Read operation: read %zu bytes from 0x%lx\n", c, io_phys);
	return c;
}

static int _release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "Device closed\n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = _open,
	.read = _read,
	.write = _write,
	.release = _release,
};

static int __init my_init(void)
{
	dev_base = ioremap(io_phys, io_size);
	major = register_chrdev(0, "first_device", &fops);
	device_class = class_create("first_device_class");
	device_file = device_create(device_class, NULL, MKDEV(major, 0), NULL, "first_device");
	printk(KERN_INFO "Device /dev/first_device created at phys 0x%lx size 0x%lx\n", io_phys, io_size);
	return 0;
}

static void __exit my_exit(void)
{
	device_destroy(device_class, MKDEV(major, 0));
	class_destroy(device_class);
	unregister_chrdev(major, "first_device");
	iounmap(dev_base);
	printk(KERN_INFO "Device /dev/first_device removed\n");
}

module_init(my_init);
module_exit(my_exit);
MODULE_AUTHOR("Bartosz Zelek");
MODULE_DESCRIPTION("A simple device module");
MODULE_LICENSE("GPL");
