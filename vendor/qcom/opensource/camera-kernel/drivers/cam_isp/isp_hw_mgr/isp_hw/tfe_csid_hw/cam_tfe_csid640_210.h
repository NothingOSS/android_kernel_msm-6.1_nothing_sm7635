/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2019-2021, The Linux Foundation. All rights reserved.
 * Copyright (c) 2022-2023 Qualcomm Innovation Center, Inc. All rights reserved.
 */

#ifndef _CAM_TFE_CSID_640_210_H_
#define _CAM_TFE_CSID_640_210_H_

#include "cam_tfe_csid_core.h"
#include "cam_tfe_csid640.h"

#define CAM_TFE_CSID_VERSION_V640_210               0x60040000

static struct cam_tfe_csid_hw_info cam_tfe_csid640_210_hw_info = {
	.csid_reg = &cam_tfe_csid_640_reg_offset,
	.hw_dts_version = CAM_TFE_CSID_VERSION_V640_210,
};

#endif /*_CAM_TFE_CSID_640_210_H_ */
