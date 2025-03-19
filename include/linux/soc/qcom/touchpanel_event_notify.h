#ifndef __TOUCHPANEL_EVENT_NOTIFY_H
#define __TOUCHPANEL_EVENT_NOTIFY_H
#include <linux/notifier.h>

struct touchpanel_coordinate {
	int x;
	int y;
};
typedef enum {
	//TOUCHPANEL_EVENT_NOTIFIER_EVENT_SCREEN_OFF,
	//TOUCHPANEL_EVENT_NOTIFIER_EVENT_SCREEN_ON,
	TOUCHPANEL_EVENT_NOTIFIER_EVENT_FINGER_DOWN,
	TOUCHPANEL_EVENT_NOTIFIER_EVENT_FINGER_UP,
	//TOUCHPANEL_EVENT_NOTIFIER_EVENT_UI_READY,
	//TOUCHPANEL_EVENT_NOTIFIER_EVENT_UI_DISAPPEAR,
} touchpanel_event_status;

int touchpanel_event_register_client(struct notifier_block *nb);
int touchpanel_event_unregister_client(struct notifier_block *nb);
int touchpanel_event_call_notifier(unsigned long val, void *data);

#endif	/* __TOUCHPANEL_EVENT_NOTIFY_H */
