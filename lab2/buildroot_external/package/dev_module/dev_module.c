#include <linux/module.h>
#include <linux/init.h>

static int __init my_init(void)
{
	pr_emerg("KERN_EMERG in line %d\n", __LINE__);
	pr_alert("KERN_ALERT in line %d\n", __LINE__);
	pr_crit("KERN_CRIT in line %d\n", __LINE__);
	pr_err("KERN_ERR in line %d\n", __LINE__);
	pr_warn("KERN_WARNING in line %d\n", __LINE__);
	pr_notice("KERN_NOTICE in line %d\n", __LINE__);
	pr_info("KERN_INFO in line %d\n", __LINE__);
	pr_debug("KERN_DEBUG in line %d\n", __LINE__);
	return 0;
}

static void __exit my_exit(void)
{
	pr_emerg("KERN_EMERG in line %d\n", __LINE__);
	pr_alert("KERN_ALERT in line %d\n", __LINE__);
	pr_crit("KERN_CRIT in line %d\n", __LINE__);
	pr_err("KERN_ERR in line %d\n", __LINE__);
	pr_warn("KERN_WARNING in line %d\n", __LINE__);
	pr_notice("KERN_NOTICE in line %d\n", __LINE__);
	pr_info("KERN_INFO in line %d\n", __LINE__);
	pr_debug("KERN_DEBUG in line %d\n", __LINE__);
}

module_init(my_init);
module_exit(my_exit);
MODULE_AUTHOR("Bartosz Zelek");
MODULE_DESCRIPTION("A simple device module");
MODULE_LICENSE("GPL");