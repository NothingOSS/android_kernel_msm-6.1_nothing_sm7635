// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2020-2022 Nothing. All rights reserved.
 */

#define pr_fmt(fmt) "%s:%s " fmt, KBUILD_MODNAME, __func__

#include <linux/device.h>
#include <linux/err.h>
#include <linux/idr.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>
#include <linux/thermal.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <trace/hooks/thermal.h>

enum {
	SHELL_FRONT = 0,
	SHELL_FRAME,
	SHELL_BACK,
	SHELL_MAX,
    MAX_SIZE
};

#define BUF_LEN		256
/* trips config */
#define TRIPS_NUM 2
#define TRIPS_HYST 1000
#define TRIPS_TEMP 125000

static DEFINE_IDA(virtual_temp_ida);
static DEFINE_SPINLOCK(sync_temp_lock);
static int virtual_temp[MAX_SIZE];

struct virtual_thermal_trip {
        int temperature;
        int hysteresis;
        enum thermal_trip_type type;
};


struct virtual_temp {
	struct thermal_zone_device *tzd;
	int id;

	/* trip data */
	int ntrips;
	struct virtual_thermal_trip *trips;
	int prev_low_trip;
	int prev_high_trip;
	int prev_temp;

	spinlock_t trips_lock;
};

static struct virtual_temp *virtual_ntc[MAX_SIZE];

static const struct of_device_id virtual_of_match[] = {
	{ .compatible = "nothing,virtual-ntc" },
	{},
};

static int get_temp(struct thermal_zone_device *tz,
					int *temp)
{
	struct virtual_temp *hst;

	if (!temp || !tz)
		return -EINVAL;

	hst = tz->devdata;
	*temp = virtual_temp[hst->id];
	return 0;
}

static int get_trip_type(struct thermal_zone_device *tz, int trip,
				    enum thermal_trip_type *type)
{
	struct virtual_temp *hst = tz->devdata;

	if (trip >= hst->ntrips || trip < 0)
		return -EDOM;

	*type = hst->trips[trip].type;

	return 0;
}

static int get_trip_temp(struct thermal_zone_device *tz, int trip,
				    int *temp)
{
	struct virtual_temp *hst = tz->devdata;

	if (trip >= hst->ntrips || trip < 0)
		return -EDOM;

	*temp = hst->trips[trip].temperature;

	return 0;
}

static int set_trip_temp(struct thermal_zone_device *tz, int trip,
				    int temp)
{
	struct virtual_temp *hst = tz->devdata;
	unsigned long flags;

	if (trip >= hst->ntrips || trip < 0)
		return -EDOM;

	spin_lock_irqsave(&hst->trips_lock, flags);
	hst->trips[trip].temperature = temp;
	spin_unlock_irqrestore(&hst->trips_lock, flags);

	pr_info("trips[%d].temperature = %d", trip, temp);

	return 0;
}

static int get_trip_hyst(struct thermal_zone_device *tz, int trip,
				    int *hyst)
{
	struct virtual_temp *hst = tz->devdata;

	if (trip >= hst->ntrips || trip < 0)
		return -EDOM;

	*hyst = hst->trips[trip].hysteresis;
	return 0;
}

static int set_trip_hyst(struct thermal_zone_device *tz, int trip,
				    int hyst)
{
	struct virtual_temp *hst = tz->devdata;
	unsigned long flags;

	if (trip >= hst->ntrips || trip < 0)
		return -EDOM;

	spin_lock_irqsave(&hst->trips_lock, flags);
	hst->trips[trip].hysteresis = hyst;
	spin_unlock_irqrestore(&hst->trips_lock, flags);

	pr_info("trips[%d].hysteresis = %d", trip, hyst);

	return 0;
}

struct thermal_zone_device_ops virtual_thermal_zone_ops = {
	.get_temp = get_temp,
	.get_trip_type = get_trip_type,
	.get_trip_temp = get_trip_temp,
	.set_trip_temp = set_trip_temp,
	.get_trip_hyst = get_trip_hyst,
	.set_trip_hyst = set_trip_hyst,
};

