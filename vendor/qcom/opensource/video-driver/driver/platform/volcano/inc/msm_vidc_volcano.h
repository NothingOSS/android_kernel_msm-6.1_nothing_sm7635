/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2020-2022, The Linux Foundation. All rights reserved.
 * Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
 */

#ifndef _MSM_VIDC_VOLCANO_H_
#define _MSM_VIDC_VOLCANO_H_

#include "msm_vidc_core.h"

#if defined(CONFIG_MSM_VIDC_VOLCANO)
int msm_vidc_init_platform_volcano(struct msm_vidc_core *core);
int msm_vidc_deinit_platform_volcano(struct msm_vidc_core *core);
#else
int msm_vidc_init_platform_volcano(struct msm_vidc_core *core)
{
	return -EINVAL;
}

int msm_vidc_deinit_platform_volcano(struct msm_vidc_core *core)
{
	return -EINVAL;
}
#endif

#endif // _MSM_VIDC_VOLCANO_H_
