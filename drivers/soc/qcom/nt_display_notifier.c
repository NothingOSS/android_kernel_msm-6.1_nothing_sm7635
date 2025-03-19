// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2021, The Linux Foundation. All rights reserved.
 *
 */

#include <linux/notifier.h>
#include <linux/module.h>
#include <linux/export.h>
#include <linux/soc/qcom/nt_display_notifier.h>

static BLOCKING_NOTIFIER_HEAD(nt_display_event_notifier_list);

/**
 *	nt_display_event_register_notifier - register a client notifier
 *	@nb: notifier block to callback on events
 */

int nt_display_event_register_notifier(struct notifier_block *nb)
{
	return blocking_notifier_chain_register(&nt_display_event_notifier_list, nb);
}
EXPORT_SYMBOL(nt_display_event_register_notifier);

/**
 *	nt_display_event_unregister_client - unregister a client notifier
 *	@nb: notifier block to callback on events
 */
int nt_display_event_unregister_notifier(struct notifier_block *nb)
{
	return blocking_notifier_chain_unregister(&nt_display_event_notifier_list, nb);
}
EXPORT_SYMBOL(nt_display_event_unregister_notifier);

/**
 *  nt_display_event_notifier_call_chain - notify clients of fb_events
 */
int nt_display_event_call_notifier(unsigned long val, void *data)
{
	return blocking_notifier_call_chain(&nt_display_event_notifier_list, val, data);
}

/**
 *  nt_display_update_lcm_state_to_fingerprint - update ui&hbm status to fingerprint
 */
void nt_display_update_lcm_state_to_fingerprint(int hbm_status)
{
	struct fp_notify_event event;

	event.hbm_status = hbm_status;
	event.ui_status = 1;

	nt_display_event_call_notifier(FP_NOTIFIER_EVENT_UI, &event);

}
EXPORT_SYMBOL(nt_display_update_lcm_state_to_fingerprint);
MODULE_LICENSE("GPL v2");