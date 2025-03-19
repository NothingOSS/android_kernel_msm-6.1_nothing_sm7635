/*
 * drivers/haptic/sfdc_drv.c
 *
 * Copyright (c) 2023 ICSense Semiconductor CO., LTD
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation (version 2 of the License only).
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/input.h>
#include <linux/miscdevice.h>
#include <linux/syscalls.h>
#include <linux/vmalloc.h>
#include <linux/poll.h>

#include "sfdc_drv.h"

static wait_queue_head_t wait_queue;
static struct ics_haptic_data *g_haptic_data = NULL;
static int32_t bemf_daq_done = 0;

static int sfdc_file_open(struct inode *inode, struct file *file)
{
    if (!try_module_get(THIS_MODULE))
    {
        return -ENODEV;
    }

    return 0;
}

static int sfdc_file_release(struct inode *inode, struct file *file)
{
    module_put(THIS_MODULE);

    return 0;
}

static ssize_t sfdc_file_read(struct file *file,
        char __user *user_buf, size_t count, loff_t *ppos)
{
    return 0;
}

static ssize_t sfdc_file_write(struct file *file,
        const char __user *user_buf, size_t count, loff_t *ppos)
{
    return count;
}

static uint32_t sfdc_bemf_daq_poll(struct file *filep, struct poll_table_struct *table)
{
    uint32_t mask = 0;

    ics_dbg("%s: sfdc poll start!\n", __func__);
    poll_wait(filep, &wait_queue, table);

    if (bemf_daq_done == 1)
    {
        ics_dbg("%s: bemf_daq_done!\n", __func__);
        mask |= POLLIN;
        bemf_daq_done = 0;
    }
    ics_dbg("%s: sfdc poll end!\n", __func__);
    return mask;
}

static struct file_operations sfdc_fops =
{
    .owner = THIS_MODULE,
    .read = sfdc_file_read,
    .write = sfdc_file_write,
    .open = sfdc_file_open,
    .release = sfdc_file_release,
    .poll = sfdc_bemf_daq_poll,
};

static struct miscdevice sfdc_misc_dev =
{
    .minor = MISC_DYNAMIC_MINOR,
    .name = "ics_sfdc",
    .fops = &sfdc_fops,
};

int32_t sfdc_misc_register(struct ics_haptic_data *haptic_data)
{
    ics_info("%s: start register sfdc misc dev\n", __func__);
    init_waitqueue_head(&wait_queue);

    misc_register(&sfdc_misc_dev);
    g_haptic_data = haptic_data;

    ics_info("%s: end register sfdc misc dev\n", __func__);
    return 0;
}

int32_t sfdc_misc_remove(struct ics_haptic_data *haptic_data)
{
    misc_deregister(&sfdc_misc_dev);
    return 0;
}

int32_t sfdc_bemf_daq_clear(void)
{
    bemf_daq_done = 0;
    return 0;
}

int32_t sfdc_wakeup_bemf_daq_poll(void)
{
    ics_dbg("%s: wakeup bemf daq poll!\n", __func__);
    bemf_daq_done = 1;
    wake_up(&wait_queue);

    return 1;
}
