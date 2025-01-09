/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2021, The Linux Foundation. All rights reserved.
 * Copyright (c) 2022 Qualcomm Innovation Center, Inc. All rights reserved.
 */

#ifndef GLINKINTERFACE_H
#define GLINKINTERFACE_H

#include <linux/delay.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/rpmsg.h>


#define	TOUCH_GLINK_INTENT_SIZE	0x0c
#define TOUCH_MSG_SIZE 0x08
#define TIMEOUT_MS 2000

struct glink_touch_priv {
 	void *handle;
 	struct mutex glink_mutex;
 	struct mutex touch_state_mutex;
 	void *lhndl;
 	char rx_buf[TOUCH_GLINK_INTENT_SIZE];
 	bool glink_touch_cmplt;
	wait_queue_head_t link_state_wait;
 	bool msm_touch_rpmsg;
};

static void *glink_touch_drv;

enum touch_slate_cmds {
	TOUCH_ENTER_PREPARE = 0x1100,
	TOUCH_ENTER,
	TOUCH_EXIT_PREPARE,
	TOUCH_EXIT
};

struct glink_touch_dev {
	struct rpmsg_endpoint *channel;
	struct device *dev;
	bool chnl_state;
	void *message;
	size_t message_length;
};

struct touch_channel_ops {
	void (*glink_channel_state)(bool state);
	void (*rx_msg)(void *data, int len);
};

void glink_touch_channel_init(void (*fn1)(bool), void (*fn2)(void *, int));



#if IS_ENABLED(CONFIG_MSM_SLATERSB_RPMSG)
int glink_touch_tx_msg(void  *msg, size_t len);
#else
static inline int glink_touch_tx_msg(void  *msg, size_t len)
{
	return -EIO;
}
#endif

#endif
