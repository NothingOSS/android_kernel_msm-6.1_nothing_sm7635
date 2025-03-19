
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/kobject.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/sysfs.h>
#include <linux/pm.h>

#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

#include <linux/err.h>
#include <linux/input.h>
#include <linux/jiffies.h>

#include <linux/ctype.h>
#include <linux/pm_runtime.h>


#include <linux/irq.h>
#include <linux/of_gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/iio/consumer.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>




struct volt_detect_info {
	struct platform_device *pdev;
};

struct iio_channel *gpio1_voltage_chan;
struct volt_detect_info *svolt_detect = NULL;
static char *hw_ver[]     = {"pre-T0","T0","EVT1","EVT2","DVT1","DVT2","PVT","MP","reserved","wrong version"};

/*
static int hwid_show(struct seq_file *m, void *v)
{
	uint32_t volt = -EINVAL;
	int version = 7;
	int ret =-EINVAL;

	pr_info("[hwid]hwid enter\n");
	if(svolt_detect == NULL) {
		return -EINVAL;
	}
	seq_puts(m, "hwid\n");
	gpio1_voltage_chan = iio_channel_get(&svolt_detect->pdev->dev, "gpio4_voltage");
	if (!gpio1_voltage_chan) {
		pr_err("get iio channel adc fail\n");
		return -EINVAL;
	}

	ret = iio_read_channel_processed(gpio1_voltage_chan, &volt);
	pr_info("[hwid]channel val = %d,return val = %d\n", volt, ret);
	if (ret < 0) {
			pr_err("[volt_detect]Error in reading gpio1_voltage channel, rc:%d\n", ret);
	}
	volt = volt / 10000;

	if (volt >= 0 && volt <= 6) {
		version = 0;
		pr_info("[hwid]pre-T0\n");
	} else if (volt >= 10 && volt <= 25) {
		version = 1;
		pr_info("[hwid]T0\n");
	} else if (volt >= 38 && volt <= 68) {
		version = 2;
		pr_info("[hwid]EVT\n");
	} else if (volt >= 78 && volt <= 105) {
		version = 3;
		pr_info("[hwid]DVT\n");
	} else if (volt >= 115 && volt <= 128) {
		version = 4;
		pr_info("[hwid]PVT\n");
	} else if (volt >= 138 && volt <= 150) {
		version = 5;
		pr_info("[hwid]MP\n");
	} else if (volt >= 158 && volt <= 170) {
		version = 6;
		pr_info("[hwid]reserved\n");
	} else {
		version = 7;
		pr_info("[hwid]wrong status. \n");
	}

	//mutex_unlock(&svolt_detect->pdev->dev.mutex);
	seq_printf(m, "hwid");
	seq_printf(m, "%d", volt);
	seq_printf(m, "%s", hw_ver[version]);
	return ret;
}

static int hwid_open(struct inode *inode, struct file *file)
{
	return single_open(file, hwid_show, inode->i_private);
}

static const struct proc_ops hwid_proc_ops = {
	.proc_open		= hwid_open,
	.proc_read		= seq_read,
	.proc_release   = single_release,
};

static int create_hwid_proc_file(void)
{
	struct proc_dir_entry *dir;

	dir = proc_create("hwid", 0444, NULL, &hwid_proc_ops);
	if (!dir) {
		pr_err("Unable to create /proc/hwid");
		return -1;
	}

	return 0;
}
*/
static ssize_t hwid_sys_show(
    struct device *dev, struct device_attribute *attr, char *buf)
{
	int count = 0;
	uint32_t volt = -EINVAL;
	int version = 9;
	int ret =-EINVAL;

	pr_info("[hwid]hwid sys enter\n");

	if(svolt_detect == NULL) {
		return -EINVAL;
	}
	mutex_lock(&svolt_detect->pdev->dev.mutex);

	gpio1_voltage_chan = iio_channel_get(&svolt_detect->pdev->dev, "gpio4_voltage");
	if (!gpio1_voltage_chan) {
		pr_err("get iio channel adc fail\n");
		return -EINVAL;
	}