static int build_trips(struct virtual_temp *hst)
{
	int i, ret = 0;

	/* trips for qcom thermal-engine and thermal-hal */
	hst->ntrips = TRIPS_NUM;

	hst->trips = kcalloc(hst->ntrips, sizeof(*hst->trips), GFP_KERNEL);
	if (!hst->trips) {
		ret = -ENOMEM;
		return ret;
	}

	/* Initialize the trip configuration */
	for(i = 0; i < hst->ntrips; i++){
		hst->trips[i].hysteresis = TRIPS_HYST;
		hst->trips[i].temperature = TRIPS_TEMP;
		hst->trips[i].type = THERMAL_TRIP_PASSIVE;
	}

	return ret;
}

static void tz_is_irq(void *unused, struct thermal_zone_device *tz, int *irq_wakeable) {
    if (tz->polling_delay_jiffies == 0)
        *irq_wakeable = 1;
}

static int virtual_temp_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *dev_node = dev->of_node;
	struct thermal_zone_device *tz_dev;
	struct virtual_temp *hst;
	int ret = 0;
	int mask = 0;
	int result, i;

	if (!of_device_is_available(dev_node)) {
		pr_err("virtual-temp dev not found\n");
		return -ENODEV;
	}

	hst = kzalloc(sizeof(struct virtual_temp), GFP_KERNEL);
	if (!hst) {
		pr_err("alloc virtual_temp mem failed\n");
		return -ENOMEM;
	}

	result = ida_simple_get(&virtual_temp_ida, 0, 0, GFP_KERNEL);
	if (result < 0) {
		pr_err("genernal stlntc id failed\n");
		ret = -EINVAL;
		goto err_free_mem;
	}

	hst->id = result;

	result = build_trips(hst);
	if(result < 0){
		pr_err("build_trips failed\n");
		ret = -EINVAL;
		goto err_remove_id;
	}

	for (i = 0; i < hst->ntrips; i++)
		mask |= 1 << i;

	tz_dev = thermal_zone_device_register(dev_node->name,
			hst->ntrips, mask, hst, &virtual_thermal_zone_ops, NULL, 0, 0);
	if (IS_ERR_OR_NULL(tz_dev)) {
		pr_err("register thermal zone for virtual temp failed\n");
		ret = -ENODEV;
		goto err_free_trip;
	}
	thermal_zone_device_enable(tz_dev);
	hst->tzd = tz_dev;
	hst->prev_high_trip = INT_MAX;
	hst->prev_low_trip = -INT_MAX;
	hst->prev_temp = 0;
	spin_lock_init(&hst->trips_lock);

	virtual_ntc[hst->id] = hst;

	platform_set_drvdata(pdev, hst);

    register_trace_android_vh_thermal_pm_notify_suspend(tz_is_irq, NULL);

	return 0;
err_free_trip:
	kfree(hst->trips);
err_remove_id:
	ida_simple_remove(&virtual_temp_ida, result);
err_free_mem:
	kfree(hst);
	return ret;
}

static int virtual_temp_remove(struct platform_device *pdev)
{
	struct virtual_temp *hst = platform_get_drvdata(pdev);

	if (hst) {
		platform_set_drvdata(pdev, NULL);
		thermal_zone_device_unregister(hst->tzd);
		if(hst->trips)
			kfree(hst->trips);
		kfree(hst);
	}

	return 0;
}

static struct platform_driver virtual_temp_platdrv = {
	.driver = {
		.name		= "virtual-temp",
		.owner		= THIS_MODULE,
		.of_match_table = virtual_of_match,
	},
	.probe	= virtual_temp_probe,
	.remove = virtual_temp_remove,
};


