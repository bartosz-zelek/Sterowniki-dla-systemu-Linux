#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>

static char buffer[256];

static int _open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "Device opened\n");
	return 0;
}

static ssize_t _write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	printk(KERN_INFO "Write operation\n");

	if (copy_from_user(buffer, buf, count)) {
		printk(KERN_ALERT "Failed to copy data from user space\n");
		return -EFAULT;
	}
	buffer[count] = '\0'; // Null-terminate the string
	printk(KERN_INFO "Received from user: %s\n", buffer);

	return count;
}

static ssize_t _read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	printk(KERN_INFO "Read operation\n");
	size_t len = strlen(buffer);
	for (size_t i = 0; i < len / 2; i++) {
		char temp = buffer[i];
		buffer[i] = buffer[len - i - 1];
		buffer[len - i - 1] = temp;
	}
	copy_to_user(buf, buffer, len);
	buffer[0] = '\0'; // Clear the buffer after reading
	return len;
}


static int _release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "Device closed\n");
	return 0;
}

static struct file_operations fops = {
	.open = _open,
	.read = _read,
	.write = _write,
	.release = _release,
};

static int major;
static struct class *device_class;
static struct device *device_file;

static int __init my_init(void)
{
	major = register_chrdev(0, "first_device", &fops);
	if (major < 0) {
		printk(KERN_ALERT "Failed to register character device\n");
		return major;
	}
	printk(KERN_INFO "Character device registered with major number %d\n", major);

	// Tworzenie klasy urządzenia
	device_class = class_create("first_device_class");
	if (IS_ERR(device_class)) {
		unregister_chrdev(major, "first_device");
		printk(KERN_ALERT "Failed to create device class\n");
		return PTR_ERR(device_class);
	}

	// Tworzenie urządzenia w /dev
	device_file = device_create(device_class, NULL, MKDEV(major, 0), NULL, "first_device");
	if (IS_ERR(device_file)) {
		class_destroy(device_class);
		unregister_chrdev(major, "first_device");
		printk(KERN_ALERT "Failed to create device file\n");
		return PTR_ERR(device_file);
	}

	printk(KERN_INFO "Device /dev/first_device created successfully\n");
	return 0;
}

static void __exit my_exit(void)
{
	device_destroy(device_class, MKDEV(major, 0));
	class_destroy(device_class);
	unregister_chrdev(major, "first_device");
	printk(KERN_INFO "Device /dev/first_device removed\n");
}

module_init(my_init);
module_exit(my_exit);
MODULE_AUTHOR("Bartosz Zelek");
MODULE_DESCRIPTION("A simple device module");
MODULE_LICENSE("GPL");