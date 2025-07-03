#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/ioport.h>
#include <linux/io.h>

static const struct of_device_id first_device_of_match[] = {
    { .compatible = "put,first_device", },
    {  }
};

MODULE_DEVICE_TABLE(of, first_device_of_match);

static int first_device_probe(struct platform_device *pdev)
{
    struct resource *res;
    const char *my_string;
    u32 my_fixed;

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

    dev_info(&pdev->dev, "Resource: start=0x%llx, end=0x%llx, size=0x%llx\n",
             (unsigned long long)res->start,
             (unsigned long long)res->end,
             (unsigned long long)resource_size(res));

    if (!of_property_read_string(pdev->dev.of_node, "my_string", &my_string))
        dev_info(&pdev->dev, "my_string: %s\n", my_string);
    else
        dev_info(&pdev->dev, "my_string not found\n");

    if (!of_property_read_u32(pdev->dev.of_node, "my_fixed", &my_fixed))
        dev_info(&pdev->dev, "my_fixed: %u (0x%x)\n", my_fixed, my_fixed);
    else
        dev_info(&pdev->dev, "my_fixed not found\n");

    if (of_property_read_bool(pdev->dev.of_node, "my_flag"))
    	dev_info(&pdev->dev, "my_flag is present (true)\n");
	else
    	dev_info(&pdev->dev, "my_flag is not present (false)\n");

    return 0;
}

static struct platform_driver first_device_driver = {
    .probe = first_device_probe,
    .driver = {
        .name = "first_device",
        .of_match_table = first_device_of_match,
        .owner = THIS_MODULE,
    },
};

module_platform_driver(first_device_driver); // macro with platform_driver_register

MODULE_AUTHOR("Bartosz Zelek");
MODULE_DESCRIPTION("A simple device module");
MODULE_LICENSE("GPL");