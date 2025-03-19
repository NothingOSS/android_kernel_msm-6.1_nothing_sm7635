#include "haptic_util.h"

static int32_t sampling_x = 0;

void ics_resample_reset(void)
{
	sampling_x = 0;
}

int32_t ics_resample(
	const int8_t* src_buf,
	int32_t src_buf_size,
	int32_t src_f0,
	int8_t* dst_buf,
	int32_t dst_buf_size,
	int32_t dst_f0)
{
	int32_t src_index = 1;
	int32_t dst_index = 0;

	int8_t pt1_y = src_buf[0];
	int8_t pt2_y = src_buf[1];
	int32_t step = dst_f0 * FIXED_BASE / src_f0;

	if (src_buf_size < 2)
	{
		return 0;
	}

	while (src_index < src_buf_size)
	{
		 while (sampling_x >= FIXED_BASE)
		{
			 ++src_index;
			if (src_index >= src_buf_size)
			{
				return dst_index;
			}
			pt1_y = pt2_y;
			pt2_y = src_buf[src_index];
			sampling_x -= FIXED_BASE;
		}
		dst_buf[dst_index] = (int8_t)(pt1_y + (pt2_y - pt1_y) * sampling_x / FIXED_BASE);
		 ++dst_index;
		if (dst_index >= dst_buf_size)
		{
			return dst_buf_size;
		}
		sampling_x += step;
	}

	return dst_index;
}
