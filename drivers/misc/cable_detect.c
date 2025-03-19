#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/device.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#define CLASS_NAME "cable_detect"

struct cable_detect_info {
	struct platform_device *pdev;
	struct class *cable_detect_class;
	int ant0;
	int ant2;
};

static char *detect_result[] = {"exist","absent"};

static struct cable_detect_info *cable;

static ssize_t ant0_show(struct class *class, struct class_attribute *attr,
                            char *buf)
{
	int value;
	value = gpio_get_value(cable->ant0);
	return sprintf(buf, "%s\n",detect_result[value]);
}

static ssize_t ant2_show(struct class *class, struct class_attribute *attr,
                            char *buf)
{
	int value;
	value = gpio_get_value(cable->ant2);
	return sprintf(buf, "%s\n",detect_result[value]);
}

static CLASS_ATTR_RO(ant0);
static CLASS_ATTR_RO(ant2);

static struct attribute *cable_detect_class_attrs[] = {
		&class_attr_ant0.attr,
		&class_attr_ant2.attr,
		NULL,
};

ATTRIBUTE_GROUPS(cable_detect_class);

static struct class cable_detect_class = {
	.name = CLASS_NAME,
	.owner = THIS_MODULE,
	.class_groups = cable_detect_class_groups,
};

static int cable_detect_parse_dt(void)
{
	cable->ant0 = of_get_named_gpio(cable->pdev->dev.of_node,"ant0_present",0);
	if(cable->ant0 < 0){
		dev_err(&cable->pdev->dev,"can not get ant0 present gpio");
		return -EINVAL;
	}

	cable->ant2 = of_get_named_gpio(cable->pdev->dev.of_node,"ant2_present",0);
	if(cable->ant2 < 0){
		dev_err(&cable->pdev->dev,"can not get ant2 present gpio");
		return -EINVAL;
	}
	return 0;
}

static int cable_detect_class_probe(struct platform_device *pdev){

	int ret;

	cable = kzalloc(sizeof(struct cable_detect_info), GFP_KERNEL);
	cable->pdev = pdev;
	cable->cable_detect_class = &cable_detect_class;

	ret = cable_detect_parse_dt();
	if(ret){
		dev_err(&cable->pdev->dev,"cable_detect_parse_dt failed");
		goto err;
	}

	ret = class_register(&cable_detect_class);
	if(ret){
		dev_err(&cable->pdev->dev,"class_register fail\n");
		goto err;
	}

	return ret;
err:
	kfree(cable);
	return ret;
}

static const struct of_device_id cable_detect_of_match[] = {
	{ .compatible = "nt,cable_detect", },
	{ },
};

MODULE_DEVICE_TABLE(of, cable_detect_of_match);

static struct platform_driver cable_detect_class_driver = {
	.probe		= cable_detect_class_probe,
	.driver		= {
		.name	= "cable_detect_class",
		.of_match_table = cable_detect_of_match,
	}
};
static int __init cable_detect_init(void)
{
	return platform_driver_register(&cable_detect_class_driver);
}

static void __exit cable_detect_exit(void)
{
	class_unregister(&cable_detect_class);
	platform_driver_unregister(&cable_detect_class_driver);
	kfree(cable);
}


module_init(cable_detect_init);
module_exit(cable_detect_exit);
MODULE_DESCRIPTION("nt ant cable detect function");
MODULE_LICENSE("GPL v2");