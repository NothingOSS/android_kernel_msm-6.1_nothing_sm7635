#ifndef __ICS_SFDC_DRV_H__
#define __ICS_SFDC_DRV_H__

#include <linux/types.h>
#include "haptic_drv.h"

#define SFDC_REG_USER_SIG           _IO(0x40, 0x20)
#define SFDC_UNREG_USER_SIG         _IO(0x40, 0x21)

int32_t sfdc_misc_register(struct ics_haptic_data *haptic_data);
int32_t sfdc_misc_remove(struct ics_haptic_data *haptic_data);
int32_t sfdc_bemf_daq_clear(void);
int32_t sfdc_wakeup_bemf_daq_poll(void);

#endif // __ICS_SFDC_DRV_H__
