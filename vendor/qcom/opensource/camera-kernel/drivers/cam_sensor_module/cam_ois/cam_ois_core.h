/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2017-2019, The Linux Foundation. All rights reserved.
 * Copyright (c) 2022 Qualcomm Innovation Center, Inc. All rights reserved.
 */
#ifndef _CAM_OIS_CORE_H_
#define _CAM_OIS_CORE_H_

#include <linux/cma.h>
#include "cam_ois_dev.h"

#define OIS_NAME_LEN 32
#define OIS_ENDIANNESS_MASK_FW              0x0F
#define OIS_ENDIANNESS_MASK_INPUTPARAM      0xF0


/**
 * @power_info: power setting info to control the power
 *
 * This API construct the default ois power setting.
 *
 * @return Status of operation. Negative in case of error. Zero otherwise.
 */
int32_t cam_ois_construct_default_power_setting(
	struct cam_sensor_power_ctrl_t *power_info);


int cam_ois_driver_cmd(struct cam_ois_ctrl_t *e_ctrl, void *arg);

/**
 * @o_ctrl: OIS ctrl structure
 *
 * This API handles the shutdown ioctl/close
 */
void cam_ois_shutdown(struct cam_ois_ctrl_t *o_ctrl);

struct completion *cam_ois_get_i3c_completion(uint32_t index);

// xft add for new_feature tele ois read more than one reg
// add func cam_ois_i2c_read_data @{
int32_t cam_ois_i2c_read_data(
	struct i2c_settings_array *i2c_settings,
	struct camera_io_master *io_master_info);
// @}

#endif
/* _CAM_OIS_CORE_H_ */