	ret = iio_read_channel_processed(gpio1_voltage_chan, &volt);
	pr_info("[hwid]channel val = %d,return val = %d\n", volt, ret);
	if (ret < 0) {
			pr_err("[volt_detect]Error in reading gpio4_voltage channel, rc:%d\n", ret);
	}
	volt = volt / 10000;

	if (volt >= 0 && volt <= 6) {
		version = 0;
		pr_info("[hwid]pre-T0\n");
	} else if (volt >= 10 && volt <= 25) {
		version = 1;
		pr_info("[hwid]T0\n");
	} else if (volt >= 38 && volt <= 48) {
		version = 2;
		pr_info("[hwid]EVT1\n");
	} else if (volt >= 58 && volt <= 68) {
		version = 3;
		pr_info("[hwid]EVT2\n");
	}else if (volt >= 78 && volt <= 88) {
		version = 4;
		pr_info("[hwid]DVT1\n");
	} else if (volt >= 95 && volt <= 105) {
		version = 5;
		pr_info("[hwid]DVT2\n");
	} else if (volt >= 115 && volt <= 134) {
		version = 6;
		pr_info("[hwid]PVT\n");
	} else if (volt >= 136 && volt <= 150) {
		version = 7;
		pr_info("[hwid]MP\n");
	} else if (volt >= 158 && volt <= 170) {
		version = 8;
		pr_info("[hwid]reserved\n");
	} else {
		version = 9;
		pr_info("[hwid]wrong status. \n");
	}

	count = snprintf(buf, PAGE_SIZE, "volt val:%d,version:%s\n",
		volt, hw_ver[version]);
	mutex_unlock(&svolt_detect->pdev->dev.mutex);

	return count;
}

static DEVICE_ATTR(hwid, S_IRUGO,
		hwid_sys_show, NULL);

static struct attribute *hwid_mode_attrs[] = {
	&dev_attr_hwid.attr,
	NULL,
};

static struct attribute_group hwid_group = {
	.attrs = hwid_mode_attrs,
};

static int create_hwid_sys_file(void)
{
	int ret = 0;

	pr_info("[hwid] sys create enter");
	ret = sysfs_create_group(&svolt_detect->pdev->dev.kobj, &hwid_group);
	 if (ret) {
        pr_err("[hwid] sys node create fail\n");
        sysfs_remove_group(&svolt_detect->pdev->dev.kobj, &hwid_group);
    }
	 return ret;
}

static int volt_detect_probe(struct platform_device *pdev)
{
	struct volt_detect_info *volt_detect;

	pr_info("[hwid] probe enter\n");

	volt_detect = kzalloc(sizeof(struct volt_detect_info), GFP_KERNEL);

	platform_set_drvdata(pdev, volt_detect);
	svolt_detect = volt_detect;

	volt_detect->pdev= pdev;

	//create_hwid_proc_file();
	create_hwid_sys_file();
	return 0;
}

static const struct of_device_id volt_detect_of_match[] = {
	{ .compatible = "qcom,volt_detect", },
	{ },
};

MODULE_DEVICE_TABLE(of, volt_detect_of_match);

static struct platform_driver volt_detect_device_driver = {
	.probe		= volt_detect_probe,
	.driver		= {
	.name	= "volt_detect",
	.of_match_table = volt_detect_of_match,
	}
};

static int __init volt_detect_init(void)
{
	pr_info("detect enter");
	return platform_driver_register(&volt_detect_device_driver);
}

static void __exit volt_detect_exit(void)
{
	printk(KERN_EMERG"[hwid]%s exit.....\n", __func__);
	//remove_proc_entry("hwid", NULL);
	sysfs_remove_group(&svolt_detect->pdev->dev.kobj, &hwid_group);
	platform_driver_unregister(&volt_detect_device_driver);
}


late_initcall(volt_detect_init);
module_exit(volt_detect_exit);
MODULE_DESCRIPTION("voltage detect function");
MODULE_LICENSE("GPL v2");

