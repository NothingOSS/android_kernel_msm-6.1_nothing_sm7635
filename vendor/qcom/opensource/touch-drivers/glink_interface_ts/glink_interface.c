// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2021, The Linux Foundation. All rights reserved.
 * Copyright (c) 2022 Qualcomm Innovation Center, Inc. All rights reserved.
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <glink_interface.h>

static struct glink_touch_dev *touch_pdev = NULL;
struct touch_channel_ops touch_ops;


void glink_touch_channel_init(void (*fn1)(bool), void (*fn2)(void *data, int len))
{
	touch_ops.glink_channel_state = fn1;
	touch_ops.rx_msg = fn2;
}
EXPORT_SYMBOL(glink_touch_channel_init);

int glink_touch_tx_msg(void  *msg, size_t len)
{
	int ret = 0;

	if (touch_pdev == NULL || !touch_pdev->chnl_state) {
		pr_err("pmsg_device is null, channel is closed\n");
		return -ENETRESET;
	}

	touch_pdev->message = msg;
	touch_pdev->message_length = len;
	if (touch_pdev->message) {
		ret = rpmsg_send(touch_pdev->channel,
			touch_pdev->message, touch_pdev->message_length);
		if (ret)
			pr_err("rpmsg_send failed: %d\n", ret);

	}

	return ret;
}
EXPORT_SYMBOL(glink_touch_tx_msg);

static int glink_touch_probe(struct rpmsg_device  *touch_rpdev)
{
	int ret = 0;
	void *msg = NULL;

	if (!touch_rpdev) {
		pr_err("rpmsg_device is null\n");
		return -ENODEV;
	}

	pr_info("%s Start of glink_touch_probe\n", __func__);
	touch_pdev = devm_kzalloc(&touch_rpdev->dev, sizeof(*touch_pdev), GFP_KERNEL);
	if (!touch_pdev)
		return -ENOMEM;

	touch_pdev->channel = touch_rpdev->ept;
	touch_pdev->dev = &touch_rpdev->dev;
	if (touch_pdev->channel == NULL)
		return -ENOMEM;
		
	touch_pdev->chnl_state = true;
	dev_set_drvdata(&touch_rpdev->dev, touch_pdev);
	
	/* send a callback to slate-MSM touch driver*/
	touch_ops.glink_channel_state(true);
	if (touch_pdev->message == NULL)
		ret = glink_touch_tx_msg(msg, 0);
	
	pr_info("%s End of glink_touch_probe\n", __func__);
	return 0;
}

static void glink_touch_remove(struct rpmsg_device *touch_rpdev)
{
	touch_pdev->chnl_state = false;
	touch_pdev->message = NULL;
	dev_dbg(&touch_rpdev->dev, "rpmsg client driver is removed\n");
	touch_ops.glink_channel_state(false);
	dev_set_drvdata(&touch_rpdev->dev, NULL);
	
}

static int glink_touch_cb(struct rpmsg_device *touch_rpdev,
				void *data, int len, void *priv, u32 src)
{
	struct glink_touch_dev *touch_dev =
			dev_get_drvdata(&touch_rpdev->dev);

	if (!touch_dev)
		return -ENODEV;
	touch_ops.rx_msg(data, len);

	return 0;
}

static const struct rpmsg_device_id glink_touch_driver_id_table[] = {
	{ "touch-ctrl" },
	{},
};
MODULE_DEVICE_TABLE(rpmsg, glink_touch_driver_id_table);

static const struct of_device_id glink_touch_driver_of_match[] = {
	{ .compatible = "qcom,slatetouch-rpmsg" },
	{},
};

static struct rpmsg_driver glink_touch_client = {
	.id_table = glink_touch_driver_id_table,
	.probe = glink_touch_probe,
	.callback = glink_touch_cb,
	.remove = glink_touch_remove,
	.drv = {
		.name = "glink_interface",
		.of_match_table = glink_touch_driver_of_match,
	},
};
module_rpmsg_driver(glink_touch_client);

MODULE_DESCRIPTION("Interface Driver for MSM TOUCH and RPMSG");
MODULE_LICENSE("GPL v2");
