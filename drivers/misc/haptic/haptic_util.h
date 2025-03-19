#ifndef __ICS_HAPTIC_UTIL_H__
#define __ICS_HAPTIC_UTIL_H__

#include <linux/types.h>

#define UNUSED(x)                   ((void)(x))
#define FIXED_BASE  65536

void ics_resample_reset(void);
int32_t ics_resample(
	const int8_t* src_buf,
	int32_t src_buf_size,
	int32_t src_f0,
	int8_t* dst_buf,
	int32_t dst_buf_size,
	int32_t dst_f0);
#endif // __ICS_HAPTIC_UTIL_H__
