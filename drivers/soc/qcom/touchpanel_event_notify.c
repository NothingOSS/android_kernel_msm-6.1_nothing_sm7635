#include <linux/soc/qcom/touchpanel_event_notify.h>
#include <linux/notifier.h>
#include <linux/module.h>
#include <linux/export.h>

static BLOCKING_NOTIFIER_HEAD(touchpanel_event_notifier_list);

/**
 *	touchpanel_event_register_client - register a client notifier
 *	@nb: notifier block to callback on events
 */

int touchpanel_event_register_client(struct notifier_block *nb)
{
	return blocking_notifier_chain_register(&touchpanel_event_notifier_list, nb);
}
EXPORT_SYMBOL(touchpanel_event_register_client);

/**
 *	touchpanel_event_unregister_client - unregister a client notifier
 *	@nb: notifier block to callback on events
 */
int touchpanel_event_unregister_client(struct notifier_block *nb)
{
	return blocking_notifier_chain_unregister(&touchpanel_event_notifier_list, nb);
}
EXPORT_SYMBOL(touchpanel_event_unregister_client);
/**
 * touchpanel_event_notifier_event - notify clients of fb_events
 *
 */
int touchpanel_event_call_notifier(unsigned long val, void *data)
{
	return blocking_notifier_call_chain(&touchpanel_event_notifier_list, val, data);
}
EXPORT_SYMBOL_GPL(touchpanel_event_call_notifier);

MODULE_LICENSE("GPL");

