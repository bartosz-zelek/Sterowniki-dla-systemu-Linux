#include <linux/module.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/io.h>

static struct kobject *my_kobj;

static void __iomem *mmio;
#define MMIO_BASE 0x10050000UL
#define MMIO_SIZE 0x10

#define REG_ARG1      0x00
#define REG_ARG2      0x04
#define REG_OPERATION 0x08
#define REG_RESULT    0x0C

static inline u32 dev_read(u32 off)
{
    return ioread32(mmio + off);
}

static inline void dev_write(u32 off, u32 val)
{
    iowrite32(val, mmio + off);
}

static ssize_t arg1_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    u32 val = dev_read(REG_ARG1);
    pr_info("dev_module: odczyt %s: %u\n", attr->attr.name, val);
    return sysfs_emit(buf, "%u\n", val);
}

static ssize_t arg1_store(struct kobject *kobj, struct kobj_attribute *attr,
                          const char *buf, size_t count)
{
    int val;
    int ret = kstrtoint(buf, 0, &val);
    if (ret)
        return ret;
    pr_info("dev_module: zapis %s: %d\n", attr->attr.name, val);
    dev_write(REG_ARG1, (u32)val);
    return count;
}

static struct kobj_attribute arg1_attribute = __ATTR(arg1, 0664, arg1_show, arg1_store);

static ssize_t arg2_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    u32 val = dev_read(REG_ARG2);
    pr_info("dev_module: odczyt %s: %u\n", attr->attr.name, val);
    return sysfs_emit(buf, "%u\n", val);
}

static ssize_t arg2_store(struct kobject *kobj, struct kobj_attribute *attr,
                          const char *buf, size_t count)
{
    int val;
    int ret = kstrtoint(buf, 0, &val);
    if (ret)
        return ret;
    pr_info("dev_module: zapis %s: %d\n", attr->attr.name, val);
    dev_write(REG_ARG2, (u32)val);
    return count;
}

static struct kobj_attribute arg2_attribute = __ATTR(arg2, 0664, arg2_show, arg2_store);

static ssize_t operation_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    u32 val = dev_read(REG_OPERATION);
    pr_info("dev_module: odczyt %s: %u\n", attr->attr.name, val);
    return sysfs_emit(buf, "%u\n", val);
}

static ssize_t operation_store(struct kobject *kobj, struct kobj_attribute *attr,
                               const char *buf, size_t count)
{
    int val;
    int ret = kstrtoint(buf, 0, &val);
    if (ret)
        return ret;
    pr_info("dev_module: zapis %s: %d\n", attr->attr.name, val);
    dev_write(REG_OPERATION, (u32)val);
    return count;
}

static struct kobj_attribute operation_attribute = __ATTR(operation, 0664, operation_show, operation_store);

static ssize_t result_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    u32 val = dev_read(REG_RESULT);
    pr_info("dev_module: odczyt %s: %u\n", attr->attr.name, val);
    return sysfs_emit(buf, "%u\n", val);
}

static ssize_t result_store(struct kobject *kobj, struct kobj_attribute *attr,
                            const char *buf, size_t count)
{
    int val;
    int ret = kstrtoint(buf, 0, &val);
    if (ret)
        return ret;
    pr_info("dev_module: zapis %s: %d\n", attr->attr.name, val);
    dev_write(REG_RESULT, (u32)val);
    return count;
}

static struct kobj_attribute result_attribute = __ATTR(result, 0664, result_show, result_store);

static void teardown(void)
{
    if (my_kobj) {
        sysfs_remove_file(my_kobj, &result_attribute.attr);
        sysfs_remove_file(my_kobj, &operation_attribute.attr);
        sysfs_remove_file(my_kobj, &arg2_attribute.attr);
        sysfs_remove_file(my_kobj, &arg1_attribute.attr);
        kobject_put(my_kobj);
        my_kobj = NULL;
    }
    if (mmio) {
        iounmap(mmio);
        mmio = NULL;
    }
}

static int __init my_init(void)
{
    int ret;

    mmio = ioremap(MMIO_BASE, MMIO_SIZE);
    if (!mmio) {
    pr_err("dev_module: ioremap nie powiodlo sie (base=0x%lx, size=0x%x)\n", (unsigned long)MMIO_BASE, MMIO_SIZE);
        return -ENOMEM;
    }

    my_kobj = kobject_create_and_add("my_object", kernel_kobj);
    if (!my_kobj) {
        teardown();
        return -ENOMEM;
    }

    ret = sysfs_create_file(my_kobj, &arg1_attribute.attr);
    if (ret) {
        teardown();
        return ret;
    }
    ret = sysfs_create_file(my_kobj, &arg2_attribute.attr);
    if (ret) {
        teardown();
        return ret;
    }
    ret = sysfs_create_file(my_kobj, &operation_attribute.attr);
    if (ret) {
        teardown();
        return ret;
    }
    ret = sysfs_create_file(my_kobj, &result_attribute.attr);
    if (ret) {
        teardown();
        return ret;
    }

    pr_info("dev_module: utworzono /sys/kernel/my_object (arg1,arg2,operation,result), mmio=0x%lx\n",
            (unsigned long)MMIO_BASE);
    return 0;
}

static void __exit my_exit(void)
{
    teardown();
    pr_info("dev_module: usunieto /sys/kernel/my_object i odmapowano MMIO\n");
}

module_init(my_init);
module_exit(my_exit);
MODULE_AUTHOR("Bartosz Zelek");
MODULE_DESCRIPTION("A simple device module");
MODULE_LICENSE("GPL");