/*
 * Vmxmon - export the Intel(R) Virtualization Technology (Intel(R) VT) for
 * IA-32, Intel(R) 64 and Intel(R) Architecture (Intel(R) VT-x) to user-space.
 * Copyright 2015-2022 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <linux/cpu.h>
#include <linux/cpumask.h>
#include <asm/io.h>
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/suspend.h>
#include <asm/nmi.h>

#include "core.h"
#include "kernel-api.h"
#include "vmxioctl.h"
#include "api.h"
#include "page.h"
#include "version.h"
#include "dev-ioctl.h"

MODULE_AUTHOR("Intel Corporation");
MODULE_DESCRIPTION("VMP kernel support");
MODULE_VERSION(VMXMON_VERSION_STRING);
MODULE_LICENSE("GPL");

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 6, 0)

static int online_state;

static int
vmxmon_cpu_online(unsigned cpu)
{
        dev_cpu_up(cpu);
        return 0;
}

static int
vmxmon_cpu_down_prepare(unsigned cpu)
{
        dev_cpu_down(cpu);
        return 0;
}

static int
compat_register_cpu_notifier(void)
{
        online_state = cpuhp_setup_state(
                CPUHP_AP_ONLINE_DYN, "vmxmon:online_dyn",
                vmxmon_cpu_online, vmxmon_cpu_down_prepare);
        return (online_state < 0) ? online_state : 0;
}

static void
compat_unregister_cpu_notifier(void)
{
        if (online_state > 0)
                cpuhp_remove_state(online_state);
}

#else   /* LINUX_VERSION_CODE < KERNEL_VERSION(4, 6, 0) */

static int
cpu_hotplug_callback(struct notifier_block *nfb, unsigned long action,
                     void *hcpu)
{
        unsigned int cpu = (unsigned long)hcpu;
        switch (action) {
        case CPU_ONLINE:
        case CPU_DOWN_FAILED:
        case CPU_ONLINE_FROZEN:
        case CPU_DOWN_FAILED_FROZEN:
                dev_cpu_up(cpu);
                return NOTIFY_OK;

        case CPU_DOWN_PREPARE:
        case CPU_DOWN_PREPARE_FROZEN:
                dev_cpu_down(cpu);
                return NOTIFY_OK;
        default:
                return NOTIFY_DONE;
        }
}

/* Unused on non-SMP kernels */
static __attribute__((unused)) struct notifier_block hotplug_cpu_notifier = {
        .notifier_call = cpu_hotplug_callback,
};

static int
compat_register_cpu_notifier(void)
{
        int cpu;
        register_hotcpu_notifier(&hotplug_cpu_notifier);
        for_each_online_cpu(cpu)
                dev_cpu_up(cpu);
        return 0;
}
static void compat_unregister_cpu_notifier(void)
{
        unregister_hotcpu_notifier(&hotplug_cpu_notifier);
}
#endif

static int
power_event(struct notifier_block *nfb, unsigned long action, void *unused)
{
        switch (action) {
        case PM_HIBERNATION_PREPARE:
        case PM_SUSPEND_PREPARE:
                dev_suspend();
                return NOTIFY_OK;
        case PM_POST_HIBERNATION:
        case PM_POST_SUSPEND:
        case PM_POST_RESTORE:
                dev_resume();
                return NOTIFY_OK;
        default:
                return NOTIFY_DONE;
        }
}

static struct notifier_block power_notifier = {
        .notifier_call = power_event,
};
static void compat_register_pm_notifier(void) {
        register_pm_notifier(&power_notifier); 
}
static void compat_unregister_pm_notifier(void) {
        unregister_pm_notifier(&power_notifier); 
}

static int
mod_open(struct inode *inode, struct file *file)
{
        if (!(file->f_mode & FMODE_WRITE))
                return -EPERM;

        file->private_data = dev_open();
        return file->private_data ? 0 : -ENOMEM;
}

static int
mod_release(struct inode *inode, struct file *file)
{
        vm_t *vm = file->private_data;
        dev_release(vm);
        return 0;
}

static long
vmxmon_ioctl(struct file *file, unsigned int cmd, unsigned long xarg)
{
        vm_t *vm = file->private_data;
        void __user *arg = (void __user *)xarg;
        return dev_ioctl(vm, cmd, arg);
}

static struct file_operations device_fops = {
        .owner          = THIS_MODULE,
        .open           = mod_open,
        .release        = mod_release,
        .unlocked_ioctl = vmxmon_ioctl,
        .compat_ioctl   = vmxmon_ioctl,
};

static struct file_operations nmihook_fops = {
        .owner = THIS_MODULE,
};

static struct miscdevice vmx_device = {
        MISC_DYNAMIC_MINOR, "vmx", &device_fops
};

/* dummy device, for backward compatibility */
static struct miscdevice nmihook_device = {
        MISC_DYNAMIC_MINOR, "nmihook", &nmihook_fops
};

static int __init
mod_register(void)
{
        int err;
        struct sysinfo si;
        unsigned long host_total_mem_mb;

        si_meminfo(&si);
        host_total_mem_mb = si.totalram * 4 / 1024;

        /* unless user has specified a limit explicitly, use the default
           memory limit which is based on the amount of available memory */ 
        if (mp.mem_limit_mb == 0)
                mp.mem_limit_mb = host_total_mem_mb / 2;

        err = init_api();
        if (err)
                return err;

        err = dev_register();
        if (err)
                return err;

        err = compat_register_cpu_notifier();
        if (err)
                goto out0;
        compat_register_pm_notifier();

        err = misc_register(&nmihook_device);
        if (err)
                goto out1;

        err = misc_register(&vmx_device);
        if (err)
                goto out2;
        return 0;

out2:
        misc_deregister(&nmihook_device);
out1:
        compat_unregister_pm_notifier();
        compat_unregister_cpu_notifier();
out0:
        dev_unregister();
        return err;
}

static void __exit
mod_unregister(void)
{
        misc_deregister(&vmx_device);
        misc_deregister(&nmihook_device);
        compat_unregister_pm_notifier();
        compat_unregister_cpu_notifier();
        dev_unregister();
        assert_memory_released();
}

module_init(mod_register);
module_exit(mod_unregister);