static ssize_t attr_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf, size_t count)
{
    int ret, temp, len, i;
    int low = -INT_MAX;
    int high = INT_MAX;
    unsigned int index = 0;
    char tmp[BUF_LEN + 1];
    unsigned long flags;
    bool is_trigger = false;


    if (count == 0) {
        return 0;
    }

    if (count > BUF_LEN) {
        len = BUF_LEN;
    } else {
        len = count;
    }

    strncpy(tmp, buf, len);

    if (tmp[len-1] == '\n') {
        tmp[len-1] = '\0';
    } else {
        tmp[len] = '\0';
    }

    ret = sscanf(tmp, "%d %d", &index, &temp);
    if (ret < 2) {
        pr_err("store failed, ret=%d\n", ret);
        return count;
    }

    if (index >= MAX_SIZE) {
        pr_err("store invalid params\n");
        return count;
    }

    if (virtual_ntc[index]->trips != NULL) {
        spin_lock_irqsave(&virtual_ntc[index]->trips_lock, flags);
        for (i = 0; i < TRIPS_NUM; i++) {
            int trip_low, trip_high;

            trip_low = virtual_ntc[index]->trips[i].temperature - virtual_ntc[index]->trips[i].hysteresis;
            trip_high = virtual_ntc[index]->trips[i].temperature;

            if (trip_low < temp && trip_low > low)
                low = trip_low;

            if (trip_high > temp && trip_high < high)
                high = trip_high;
        }

        if (temp >= virtual_ntc[index]->prev_high_trip || temp <= virtual_ntc[index]->prev_low_trip) {
            pr_info("trigger! virtual_ntc[%d]: prev_low_trip = %d, prev_high_trip = %d\n", index, virtual_ntc[index]->prev_low_trip, virtual_ntc[index]->prev_high_trip);
            pr_info("virtual_ntc[%d]: trip_low = %d, trip_high = %d, temp = %d\n", index, low, high, temp);
            is_trigger = true;
        }

        virtual_ntc[index]->prev_low_trip = low;
        virtual_ntc[index]->prev_high_trip = high;

        spin_unlock_irqrestore(&virtual_ntc[index]->trips_lock, flags);
    }

    if (abs(virtual_ntc[index]->prev_temp - temp) >= 1000)
        is_trigger = true;

    spin_lock_irqsave(&sync_temp_lock, flags);
    virtual_temp[index] = temp;
    spin_unlock_irqrestore(&sync_temp_lock, flags);
    virtual_ntc[index]->prev_temp = temp;

    if (is_trigger && virtual_ntc[index]->tzd != NULL) {
        thermal_zone_device_update(virtual_ntc[index]->tzd, THERMAL_TRIP_VIOLATED);
    }

    return count;
}

static ssize_t attr_show(struct device *dev,
        struct device_attribute *attr,
        char *buf)
{
    int temp = 0, index, val;
    unsigned long flags;

    spin_lock_irqsave(&sync_temp_lock, flags);
    for (index = 0; index < MAX_SIZE; index++) {
        if (virtual_temp[index] > temp) {
            temp = virtual_temp[index];
        }
        printk("virtual_temp[%d] is %d", index, virtual_temp[index]);
    }
    spin_unlock_irqrestore(&sync_temp_lock, flags);

    val = sprintf(buf, "%d\n", temp);
    printk("Max virtual temp is %d", temp);
    return val;
}

static struct class *virtual_temp_class;
static struct device *dev;

static DEVICE_ATTR(data, 0644, attr_show, attr_store);

static int __init init_virtual_temp(void)
{
    int ret = 0;
	printk("Init virtual_temp dev\n");
    virtual_temp_class = class_create(THIS_MODULE, "virtual_temp");
    if (IS_ERR(virtual_temp_class)) {
        ret = PTR_ERR(virtual_temp_class);
        printk(KERN_ALERT "Failed to create class.\n");
        return ret;
    }

    dev = device_create(virtual_temp_class, NULL, MKDEV(100,0), NULL, "shell");
    if (IS_ERR(dev)) {
        ret = PTR_ERR(dev);
        printk(KERN_ALERT "Failed to create device\n");
        return ret;
    }

    ret = device_create_file(dev, &dev_attr_data);
    if (ret < 0) {
        printk(KERN_ALERT "Failed to create attribute file.\n");
        return ret;
    }

	spin_lock_init(&sync_temp_lock);

	return platform_driver_register(&virtual_temp_platdrv);
}

static void __exit cleanup_virtual_temp(void)
{
    printk("Cleanup virtual_temp\n");
	platform_driver_unregister(&virtual_temp_platdrv);
    device_remove_file(dev, &dev_attr_data);
    device_destroy(virtual_temp_class, MKDEV(100, 0));
    class_destroy(virtual_temp_class);
}


module_init(init_virtual_temp);
module_exit(cleanup_virtual_temp);
MODULE_LICENSE("GPL v2");
