#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/device.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#define CLASS_NAME "slot_detect"

struct slot_detect_info {
	struct platform_device *pdev;
	struct class *slot_detect_class;
	int uim0;
	int uim1;
};

static char *detect_result[] = {"absent","present"};

static struct slot_detect_info *slot;

static ssize_t uim0_show(struct class *class, struct class_attribute *attr,
                            char *buf)
{
	int value;
	value = gpio_get_value(slot->uim0);
	return sprintf(buf, "%s\n",detect_result[value]);
}

static ssize_t uim1_show(struct class *class, struct class_attribute *attr,
                            char *buf)
{
	int value;
	value = gpio_get_value(slot->uim1);
	return sprintf(buf, "%s\n",detect_result[value]);
}

static CLASS_ATTR_RO(uim0);
static CLASS_ATTR_RO(uim1);

static struct attribute *slot_detect_class_attrs[] = {
		&class_attr_uim0.attr,
		&class_attr_uim1.attr,
		NULL,
};

ATTRIBUTE_GROUPS(slot_detect_class);

static struct class slot_detect_class = {
	.name = CLASS_NAME,
	.owner = THIS_MODULE,
	.class_groups = slot_detect_class_groups,
};

static int slot_detect_parse_dt(void)
{
	slot->uim0 = of_get_named_gpio(slot->pdev->dev.of_node,"uim0_present",0);
	if(slot->uim0 < 0){
		dev_err(&slot->pdev->dev,"can not get uim0 present gpio");
		return -EINVAL;
	}

	slot->uim1 = of_get_named_gpio(slot->pdev->dev.of_node,"uim1_present",0);
	if(slot->uim1 < 0){
		dev_err(&slot->pdev->dev,"can not get uim1 present gpio");
		return -EINVAL;
	}
	return 0;
}

static int slot_detect_class_probe(struct platform_device *pdev){

	int ret;

	slot = kzalloc(sizeof(struct slot_detect_info), GFP_KERNEL);
	slot->pdev = pdev;
	slot->slot_detect_class = &slot_detect_class;

	ret = slot_detect_parse_dt();
	if(ret){
		dev_err(&slot->pdev->dev,"slot_detect_parse_dt failed");
		goto err;
	}

	ret = class_register(&slot_detect_class);
	if(ret){
		dev_err(&slot->pdev->dev,"class_register fail\n");
		goto err;
	}

	return ret;
err:
	kfree(slot);
	return ret;
}

static const struct of_device_id slot_detect_of_match[] = {
	{ .compatible = "nt,slot_detect", },
	{ },
};

MODULE_DEVICE_TABLE(of, slot_detect_of_match);

static struct platform_driver slot_detect_class_driver = {
	.probe		= slot_detect_class_probe,
	.driver		= {
		.name	= "slot_detect_class",
		.of_match_table = slot_detect_of_match,
	}
};
static int __init slot_detect_init(void)
{
	return platform_driver_register(&slot_detect_class_driver);
}

static void __exit slot_detect_exit(void)
{
	class_unregister(&slot_detect_class);
	platform_driver_unregister(&slot_detect_class_driver);
	kfree(slot);
}


module_init(slot_detect_init);
module_exit(slot_detect_exit);
MODULE_DESCRIPTION("nt sim slot detect function");
MODULE_LICENSE("GPL v2");