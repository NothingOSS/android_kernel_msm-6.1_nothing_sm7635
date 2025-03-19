#ifndef __AAC_RICHTAP_DRV_H__
#define __AAC_RICHTAP_DRV_H__

#include <linux/types.h>
#include "haptic_drv.h"

enum
{
	MMAP_BUF_DATA_VALID = 0x55,
	MMAP_BUF_DATA_FINISHED = 0xAA,
	MMAP_BUF_DATA_INVALID = 0xFF,
};

#define RICHTAP_IOCTL_GROUP 0x52
#define RICHTAP_GET_HWINFO		  _IO(RICHTAP_IOCTL_GROUP, 0x03)
#define RICHTAP_SET_FREQ			_IO(RICHTAP_IOCTL_GROUP, 0x04)
#define RICHTAP_SETTING_GAIN		_IO(RICHTAP_IOCTL_GROUP, 0x05)
#define RICHTAP_OFF_MODE			_IO(RICHTAP_IOCTL_GROUP, 0x06)
#define RICHTAP_TIMEOUT_MODE		_IO(RICHTAP_IOCTL_GROUP, 0x07)
#define RICHTAP_RAM_MODE			_IO(RICHTAP_IOCTL_GROUP, 0x08)
#define RICHTAP_RTP_MODE			_IO(RICHTAP_IOCTL_GROUP, 0x09)
#define RICHTAP_STREAM_MODE		 _IO(RICHTAP_IOCTL_GROUP, 0x0A)
#define RICHTAP_UPDATE_RAM		  _IO(RICHTAP_IOCTL_GROUP, 0x10)
#define RICHTAP_GET_F0			  _IO(RICHTAP_IOCTL_GROUP, 0x11)
#define RICHTAP_STOP_MODE		   _IO(RICHTAP_IOCTL_GROUP, 0x12)
#define RICHTAP_DEBUG_INFO		  _IO(RICHTAP_IOCTL_GROUP, 0x18)

#define RICHTAP_MMAP_BUF_SIZE	   1000
#define RICHTAP_MMAP_PAGE_ORDER	 2
#define RICHTAP_MMAP_BUF_SUM		16
#define RICHTAP_RTP_BUF_SIZE		1024
#define RICHTAP_DEBUG_INFO_SIZE	 128

#define RICHTAP_ICS_RT6010		  0x07

#pragma pack(4)
struct mmap_buf_format
{
	uint8_t status;
	uint8_t bit;
	int16_t length;
	uint32_t reserve;
	struct mmap_buf_format *kernel_next;
	struct mmap_buf_format *user_next;
	uint8_t data[RICHTAP_MMAP_BUF_SIZE];
};
#pragma pack()

struct aac_richtap_data
{
	uint8_t *richtap_ptr;
	struct mmap_buf_format *start_buf;
	struct mmap_buf_format *curr_buf;
	int16_t pos;
	atomic_t richtap_stream_mode;
};

#define DEFAULT_RICHTAP_NAME    "aac_richtap"
int32_t richtap_misc_register(struct ics_haptic_data *haptic_data);
int32_t richtap_misc_remove(struct ics_haptic_data *haptic_data);
int32_t richtap_irq_handler(void *data);

#endif // __AAC_RICHTAP_DRV_H__
